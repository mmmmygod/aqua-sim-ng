/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef AQUA_SIM_ROUTING_UW_AODV_H
#define AQUA_SIM_ROUTING_UW_AODV_H

#include "aqua-sim-address.h"
#include "aqua-sim-header.h"
#include "aqua-sim-routing.h"

#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/random-variable-stream.h"
#include "ns3/traced-value.h"

#include <cstdint>
#include <deque>
#include <map>
#include <set>
#include <utility>

namespace ns3 {

class AquaSimUWAodvRouteFreshnessTest;
class AquaSimUWAodvRerrTest;

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
    UWAODV_RERR = 4
  };

  enum Flags
  {
    FLAG_NONE = 0,
    FLAG_UNKNOWN_DEST_SEQ = 1 << 0
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

  struct RouteEntry
  {
    AquaSimAddress nextHop;
    uint16_t hopCount;
    uint32_t destSeqNo;
    bool validSeqNo;
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

  typedef std::pair<uint16_t, uint32_t> RequestKey;

  AquaSimAddress GetLocalAddress() const;
  bool HasValidRoute(AquaSimAddress destination) const;
  bool LookupRoute(AquaSimAddress destination, RouteEntry& entry) const;
  bool LookupAnyRoute(AquaSimAddress destination, RouteEntry& entry) const;
  bool IsSeqNoNewer(uint32_t incomingSeqNo, uint32_t currentSeqNo) const;
  bool ShouldUpdateRoute(const RouteEntry& current,
                         uint32_t incomingSeqNo,
                         bool incomingValidSeqNo,
                         uint16_t incomingHopCount) const;
  bool RouteIsFreshEnough(const RouteEntry& route, const AquaSimUWAodvHeader& rreq) const;
  void UpdateRoute(AquaSimAddress destination,
                   AquaSimAddress nextHop,
                   uint16_t hopCount,
                   uint32_t destSeqNo,
                   bool validSeqNo,
                   Time lifetime);

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

  bool RouteOutput(Ptr<Packet> packet, const Address& dest);
  void PrepareDataPacket(Ptr<Packet> packet, AquaSimHeader& ash, AquaSimAddress destination);
  bool ForwardDataPacket(Ptr<Packet> packet, AquaSimAddress destination);
  bool BroadcastDataPacket(Ptr<Packet> packet);
  bool DeliverDataPacket(Ptr<Packet> packet, AquaSimHeader ash, AquaSimUWAodvHeader aodv);

  bool EnqueuePacket(AquaSimAddress destination, Ptr<Packet> packet);
  void SendQueuedPackets(AquaSimAddress destination);
  void DropQueuedPackets(AquaSimAddress destination);

  void StartRouteDiscovery(AquaSimAddress destination);
  void SendRreq(AquaSimAddress destination);
  void SendRrep(AquaSimAddress origin, AquaSimAddress destination);
  void SendRrep(AquaSimAddress origin, AquaSimAddress destination, AquaSimAddress nextHop);
  void SendRerr(AquaSimAddress unreachableDestination,
                uint32_t unreachableDestSeqNo,
                const std::set<AquaSimAddress>& precursors);
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
  void RouteRequestTimeout(AquaSimAddress destination, uint32_t attempt);
  uint16_t GetRreqHopLimit(AquaSimAddress destination, uint32_t attempt);
  Time GetRreqTimeout(uint16_t hopLimit) const;
  Time GetRreqJitter() const;
  void AddPrecursor(AquaSimAddress destination, AquaSimAddress precursor);
  std::set<AquaSimAddress> InvalidateRoute(AquaSimAddress destination,
                                           uint32_t destSeqNo,
                                           bool validSeqNo);
  bool ShouldAcceptRerr(AquaSimAddress destination, uint32_t destSeqNo) const;

  void AddAodvTag(Ptr<Packet> packet);
  bool IsAodvPacket(Ptr<Packet> packet) const;
  bool IsPacketForMe(AquaSimHeader ash) const;
  bool IsBroadcastDestination(AquaSimAddress destination) const;

  std::map<AquaSimAddress, RouteEntry> m_routeTable;
  std::map<AquaSimAddress, std::deque<Ptr<Packet>>> m_pendingQueue;
  std::map<AquaSimAddress, uint32_t> m_rreqAttempts;
  std::map<AquaSimAddress, uint16_t> m_rreqHopLimits;
  std::map<RequestKey, RreqCollection> m_rreqCollections;
  std::set<AquaSimAddress> m_activeDiscoveries;
  std::set<RequestKey> m_seenRreqs;

  uint32_t m_nextRreqId;
  uint32_t m_sequenceNumber;
  uint32_t m_maxQueueLen;
  uint32_t m_maxRreqAttempts;
  uint16_t m_maxHopCount;
  uint16_t m_ttlStart;
  uint16_t m_ttlIncrement;
  uint16_t m_ttlThreshold;
  uint16_t m_netDiameter;
  bool m_enableRreqCollection;
  Time m_rreqTimeout;
  Time m_nodeTraversalTime;
  uint16_t m_timeoutBuffer;
  Time m_rreqJitter;
  Time m_rrepWaitTime;
  Time m_routeLifetime;
  Ptr<UniformRandomVariable> m_uniformRandomVariable;

  TracedValue<uint32_t> m_rreqTx;
  TracedValue<uint32_t> m_rreqRx;
  TracedValue<uint32_t> m_rrepTx;
  TracedValue<uint32_t> m_rrepRx;
  TracedValue<uint32_t> m_rerrTx;
  TracedValue<uint32_t> m_rerrRx;
  TracedValue<uint32_t> m_queuedPackets;
  TracedValue<uint32_t> m_queueDrops;
  TracedValue<uint32_t> m_forwardedData;
  TracedValue<uint32_t> m_deliveredData;
  TracedValue<uint32_t> m_noRouteDrops;
};

} // namespace ns3

#endif /* AQUA_SIM_ROUTING_UW_AODV_H */
