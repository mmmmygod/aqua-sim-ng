/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"

#include <iostream>

using namespace ns3;

namespace ns3
{

class AquaSimUWAodvControlFlagsTest
{
public:
  static bool Run()
  {
    AquaSimUWAodvRouting routing;
    bool ok = true;

    ok &= Check(routing.m_gratuitousReply,
                "GratuitousReply should default to enabled like ns-3 AODV");
    ok &= Check(!routing.m_destinationOnly,
                "DestinationOnly should default to disabled like ns-3 AODV");
    ok &= Check(routing.m_enableBroadcast,
                "EnableBroadcast should default to enabled like ns-3 AODV");

    AquaSimUWAodvRouting::RouteEntry freshRoute;
    freshRoute.nextHop = AquaSimAddress(4);
    freshRoute.hopCount = 1;
    freshRoute.destSeqNo = 20;
    freshRoute.validSeqNo = true;
    freshRoute.state = AquaSimUWAodvRouting::ROUTE_VALID;
    freshRoute.valid = true;
    freshRoute.expire = Simulator::Now() + Seconds(30);

    AquaSimUWAodvHeader rreq;
    rreq.SetType(AquaSimUWAodvHeader::UWAODV_RREQ);
    rreq.SetOrigin(AquaSimAddress(1));
    rreq.SetDestination(AquaSimAddress(9));
    rreq.SetOriginSeqNo(3);
    rreq.SetDestSeqNo(10);
    rreq.SetUnknownDestSeqNo(false);

    ok &= Check(routing.CanSendIntermediateRrep(freshRoute, rreq, AquaSimAddress(2)),
                "fresh route should allow intermediate RREP by default");
    ok &= Check(!routing.CanSendIntermediateRrep(freshRoute, rreq, AquaSimAddress(4)),
                "intermediate RREP should be suppressed when route next hop is the RREQ previous hop");
    ok &= Check(routing.ShouldDropRreqToAvoidLoop(freshRoute, AquaSimAddress(4)),
                "RREQ should be dropped when destination route next hop is the previous hop");
    ok &= Check(!routing.ShouldDropRreqToAvoidLoop(freshRoute, AquaSimAddress(2)),
                "RREQ should not be dropped when destination route uses a different next hop");
    ok &= Check(!routing.ShouldSendGratuitousRrep(freshRoute, rreq, AquaSimAddress(2)),
                "gratuitous RREP should not be sent unless requested");

    rreq.SetGratuitousRrep(true);
    ok &= Check(routing.ShouldSendGratuitousRrep(freshRoute, rreq, AquaSimAddress(2)),
                "gratuitous RREP should be sent after a valid intermediate reply when requested");

    rreq.SetDestinationOnly(true);
    ok &= Check(!routing.CanSendIntermediateRrep(freshRoute, rreq, AquaSimAddress(2)),
                "DestinationOnly should prevent intermediate RREP");
    ok &= Check(!routing.ShouldSendGratuitousRrep(freshRoute, rreq, AquaSimAddress(2)),
                "DestinationOnly should also prevent gratuitous RREP");
    ok &= Check(routing.ShouldRefreshForwardedRreqDestSeqNo(freshRoute, rreq, AquaSimAddress(2)),
                "DestinationOnly should still refresh forwarded RREQ destination sequence number");

    AquaSimUWAodvRouting::RouteEntry staleRoute = freshRoute;
    staleRoute.destSeqNo = 9;
    ok &= Check(!routing.CanSendIntermediateRrep(staleRoute, rreq, AquaSimAddress(2)),
                "stale intermediate route should not reply");
    ok &= Check(!routing.ShouldRefreshForwardedRreqDestSeqNo(staleRoute, rreq, AquaSimAddress(2)),
                "stale route should not refresh forwarded RREQ destination sequence number");

    AquaSimUWAodvRouting::RouteEntry invalidRoute = freshRoute;
    invalidRoute.state = AquaSimUWAodvRouting::ROUTE_INVALID;
    invalidRoute.valid = false;
    ok &= Check(!routing.ShouldDropRreqToAvoidLoop(invalidRoute, AquaSimAddress(4)),
                "invalid remembered route should not suppress new RREQ forwarding");

    return ok;
  }

private:
  static bool Check(bool condition, const std::string& message)
  {
    if (!condition)
      {
        std::cerr << "UW-AODV control flags check failed: " << message << std::endl;
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

  bool ok = AquaSimUWAodvControlFlagsTest::Run();
  std::cout << "UW-AODV control flags check: " << (ok ? "PASS" : "FAIL") << std::endl;
  return ok ? 0 : 1;
}
