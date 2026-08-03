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
    current.state = AquaSimUWAodvRouting::ROUTE_VALID;
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
    invalid.state = AquaSimUWAodvRouting::ROUTE_INVALID;
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

    AquaSimUWAodvHeader destinationRreq;
    destinationRreq.SetUnknownDestSeqNo(false);
    routing.m_sequenceNumber = 10;
    destinationRreq.SetDestSeqNo(10);
    routing.UpdateOwnSeqNoBeforeDestinationRrep(destinationRreq);
    ok &= Check(routing.m_sequenceNumber == 10,
                "destination should not increment sequence number when RREQ carries current value");
    destinationRreq.SetDestSeqNo(11);
    routing.UpdateOwnSeqNoBeforeDestinationRrep(destinationRreq);
    ok &= Check(routing.m_sequenceNumber == 11,
                "destination should increment sequence number when RREQ carries current value plus one");
    destinationRreq.SetDestSeqNo(20);
    routing.UpdateOwnSeqNoBeforeDestinationRrep(destinationRreq);
    ok &= Check(routing.m_sequenceNumber == 11,
                "destination should not jump to a much larger RREQ destination sequence number");
    destinationRreq.SetUnknownDestSeqNo(true);
    destinationRreq.SetDestSeqNo(12);
    routing.UpdateOwnSeqNoBeforeDestinationRrep(destinationRreq);
    ok &= Check(routing.m_sequenceNumber == 11,
                "unknown RREQ destination sequence number should not update destination sequence number");

    AquaSimAddress destination(9);
    ok &= Check(routing.GetRreqHopLimit(destination, 1) == 1,
                "first expanding-ring RREQ should use TtlStart");
    ok &= Check(routing.GetRreqHopLimit(destination, 2) == 3,
                "second expanding-ring RREQ should add TtlIncrement");
    ok &= Check(routing.GetRreqHopLimit(destination, 3) == 5,
                "third expanding-ring RREQ should keep expanding below TtlThreshold");
    ok &= Check(routing.GetRreqHopLimit(destination, 4) == 7,
                "fourth expanding-ring RREQ may reach TtlThreshold");
    ok &= Check(routing.GetRreqHopLimit(destination, 5) == 32,
                "expanding-ring RREQ should switch to NetDiameter after TtlThreshold");
    ok &= Check(routing.GetRreqTimeout(1) == Seconds(3),
                "RREQ timeout should include node traversal time and timeout buffer for TTL=1");
    ok &= Check(routing.GetRreqTimeout(5) == Seconds(7),
                "RREQ timeout should grow with the RREQ hop limit");
    ok &= Check(routing.GetNetTraversalTime() == Seconds(32),
                "NetTraversalTime should follow 2 * NodeTraversalTime * NetDiameter");
    ok &= Check(routing.GetPathDiscoveryTime() == Seconds(64),
                "PathDiscoveryTime should follow 2 * NetTraversalTime");
    ok &= Check(routing.GetMyRouteTimeout() == Seconds(240),
                "automatic MyRouteTimeout should be 2 * max(ActiveRouteTimeout, PathDiscoveryTime)");
    routing.m_myRouteTimeout = Seconds(77);
    ok &= Check(routing.GetMyRouteTimeout() == Seconds(77),
                "explicit MyRouteTimeout should override the automatic value");
    routing.m_myRouteTimeout = Seconds(0);
    ok &= Check(routing.GetNextHopWait() == MilliSeconds(510),
                "automatic NextHopWait should be 10ms + NodeTraversalTime");
    routing.m_nextHopWait = Seconds(3);
    ok &= Check(routing.GetNextHopWait() == Seconds(3),
                "explicit NextHopWait should override the automatic value");
    routing.m_nextHopWait = Seconds(0);
    ok &= Check(routing.GetBlacklistTimeout() == Seconds(64),
                "automatic BlacklistTimeout should use RreqRetries * NetTraversalTime");
    routing.m_blacklistTimeout = Seconds(7);
    ok &= Check(routing.GetBlacklistTimeout() == Seconds(7),
                "explicit BlacklistTimeout should override the automatic value");
    routing.m_blacklistTimeout = Seconds(0);
    ok &= Check(routing.GetReverseRouteLifetime(1) == Seconds(63),
                "one-hop reverse route lifetime should leave enough time for RREP return");
    ok &= Check(routing.GetReverseRouteLifetime(5) == Seconds(59),
                "reverse route lifetime should shrink with RREQ hop count");

    AquaSimUWAodvRouting::RequestKey oldRreq(AquaSimAddress(1).GetAsInt(), 1);
    AquaSimUWAodvRouting::RequestKey freshRreq(AquaSimAddress(1).GetAsInt(), 2);
    routing.m_seenRreqs[oldRreq] = Simulator::Now() - Seconds(1);
    routing.m_seenRreqs[freshRreq] = Simulator::Now() + Seconds(1);
    ok &= Check(routing.HasSeenRreq(freshRreq),
                "fresh duplicate RREQ cache entry should be recognized");
    ok &= Check(!routing.HasSeenRreq(oldRreq),
                "expired duplicate RREQ cache entry should not be recognized");
    routing.PurgeSeenRreqs();
    ok &= Check(routing.m_seenRreqs.find(oldRreq) == routing.m_seenRreqs.end(),
                "duplicate RREQ purge should remove expired entries");
    ok &= Check(routing.m_seenRreqs.find(freshRreq) != routing.m_seenRreqs.end(),
                "duplicate RREQ purge should keep unexpired entries");

    AquaSimUWAodvRouting::RequestKey rememberedRreq(AquaSimAddress(1).GetAsInt(), 3);
    routing.RememberRreq(rememberedRreq);
    ok &= Check(routing.HasSeenRreq(rememberedRreq),
                "RememberRreq should add a live duplicate cache entry");
    ok &= Check(routing.m_seenRreqs[rememberedRreq] == Simulator::Now() +
                  routing.GetPathDiscoveryTime(),
                "RememberRreq should expire entries after PathDiscoveryTime");

    Ptr<Packet> cachedDataPacket = Create<Packet>(8);
    AquaSimUWAodvRouting::DataKey oldData(AquaSimAddress(1).GetAsInt(), cachedDataPacket->GetUid());
    AquaSimUWAodvRouting::DataKey freshData(AquaSimAddress(2).GetAsInt(), cachedDataPacket->GetUid());
    routing.m_seenDataPackets[oldData] = Simulator::Now() - Seconds(1);
    routing.m_seenDataPackets[freshData] = Simulator::Now() + Seconds(1);
    ok &= Check(routing.HasSeenDataPacket(freshData),
                "fresh duplicate DATA cache entry should be recognized");
    ok &= Check(!routing.HasSeenDataPacket(oldData),
                "expired duplicate DATA cache entry should not be recognized");
    routing.PurgeSeenDataPackets();
    ok &= Check(routing.m_seenDataPackets.find(oldData) == routing.m_seenDataPackets.end(),
                "duplicate DATA purge should remove expired entries");
    ok &= Check(routing.m_seenDataPackets.find(freshData) != routing.m_seenDataPackets.end(),
                "duplicate DATA purge should keep unexpired entries");

    AquaSimUWAodvRouting::DataKey rememberedData(AquaSimAddress(3).GetAsInt(),
                                                 Create<Packet>(4)->GetUid());
    routing.RememberDataPacket(rememberedData);
    ok &= Check(routing.HasSeenDataPacket(rememberedData),
                "RememberDataPacket should add a live duplicate cache entry");
    ok &= Check(routing.m_seenDataPackets[rememberedData] == Simulator::Now() +
                  routing.GetPathDiscoveryTime(),
                "RememberDataPacket should expire entries after PathDiscoveryTime");

    routing.m_rreqRateLimit = 2;
    ok &= Check(routing.CanSendRreqNow(),
                "RREQ rate limiter should initially allow transmission");
    routing.NoteRreqSent();
    routing.NoteRreqSent();
    ok &= Check(!routing.CanSendRreqNow(),
                "RREQ rate limiter should block after the per-second limit is reached");
    ok &= Check(routing.GetRreqRateLimitDelay() == Seconds(1),
                "RREQ rate limiter should delay until the next one-second window");

    routing.m_rerrRateLimit = 1;
    ok &= Check(routing.CanSendRerrNow(),
                "RERR rate limiter should initially allow transmission");
    routing.NoteRerrSent();
    ok &= Check(!routing.CanSendRerrNow(),
                "RERR rate limiter should block after the per-second limit is reached");
    ok &= Check(routing.GetRerrRateLimitDelay() == Seconds(1),
                "RERR rate limiter should delay until the next one-second window");

    Simulator::Schedule(Seconds(1.1),
                        [&routing, &ok] {
                          ok &= Check(routing.CanSendRreqNow(),
                                      "RREQ rate limiter should reopen after one second");
                          ok &= Check(routing.CanSendRerrNow(),
                                      "RERR rate limiter should reopen after one second");
                        });

    routing.m_blacklist[AquaSimAddress(4)] = Simulator::Now() + Seconds(1);
    routing.m_blacklist[AquaSimAddress(5)] = Simulator::Now() - Seconds(1);
    ok &= Check(routing.IsBlacklisted(AquaSimAddress(4)),
                "fresh blacklist entry should block RREQ from that neighbor");
    ok &= Check(!routing.IsBlacklisted(AquaSimAddress(5)),
                "expired blacklist entry should not block RREQ from that neighbor");
    routing.PurgeBlacklist();
    ok &= Check(routing.m_blacklist.find(AquaSimAddress(4)) != routing.m_blacklist.end(),
                "blacklist purge should keep unexpired entries");
    ok &= Check(routing.m_blacklist.find(AquaSimAddress(5)) == routing.m_blacklist.end(),
                "blacklist purge should remove expired entries");

    routing.RrepAckTimeout(AquaSimAddress(6));
    ok &= Check(routing.IsBlacklisted(AquaSimAddress(6)),
                "RREP_ACK timeout should blacklist the neighbor");

    AquaSimAddress ackNeighbor(7);
    AquaSimUWAodvRouting::RouteEntry ackRoute;
    ackRoute.nextHop = ackNeighbor;
    ackRoute.hopCount = 1;
    ackRoute.state = AquaSimUWAodvRouting::ROUTE_INVALID;
    ackRoute.valid = false;
    ackRoute.expire = Simulator::Now();
    routing.m_routeTable[ackNeighbor] = ackRoute;
    AquaSimUWAodvHeader ackHeader;
    ackHeader.SetDestination(ackNeighbor);
    routing.RecvRrepAck(Create<Packet>(), AquaSimHeader(), ackHeader, ackNeighbor);
    ok &= Check(routing.m_routeTable[ackNeighbor].state == AquaSimUWAodvRouting::ROUTE_VALID,
                "RREP_ACK should mark the neighbor route valid");
    ok &= Check(routing.m_routeTable[ackNeighbor].expire == Simulator::Now() + routing.m_routeLifetime,
                "RREP_ACK should refresh the neighbor route lifetime");

    AquaSimAddress discoveryDestination(8);
    routing.m_activeDiscoveries.insert(discoveryDestination);
    routing.m_rreqAttempts[discoveryDestination] = 2;
    routing.m_rreqHopLimits[discoveryDestination] = 5;
    AquaSimUWAodvRouting::RouteEntry discoveryRoute;
    discoveryRoute.state = AquaSimUWAodvRouting::ROUTE_IN_SEARCH;
    routing.m_routeTable[discoveryDestination] = discoveryRoute;
    ok &= Check(routing.IsRouteDiscoveryActive(discoveryDestination),
                "explicit discovery state should be active");
    routing.ClearRouteDiscovery(discoveryDestination);
    ok &= Check(!routing.IsRouteDiscoveryActive(discoveryDestination),
                "clearing discovery should remove active state");
    ok &= Check(routing.m_rreqAttempts.find(discoveryDestination) == routing.m_rreqAttempts.end(),
                "clearing discovery should remove retry attempts");
    ok &= Check(routing.m_rreqHopLimits.find(discoveryDestination) == routing.m_rreqHopLimits.end(),
                "clearing discovery should remove expanding-ring hop limit");
    ok &= Check(routing.m_routeTable[discoveryDestination].state ==
                  AquaSimUWAodvRouting::ROUTE_INVALID,
                "clearing discovery should move IN_SEARCH route state back to INVALID");
    routing.RouteRequestTimeout(discoveryDestination, 2);
    ok &= Check(routing.m_rreqAttempts.find(discoveryDestination) == routing.m_rreqAttempts.end(),
                "stale discovery timeout should not recreate retry state");

    AquaSimAddress stateDestination(11);
    routing.UpdateRoute(stateDestination,
                        AquaSimAddress(2),
                        1,
                        25,
                        true,
                        Seconds(30));
    ok &= Check(routing.m_routeTable[stateDestination].state ==
                  AquaSimUWAodvRouting::ROUTE_VALID,
                "UpdateRoute should install a VALID route state");
    ok &= Check(routing.IsRouteUsable(routing.m_routeTable[stateDestination]),
                "fresh VALID route state should be usable");

    AquaSimAddress purgeDestination(12);
    AquaSimUWAodvRouting::RouteEntry purgeRoute;
    purgeRoute.nextHop = AquaSimAddress(2);
    purgeRoute.hopCount = 1;
    purgeRoute.destSeqNo = 30;
    purgeRoute.validSeqNo = true;
    purgeRoute.state = AquaSimUWAodvRouting::ROUTE_VALID;
    purgeRoute.valid = true;
    purgeRoute.expire = Simulator::Now() - Seconds(1);
    routing.m_routeTable[purgeDestination] = purgeRoute;
    routing.PurgeRouteTable();
    ok &= Check(routing.m_routeTable.find(purgeDestination) != routing.m_routeTable.end(),
                "expired VALID route should be retained as INVALID until DeletePeriod");
    ok &= Check(routing.m_routeTable[purgeDestination].state ==
                  AquaSimUWAodvRouting::ROUTE_INVALID,
                "expired VALID route should become INVALID during purge");
    ok &= Check(routing.m_routeTable[purgeDestination].expire ==
                  Simulator::Now() + routing.m_deletePeriod,
                "expired VALID route should receive DeletePeriod lifetime");

    routing.m_routeTable[purgeDestination].expire = Simulator::Now() - Seconds(1);
    routing.PurgeRouteTable();
    ok &= Check(routing.m_routeTable.find(purgeDestination) == routing.m_routeTable.end(),
                "expired INVALID route should be removed by purge");

    AquaSimAddress searchDestination(13);
    AquaSimUWAodvRouting::RouteEntry searchRoute;
    searchRoute.state = AquaSimUWAodvRouting::ROUTE_IN_SEARCH;
    searchRoute.expire = Simulator::Now() - Seconds(1);
    routing.m_routeTable[searchDestination] = searchRoute;
    routing.PurgeRouteTable();
    ok &= Check(routing.m_routeTable.find(searchDestination) != routing.m_routeTable.end() &&
                  routing.m_routeTable[searchDestination].state ==
                    AquaSimUWAodvRouting::ROUTE_INVALID,
                "expired IN_SEARCH route should be retained as INVALID until DeletePeriod");

    AquaSimAddress queueDestination(14);
    routing.m_maxQueueLen = 2;
    routing.m_maxQueueTime = Seconds(1);
    ok &= Check(routing.EnqueuePacket(queueDestination, Create<Packet>(10)),
                "first pending packet should enqueue");
    ok &= Check(routing.EnqueuePacket(queueDestination, Create<Packet>(10)),
                "second pending packet should enqueue");
    ok &= Check(routing.EnqueuePacket(queueDestination, Create<Packet>(10)),
                "full pending queue should drop oldest packet and enqueue the new one");
    ok &= Check(routing.m_pendingQueue[queueDestination].size() == 2,
                "pending queue should remain bounded by MaxQueueLen");
    ok &= Check(routing.m_queueDrops.Get() == 1,
                "full pending queue should count one queue drop");

    Simulator::Schedule(Seconds(1.2),
                        [&routing, &ok, queueDestination] {
                          routing.PurgeExpiredQueuedPackets(queueDestination);
                          ok &= Check(routing.m_pendingQueue.find(queueDestination) ==
                                        routing.m_pendingQueue.end(),
                                      "pending queue purge should remove expired packets");
                          ok &= Check(routing.m_noRouteDrops.Get() == 2,
                                      "expired pending packets should count as no-route drops");
                        });

    Simulator::Schedule(Seconds(1.5),
                        [&routing, &ok] {
                          routing.m_helloInterval = Seconds(10);
                          routing.m_lastBroadcastTime = Simulator::Now() - Seconds(1);
                          ok &= Check(routing.GetHelloDelay() == Seconds(9),
                                      "HELLO should be deferred until one HelloInterval after recent broadcast");
                          routing.m_lastBroadcastTime = Simulator::Now() - Seconds(10);
                          ok &= Check(routing.GetHelloDelay() == Seconds(0),
                                      "HELLO should not be deferred after a full HelloInterval has elapsed");
                        });

    AquaSimAddress expiringDestination(10);
    AquaSimUWAodvRouting::RouteEntry expiringRoute;
    expiringRoute.nextHop = AquaSimAddress(2);
    expiringRoute.hopCount = 1;
    expiringRoute.destSeqNo = 20;
    expiringRoute.validSeqNo = true;
    expiringRoute.state = AquaSimUWAodvRouting::ROUTE_VALID;
    expiringRoute.valid = true;
    expiringRoute.expire = Simulator::Now() + Seconds(1);
    routing.m_routeTable[expiringDestination] = expiringRoute;
    ok &= Check(routing.HasValidRoute(expiringDestination),
                "freshly installed short-lived route should be valid");
    Simulator::Schedule(Seconds(2),
                        [&routing, &ok, expiringDestination] {
                          ok &= Check(!routing.HasValidRoute(expiringDestination),
                                      "route lookup should reject expired active route");
                        });
    Simulator::Run();
    Simulator::Destroy();

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
