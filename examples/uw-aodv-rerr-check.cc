/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"

#include <iostream>
#include <set>
#include <vector>

using namespace ns3;

namespace ns3
{

class AquaSimUWAodvRerrTest
{
public:
  static bool Run()
  {
    AquaSimUWAodvRouting routing;
    bool ok = true;

    AquaSimUWAodvHeader tx;
    tx.SetType(AquaSimUWAodvHeader::UWAODV_RERR);
    tx.SetHopCount(0);
    tx.SetRequestId(0);
    tx.SetOrigin(AquaSimAddress(2));
    tx.SetDestination(AquaSimAddress(5));
    tx.SetOriginSeqNo(12);
    tx.SetDestSeqNo(44);
    tx.SetLifetime(0);

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(tx);

    AquaSimUWAodvHeader rx;
    packet->RemoveHeader(rx);

    ok &= Check(rx.GetType() == AquaSimUWAodvHeader::UWAODV_RERR,
                "RERR packet type should serialize and deserialize");
    ok &= Check(rx.GetDestination() == AquaSimAddress(5),
                "RERR unreachable destination mismatch");
    ok &= Check(rx.GetDestSeqNo() == 44,
                "RERR unreachable destination sequence number mismatch");

    AquaSimUWAodvRerrListHeader listTx;
    listTx.AddUnreachableDestination(AquaSimAddress(5), 44);
    listTx.AddUnreachableDestination(AquaSimAddress(6), 45);

    Ptr<Packet> listPacket = Create<Packet>();
    listPacket->AddHeader(listTx);

    AquaSimUWAodvRerrListHeader listRx;
    listPacket->RemoveHeader(listRx);
    const std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination>& unreachable =
      listRx.GetUnreachableDestinations();

    ok &= Check(unreachable.size() == 2,
                "RERR unreachable destination list should preserve entry count");
    ok &= Check(unreachable[0].first == AquaSimAddress(5) && unreachable[0].second == 44,
                "first RERR list entry mismatch");
    ok &= Check(unreachable[1].first == AquaSimAddress(6) && unreachable[1].second == 45,
                "second RERR list entry mismatch");

    AquaSimUWAodvRouting::RouteEntry route;
    route.nextHop = AquaSimAddress(4);
    route.hopCount = 2;
    route.destSeqNo = 10;
    route.validSeqNo = true;
    route.state = AquaSimUWAodvRouting::ROUTE_VALID;
    route.valid = true;
    route.expire = Simulator::Now() + Seconds(30);
    route.precursors.insert(AquaSimAddress(1));
    route.precursors.insert(AquaSimAddress(3));
    routing.m_routeTable[AquaSimAddress(5)] = route;

    ok &= Check(!routing.ShouldAcceptRerr(AquaSimAddress(5), 9, AquaSimAddress(4)),
                "older RERR sequence number should be ignored");
    ok &= Check(routing.ShouldAcceptRerr(AquaSimAddress(5), 10, AquaSimAddress(4)),
                "equal RERR sequence number should be accepted");
    ok &= Check(routing.ShouldAcceptRerr(AquaSimAddress(5), 11, AquaSimAddress(4)),
                "newer RERR sequence number should be accepted");
    ok &= Check(!routing.ShouldAcceptRerr(AquaSimAddress(5), 11, AquaSimAddress(6)),
                "RERR from a node that is not the current next hop should be ignored");

    std::set<AquaSimAddress> precursors =
      routing.InvalidateRoute(AquaSimAddress(5), 11, true);
    AquaSimUWAodvRouting::RouteEntry invalidated =
      routing.m_routeTable[AquaSimAddress(5)];

    ok &= Check(precursors.size() == 2,
                "route invalidation should return the route precursor set");
    ok &= Check(precursors.find(AquaSimAddress(1)) != precursors.end() &&
                precursors.find(AquaSimAddress(3)) != precursors.end(),
                "route invalidation should preserve precursor addresses");
    ok &= Check(!invalidated.valid, "route should be marked invalid");
    ok &= Check(invalidated.state == AquaSimUWAodvRouting::ROUTE_INVALID,
                "route state should be marked INVALID");
    ok &= Check(invalidated.expire == Simulator::Now() + routing.m_deletePeriod,
                "invalidated route should be retained until DeletePeriod");
    ok &= Check(invalidated.validSeqNo && invalidated.destSeqNo == 11,
                "route should keep the fresh RERR destination sequence number");
    ok &= Check(!routing.ShouldAcceptRerr(AquaSimAddress(5), 12, AquaSimAddress(4)),
                "RERR for an already invalid route should be ignored");

    AquaSimUWAodvRouting precursorRouting;
    AquaSimUWAodvRouting::RouteEntry precursorRoute;
    precursorRoute.state = AquaSimUWAodvRouting::ROUTE_VALID;
    precursorRoute.valid = true;
    precursorRoute.expire = Simulator::Now() + Seconds(30);
    precursorRouting.m_routeTable[AquaSimAddress(1)] = precursorRoute;
    precursorRouting.m_routeTable[AquaSimAddress(2)] = precursorRoute;
    precursorRouting.m_routeTable[AquaSimAddress(4)] = precursorRoute;
    precursorRouting.m_routeTable[AquaSimAddress(9)] = precursorRoute;

    precursorRouting.AddPrecursor(AquaSimAddress(9), AquaSimAddress(2));
    precursorRouting.AddPrecursor(AquaSimAddress(1), AquaSimAddress(4));
    precursorRouting.AddPrecursor(AquaSimAddress(4), AquaSimAddress(2));
    precursorRouting.AddPrecursor(AquaSimAddress(2), AquaSimAddress(4));
    ok &= Check(precursorRouting.m_routeTable[AquaSimAddress(9)].precursors.count(AquaSimAddress(2)) == 1 &&
                precursorRouting.m_routeTable[AquaSimAddress(1)].precursors.count(AquaSimAddress(4)) == 1 &&
                precursorRouting.m_routeTable[AquaSimAddress(4)].precursors.count(AquaSimAddress(2)) == 1 &&
                precursorRouting.m_routeTable[AquaSimAddress(2)].precursors.count(AquaSimAddress(4)) == 1,
                "RREP forwarding should maintain classic destination, origin, and next-hop precursor links");

    AquaSimUWAodvRouting multiRouting;
    AquaSimUWAodvRouting::RouteEntry routeA;
    routeA.nextHop = AquaSimAddress(4);
    routeA.hopCount = 2;
    routeA.destSeqNo = 20;
    routeA.validSeqNo = true;
    routeA.state = AquaSimUWAodvRouting::ROUTE_VALID;
    routeA.valid = true;
    routeA.expire = Simulator::Now() + Seconds(30);
    multiRouting.m_routeTable[AquaSimAddress(8)] = routeA;

    AquaSimUWAodvRouting::RouteEntry routeB = routeA;
    routeB.destSeqNo = 30;
    multiRouting.m_routeTable[AquaSimAddress(9)] = routeB;

    AquaSimUWAodvRouting::RouteEntry routeC = routeA;
    routeC.nextHop = AquaSimAddress(7);
    routeC.destSeqNo = 40;
    multiRouting.m_routeTable[AquaSimAddress(10)] = routeC;

    AquaSimUWAodvRerrListHeader multiList;
    multiList.AddUnreachableDestination(AquaSimAddress(8), 21);
    multiList.AddUnreachableDestination(AquaSimAddress(9), 31);
    multiList.AddUnreachableDestination(AquaSimAddress(10), 41);
    Ptr<Packet> multiPacket = Create<Packet>();
    multiPacket->AddHeader(multiList);

    AquaSimUWAodvHeader multiAodv;
    multiAodv.SetType(AquaSimUWAodvHeader::UWAODV_RERR);
    multiAodv.SetDestination(AquaSimAddress(8));
    multiAodv.SetDestSeqNo(21);

    ok &= Check(multiRouting.RecvRerr(multiPacket,
                                      AquaSimHeader(),
                                      multiAodv,
                                      AquaSimAddress(4)),
                "multi-destination RERR should be accepted when at least one route uses previous hop");
    ok &= Check(multiRouting.m_routeTable[AquaSimAddress(8)].state ==
                  AquaSimUWAodvRouting::ROUTE_INVALID &&
                !multiRouting.m_routeTable[AquaSimAddress(8)].valid &&
                multiRouting.m_routeTable[AquaSimAddress(8)].destSeqNo == 21,
                "multi-destination RERR should invalidate the first matching route");
    ok &= Check(multiRouting.m_routeTable[AquaSimAddress(9)].state ==
                  AquaSimUWAodvRouting::ROUTE_INVALID &&
                !multiRouting.m_routeTable[AquaSimAddress(9)].valid &&
                multiRouting.m_routeTable[AquaSimAddress(9)].destSeqNo == 31,
                "multi-destination RERR should invalidate the second matching route");
    ok &= Check(multiRouting.m_routeTable[AquaSimAddress(10)].state ==
                  AquaSimUWAodvRouting::ROUTE_VALID &&
                multiRouting.m_routeTable[AquaSimAddress(10)].valid,
                "multi-destination RERR should ignore routes that do not use the previous hop");

    return ok;
  }

private:
  static bool Check(bool condition, const std::string& message)
  {
    if (!condition)
      {
        std::cerr << "UW-AODV RERR check failed: " << message << std::endl;
        return false;
      }
    return true;
  }
};

} // namespace ns3

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  bool ok = AquaSimUWAodvRerrTest::Run();
  std::cout << "UW-AODV RERR check: " << (ok ? "PASS" : "FAIL") << std::endl;
  return ok ? 0 : 1;
}
