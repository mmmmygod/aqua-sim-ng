/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef AQUA_SIM_ROUTING_UW_AODV_H
#define AQUA_SIM_ROUTING_UW_AODV_H

#include "aqua-sim-address.h"
#include "aqua-sim-header.h"
#include "aqua-sim-routing.h"

#include "ns3/header.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include "ns3/random-variable-stream.h"
#include "ns3/traced-value.h"

#include <cstdint>
#include <deque>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace ns3 {

class AquaSimUWAodvRouteFreshnessTest;
class AquaSimUWAodvRerrTest;
class AquaSimUWAodvControlFlagsTest;

/**
 * \ingroup aqua-sim-ng
 *
 * \brief Compact control/data shim for underwater AODV.
 */
class AquaSimUWAodvHeader : public Header
{
public:
  enum PacketType
  {
    UWAODV_DATA = 1,
    UWAODV_RREQ = 2,
    UWAODV_RREP = 3,
    UWAODV_RERR = 4,
    UWAODV_HELLO = 5,
    UWAODV_RREP_ACK = 6
  };

  enum Flags
  {
    FLAG_NONE = 0,
    FLAG_UNKNOWN_DEST_SEQ = 1 << 0,
    FLAG_ACK_REQUIRED = 1 << 1,
    FLAG_GRATUITOUS_RREP = 1 << 2,
    FLAG_DESTINATION_ONLY = 1 << 3
  };

  AquaSimUWAodvHeader();
  ~AquaSimUWAodvHeader() override;

  static TypeId GetTypeId();
  TypeId GetInstanceTypeId() const override;

  void SetType(uint8_t type);
  void SetHopCount(uint16_t hopCount);
  void SetRequestId(uint32_t requestId);
  void SetOrigin(AquaSimAddress origin);
  void SetDestination(AquaSimAddress destination);
  void SetOriginSeqNo(uint32_t seqNo);
  void SetDestSeqNo(uint32_t seqNo);
  void SetLifetime(uint32_t lifetimeMs);
  void SetHopLimit(uint16_t hopLimit);
  void SetFlags(uint8_t flags);
  void SetUnknownDestSeqNo(bool unknown);
  void SetAckRequired(bool required);
  void SetGratuitousRrep(bool gratuitous);
  void SetDestinationOnly(bool destinationOnly);

  uint8_t GetType() const;
  uint16_t GetHopCount() const;
  uint32_t GetRequestId() const;
  AquaSimAddress GetOrigin() const;
  AquaSimAddress GetDestination() const;
  uint32_t GetOriginSeqNo() const;
  uint32_t GetDestSeqNo() const;
  uint32_t GetLifetime() const;
  uint16_t GetHopLimit() const;
  uint8_t GetFlags() const;
  bool IsUnknownDestSeqNo() const;
  bool IsAckRequired() const;
  bool IsGratuitousRrep() const;
  bool IsDestinationOnly() const;

  uint32_t GetSerializedSize() const override;
  void Serialize(Buffer::Iterator start) const override;
  uint32_t Deserialize(Buffer::Iterator start) override;
  void Print(std::ostream& os) const override;

private:
  uint8_t m_type;
  uint16_t m_hopCount;
  uint32_t m_requestId;
  AquaSimAddress m_origin;
  AquaSimAddress m_destination;
  uint32_t m_originSeqNo;
  uint32_t m_destSeqNo;
  uint32_t m_lifetimeMs;
  uint16_t m_hopLimit;
  uint8_t m_flags;
};

/**
 * \ingroup aqua-sim-ng
 *
 * \brief Variable-length unreachable destination list used by UW-AODV RERR.
 */
class AquaSimUWAodvRerrListHeader : public Header
{
public:
  typedef std::pair<AquaSimAddress, uint32_t> UnreachableDestination;

  AquaSimUWAodvRerrListHeader();
  ~AquaSimUWAodvRerrListHeader() override;

  static TypeId GetTypeId();
  TypeId GetInstanceTypeId() const override;

  void AddUnreachableDestination(AquaSimAddress destination, uint32_t destSeqNo);
  const std::vector<UnreachableDestination>& GetUnreachableDestinations() const;

  uint32_t GetSerializedSize() const override;
  void Serialize(Buffer::Iterator start) const override;
  uint32_t Deserialize(Buffer::Iterator start) override;
  void Print(std::ostream& os) const override;

private:
  std::vector<UnreachableDestination> m_unreachable;
};

