/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"

#include <iostream>

using namespace ns3;

namespace ns3
{

class AquaSimUWAodvRouteFreshnessTest
{
public:
  static bool Run()
  {
    AquaSimUWAodvRouting routing;
    bool ok = true;

    AquaSimUWAodvRouting::RouteEntry current;
    current.nextHop = AquaSimAddress(2);
    current.hopCount = 3;
    current.destSeqNo = 10;
    current.validSeqNo = true;
    current.valid = true;
    current.expire = Simulator::Now() + Seconds(30);

    ok &= Check(routing.ShouldUpdateRoute(current, 11, true, 5),
                "newer destination sequence number should replace older route");
    ok &= Check(routing.ShouldUpdateRoute(current, 10, true, 2),
                "same destination sequence number with shorter hop count should replace route");
    ok &= Check(!routing.ShouldUpdateRoute(current, 10, true, 3),
                "same destination sequence number with same hop count should not replace route");
    ok &= Check(!routing.ShouldUpdateRoute(current, 9, true, 1),
                "older destination sequence number should not replace fresher route");
    ok &= Check(!routing.ShouldUpdateRoute(current, 0, false, 1),
                "unknown destination sequence number should not replace known fresh route");

    AquaSimUWAodvRouting::RouteEntry noSeq = current;
    noSeq.validSeqNo = false;
    ok &= Check(routing.ShouldUpdateRoute(noSeq, 1, true, 4),
                "known incoming sequence number should replace route without valid sequence number");
    ok &= Check(routing.ShouldUpdateRoute(noSeq, 0, false, 2),
                "unknown sequence number may improve route without valid sequence number if hop count is shorter");
    ok &= Check(!routing.ShouldUpdateRoute(noSeq, 0, false, 4),
                "unknown sequence number should not worsen route without valid sequence number");

    AquaSimUWAodvRouting::RouteEntry invalid = current;
    invalid.valid = false;
    ok &= Check(routing.ShouldUpdateRoute(invalid, 1, true, 10),
                "invalid current route should be replaceable");

    AquaSimUWAodvRouting::RouteEntry expired = current;
    expired.expire = Simulator::Now() - Seconds(1);
    ok &= Check(routing.ShouldUpdateRoute(expired, 1, false, 10),
                "expired current route should be replaceable");

    ok &= Check(routing.IsSeqNoNewer(11, 10), "simple sequence increment should be newer");
    ok &= Check(!routing.IsSeqNoNewer(10, 11), "lower sequence number should not be newer");
    ok &= Check(routing.IsSeqNoNewer(1, 0xffffffff),
                "sequence number comparison should handle 32-bit wrap-around");

    return ok;
  }

private:
  static bool Check(bool condition, const std::string& message)
  {
    if (!condition)
      {
        std::cerr << "UW-AODV route freshness check failed: " << message << std::endl;
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

  bool ok = AquaSimUWAodvRouteFreshnessTest::Run();
  std::cout << "UW-AODV route freshness check: " << (ok ? "PASS" : "FAIL") << std::endl;
  return ok ? 0 : 1;
}
