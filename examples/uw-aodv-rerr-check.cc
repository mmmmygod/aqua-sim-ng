/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"

#include <iostream>
#include <set>

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

    AquaSimUWAodvRouting::RouteEntry route;
    route.nextHop = AquaSimAddress(4);
    route.hopCount = 2;
    route.destSeqNo = 10;
    route.validSeqNo = true;
    route.valid = true;
    route.expire = Simulator::Now() + Seconds(30);
    route.precursors.insert(AquaSimAddress(1));
    route.precursors.insert(AquaSimAddress(3));
    routing.m_routeTable[AquaSimAddress(5)] = route;

    ok &= Check(!routing.ShouldAcceptRerr(AquaSimAddress(5), 9),
                "older RERR sequence number should be ignored");
    ok &= Check(routing.ShouldAcceptRerr(AquaSimAddress(5), 10),
                "equal RERR sequence number should be accepted");
    ok &= Check(routing.ShouldAcceptRerr(AquaSimAddress(5), 11),
                "newer RERR sequence number should be accepted");

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
    ok &= Check(invalidated.validSeqNo && invalidated.destSeqNo == 11,
                "route should keep the fresh RERR destination sequence number");

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