/**
 * \ingroup aqua-sim-ng
 *
 * \brief Minimal underwater AODV routing protocol for Aqua-Sim NG.
 */
class AquaSimUWAodvRouting : public AquaSimRouting
{
public:
  AquaSimUWAodvRouting();
  ~AquaSimUWAodvRouting() override;

  static TypeId GetTypeId();

  bool Recv(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;
  int64_t AssignStreams(int64_t stream) override;

protected:
  void DoDispose() override;

private:
  friend class AquaSimUWAodvRouteFreshnessTest;
  friend class AquaSimUWAodvRerrTest;
  friend class AquaSimUWAodvControlFlagsTest;

  enum RouteState
  {
    ROUTE_VALID,
    ROUTE_INVALID,
    ROUTE_IN_SEARCH
  };

  struct RouteEntry
  {
    RouteEntry();

    AquaSimAddress nextHop;
    uint16_t hopCount;
    uint32_t destSeqNo;
    bool validSeqNo;
    RouteState state;
    bool valid;
    Time expire;
    std::set<AquaSimAddress> precursors;
  };

  struct RreqCandidate
  {
    AquaSimAddress previousHop;
    uint16_t hopCount;
    uint32_t originSeqNo;
    Time receiveTime;
  };

  struct RreqCollection
  {
    RreqCandidate best;
    bool hasCandidate;
  };

  struct PendingPacket
  {
    Ptr<Packet> packet;
    Time expire;
  };

  typedef std::pair<uint16_t, uint32_t> RequestKey;
  typedef std::pair<uint16_t, uint64_t> DataKey;

  AquaSimAddress GetLocalAddress() const;
  bool HasValidRoute(AquaSimAddress destination) const;
  bool LookupRoute(AquaSimAddress destination, RouteEntry& entry) const;
  bool LookupAnyRoute(AquaSimAddress destination, RouteEntry& entry) const;
  bool IsRouteUsable(const RouteEntry& route) const;
  bool IsRouteActiveState(const RouteEntry& route) const;
  void SetRouteState(RouteEntry& route, RouteState state) const;
  bool IsSeqNoNewer(uint32_t incomingSeqNo, uint32_t currentSeqNo) const;
  bool ShouldUpdateRoute(const RouteEntry& current,
                         uint32_t incomingSeqNo,
                         bool incomingValidSeqNo,
                         uint16_t incomingHopCount) const;
  bool RouteIsFreshEnough(const RouteEntry& route, const AquaSimUWAodvHeader& rreq) const;
  bool ShouldDropRreqToAvoidLoop(const RouteEntry& route, AquaSimAddress previousHop) const;
  bool CanSendIntermediateRrep(const RouteEntry& route,
                               const AquaSimUWAodvHeader& rreq,
                               AquaSimAddress previousHop) const;
  bool ShouldRefreshForwardedRreqDestSeqNo(const RouteEntry& route,
                                           const AquaSimUWAodvHeader& rreq,
                                           AquaSimAddress previousHop) const;
  bool ShouldSendGratuitousRrep(const RouteEntry& route,
                                const AquaSimUWAodvHeader& rreq,
                                AquaSimAddress previousHop) const;
  void UpdateOwnSeqNoBeforeDestinationRrep(const AquaSimUWAodvHeader& rreq);
  Time GetNetTraversalTime() const;
  Time GetPathDiscoveryTime() const;
  Time GetMyRouteTimeout() const;
  Time GetNextHopWait() const;
  Time GetBlacklistTimeout() const;
  Time GetReverseRouteLifetime(uint16_t reverseHopCount) const;
  void PurgeRouteTable();
  void UpdateRoute(AquaSimAddress destination,
                   AquaSimAddress nextHop,
                   uint16_t hopCount,
                   uint32_t destSeqNo,
                   bool validSeqNo,
                   Time lifetime);
  void RefreshRouteLifetime(AquaSimAddress destination, Time lifetime);
  void RefreshActiveRouteUse(AquaSimAddress destination, AquaSimAddress nextHop);

  bool RecvAodvPacket(Ptr<Packet> packet);
  bool RecvData(Ptr<Packet> packet, AquaSimHeader ash, AquaSimUWAodvHeader aodv);
  bool RecvRreq(Ptr<Packet> packet,
                AquaSimHeader ash,
                AquaSimUWAodvHeader aodv,
                AquaSimAddress previousHop);
  bool RecvRrep(Ptr<Packet> packet,
                AquaSimHeader ash,
                AquaSimUWAodvHeader aodv,
                AquaSimAddress previousHop);
  bool RecvRerr(Ptr<Packet> packet,
                AquaSimHeader ash,
                AquaSimUWAodvHeader aodv,
                AquaSimAddress previousHop);
  bool RecvHello(Ptr<Packet> packet,
                 AquaSimHeader ash,
                 AquaSimUWAodvHeader aodv,
                 AquaSimAddress previousHop);
  bool RecvRrepAck(Ptr<Packet> packet,
                   AquaSimHeader ash,
                   AquaSimUWAodvHeader aodv,
                   AquaSimAddress previousHop);

  bool RouteOutput(Ptr<Packet> packet, const Address& dest);
  void PrepareDataPacket(Ptr<Packet> packet, AquaSimHeader& ash, AquaSimAddress destination);
  bool ForwardDataPacket(Ptr<Packet> packet, AquaSimAddress destination);
  bool BroadcastDataPacket(Ptr<Packet> packet);
  bool DeliverDataPacket(Ptr<Packet> packet, AquaSimHeader ash, AquaSimUWAodvHeader aodv);

  bool EnqueuePacket(AquaSimAddress destination, Ptr<Packet> packet);
  void PurgeExpiredQueuedPackets(AquaSimAddress destination);
  void PurgeExpiredQueuedPackets();
  void SendQueuedPackets(AquaSimAddress destination);
  void DropQueuedPackets(AquaSimAddress destination);

  bool IsRouteDiscoveryActive(AquaSimAddress destination) const;
  void ClearRouteDiscovery(AquaSimAddress destination);
  void StartRouteDiscovery(AquaSimAddress destination);
  void SendRreq(AquaSimAddress destination);
  void SendRrep(AquaSimAddress origin, AquaSimAddress destination);
  void SendRrep(AquaSimAddress origin, AquaSimAddress destination, AquaSimAddress nextHop);
  void SendGratuitousRrep(AquaSimAddress origin, AquaSimAddress destination);
  void SendRerr(AquaSimAddress unreachableDestination,
                uint32_t unreachableDestSeqNo,
                const std::set<AquaSimAddress>& precursors);
  void SendRerr(const std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination>& unreachable,
                const std::set<AquaSimAddress>& precursors);
  void SendRerrWhenNoRouteToForward(AquaSimAddress unreachableDestination,
                                    uint32_t unreachableDestSeqNo,
                                    AquaSimAddress origin);
  void BroadcastRerr(AquaSimAddress unreachableDestination, uint32_t unreachableDestSeqNo);
  void SendRrepAck(AquaSimAddress nextHop);
  void RequireRrepAck(AquaSimAddress nextHop);
  void RrepAckTimeout(AquaSimAddress nextHop);
  void BlacklistNeighbor(AquaSimAddress neighbor);
  bool IsBlacklisted(AquaSimAddress neighbor) const;
  void PurgeBlacklist();
  void RememberRreq(const RequestKey& key);
  bool HasSeenRreq(const RequestKey& key) const;
  void PurgeSeenRreqs();
  void RememberDataPacket(const DataKey& key);
  bool HasSeenDataPacket(const DataKey& key) const;
  void PurgeSeenDataPackets();
  void QueueRrepCandidate(const RequestKey& key,
                          const AquaSimUWAodvHeader& aodv,
                          AquaSimAddress previousHop);
  void SendCollectedRrep(RequestKey key);
  bool IsBetterRreqCandidate(const RreqCandidate& candidate,
                             const RreqCandidate& current) const;
  void ForwardRreq(Ptr<Packet> packet, AquaSimHeader ash, AquaSimUWAodvHeader aodv);
  void ForwardRrep(Ptr<Packet> packet, AquaSimHeader ash, AquaSimUWAodvHeader aodv, AquaSimAddress nextHop);
  void ForwardRerr(AquaSimAddress unreachableDestination,
                   uint32_t unreachableDestSeqNo,
                   const std::set<AquaSimAddress>& precursors);
  void ForwardRerr(const std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination>& unreachable,
                   const std::set<AquaSimAddress>& precursors);
  void RouteRequestTimeout(AquaSimAddress destination, uint32_t attempt);
  uint16_t GetRreqHopLimit(AquaSimAddress destination, uint32_t attempt);
  Time GetRreqTimeout(uint16_t hopLimit) const;
  Time GetRreqJitter() const;
  bool CanSendRreqNow() const;
  bool CanSendRerrNow() const;
  void NoteRreqSent();
  void NoteRerrSent();
  Time GetRreqRateLimitDelay() const;
  Time GetRerrRateLimitDelay() const;
  void AddPrecursor(AquaSimAddress destination, AquaSimAddress precursor);
  std::set<AquaSimAddress> InvalidateRoute(AquaSimAddress destination,
                                           uint32_t destSeqNo,
                                           bool validSeqNo);
  bool ShouldAcceptRerr(AquaSimAddress destination,
                        uint32_t destSeqNo,
                        AquaSimAddress previousHop) const;
  void MaybeStartHello();
  void SendHello();
  void NoteBroadcastSent();
  Time GetHelloDelay() const;
  void UpdateNeighbor(AquaSimAddress neighbor);
  void PurgeDeadNeighbors();
  void InvalidateRoutesViaNeighbor(AquaSimAddress neighbor);

  void AddAodvTag(Ptr<Packet> packet);
  bool IsAodvPacket(Ptr<Packet> packet) const;
  bool IsPacketForMe(AquaSimHeader ash) const;
  bool IsBroadcastDestination(AquaSimAddress destination) const;

  std::map<AquaSimAddress, RouteEntry> m_routeTable;
  std::map<AquaSimAddress, std::deque<PendingPacket>> m_pendingQueue;
  std::map<AquaSimAddress, uint32_t> m_rreqAttempts;
  std::map<AquaSimAddress, uint16_t> m_rreqHopLimits;
  std::map<RequestKey, RreqCollection> m_rreqCollections;
  std::map<AquaSimAddress, Time> m_neighbors;
  std::map<AquaSimAddress, Time> m_blacklist;
  std::map<AquaSimAddress, EventId> m_rrepAckTimers;
  std::set<AquaSimAddress> m_activeDiscoveries;
  std::map<RequestKey, Time> m_seenRreqs;
  std::map<DataKey, Time> m_seenDataPackets;

  uint32_t m_nextRreqId;
  uint32_t m_sequenceNumber;
  uint32_t m_maxQueueLen;
  Time m_maxQueueTime;
  uint32_t m_maxRreqAttempts;
  uint16_t m_maxHopCount;
  uint16_t m_ttlStart;
  uint16_t m_ttlIncrement;
  uint16_t m_ttlThreshold;
  uint16_t m_netDiameter;
  bool m_enableRreqCollection;
  bool m_gratuitousReply;
  bool m_destinationOnly;
  bool m_enableBroadcast;
  Time m_rreqTimeout;
  Time m_nodeTraversalTime;
  uint16_t m_timeoutBuffer;
  Time m_rreqJitter;
  Time m_rrepWaitTime;
  Time m_routeLifetime;
  Time m_myRouteTimeout;
  Time m_deletePeriod;
  uint32_t m_rreqRateLimit;
  uint32_t m_rerrRateLimit;
  uint32_t m_rreqCount;
  uint32_t m_rerrCount;
  Time m_rreqRateLimitWindowStart;
  Time m_rerrRateLimitWindowStart;
  bool m_enableRrepAck;
  Time m_nextHopWait;
  Time m_blacklistTimeout;
  bool m_enableHello;
  bool m_helloStarted;
  Time m_helloInterval;
  Time m_lastBroadcastTime;
  uint16_t m_allowedHelloLoss;
  Ptr<UniformRandomVariable> m_uniformRandomVariable;

  TracedValue<uint32_t> m_rreqTx;
  TracedValue<uint32_t> m_rreqRx;
  TracedValue<uint32_t> m_rrepTx;
  TracedValue<uint32_t> m_rrepRx;
  TracedValue<uint32_t> m_rerrTx;
  TracedValue<uint32_t> m_rerrRx;
  TracedValue<uint32_t> m_rrepAckTx;
  TracedValue<uint32_t> m_rrepAckRx;
  TracedValue<uint32_t> m_blacklistAdds;
  TracedValue<uint32_t> m_helloTx;
  TracedValue<uint32_t> m_helloRx;
  TracedValue<uint32_t> m_queuedPackets;
  TracedValue<uint32_t> m_queueDrops;
  TracedValue<uint32_t> m_forwardedData;
  TracedValue<uint32_t> m_deliveredData;
  TracedValue<uint32_t> m_noRouteDrops;
  TracedValue<uint32_t> m_duplicateDataDrops;
};

} // namespace ns3

#endif /* AQUA_SIM_ROUTING_UW_AODV_H */
