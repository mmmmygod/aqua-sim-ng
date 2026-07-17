/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "aqua-sim-routing-uw-aodv.h"

#include "aqua-sim-pt-tag.h"

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("AquaSimUWAodvRouting");
NS_OBJECT_ENSURE_REGISTERED(AquaSimUWAodvHeader);
NS_OBJECT_ENSURE_REGISTERED(AquaSimUWAodvRouting);

AquaSimUWAodvHeader::AquaSimUWAodvHeader()
  : m_type(UWAODV_DATA),
    m_hopCount(0),
    m_requestId(0),
    m_origin(AquaSimAddress()),
    m_destination(AquaSimAddress()),
    m_originSeqNo(0),
    m_destSeqNo(0),
    m_lifetimeMs(0),
    m_hopLimit(0),
    m_flags(FLAG_NONE)
{
}

AquaSimUWAodvHeader::~AquaSimUWAodvHeader()
{
}

TypeId
AquaSimUWAodvHeader::GetTypeId()
{
  static TypeId tid = TypeId("ns3::AquaSimUWAodvHeader")
    .SetParent<Header>()
    .AddConstructor<AquaSimUWAodvHeader>();
  return tid;
}

TypeId
AquaSimUWAodvHeader::GetInstanceTypeId() const
{
  return GetTypeId();
}

void
AquaSimUWAodvHeader::SetType(uint8_t type)
{
  m_type = type;
}

void
AquaSimUWAodvHeader::SetHopCount(uint16_t hopCount)
{
  m_hopCount = hopCount;
}

void
AquaSimUWAodvHeader::SetRequestId(uint32_t requestId)
{
  m_requestId = requestId;
}

void
AquaSimUWAodvHeader::SetOrigin(AquaSimAddress origin)
{
  m_origin = origin;
}

void
AquaSimUWAodvHeader::SetDestination(AquaSimAddress destination)
{
  m_destination = destination;
}

void
AquaSimUWAodvHeader::SetOriginSeqNo(uint32_t seqNo)
{
  m_originSeqNo = seqNo;
}

void
AquaSimUWAodvHeader::SetDestSeqNo(uint32_t seqNo)
{
  m_destSeqNo = seqNo;
}

void
AquaSimUWAodvHeader::SetLifetime(uint32_t lifetimeMs)
{
  m_lifetimeMs = lifetimeMs;
}

void
AquaSimUWAodvHeader::SetHopLimit(uint16_t hopLimit)
{
  m_hopLimit = hopLimit;
}

void
AquaSimUWAodvHeader::SetFlags(uint8_t flags)
{
  m_flags = flags;
}

void
AquaSimUWAodvHeader::SetUnknownDestSeqNo(bool unknown)
{
  if (unknown)
    {
      m_flags |= FLAG_UNKNOWN_DEST_SEQ;
    }
  else
    {
      m_flags = static_cast<uint8_t>(m_flags & ~FLAG_UNKNOWN_DEST_SEQ);
    }
}

uint8_t
AquaSimUWAodvHeader::GetType() const
{
  return m_type;
}

uint16_t
AquaSimUWAodvHeader::GetHopCount() const
{
  return m_hopCount;
}

uint32_t
AquaSimUWAodvHeader::GetRequestId() const
{
  return m_requestId;
}

AquaSimAddress
AquaSimUWAodvHeader::GetOrigin() const
{
  return m_origin;
}

AquaSimAddress
AquaSimUWAodvHeader::GetDestination() const
{
  return m_destination;
}

uint32_t
AquaSimUWAodvHeader::GetOriginSeqNo() const
{
  return m_originSeqNo;
}

uint32_t
AquaSimUWAodvHeader::GetDestSeqNo() const
{
  return m_destSeqNo;
}

uint32_t
AquaSimUWAodvHeader::GetLifetime() const
{
  return m_lifetimeMs;
}

uint16_t
AquaSimUWAodvHeader::GetHopLimit() const
{
  return m_hopLimit;
}

uint8_t
AquaSimUWAodvHeader::GetFlags() const
{
  return m_flags;
}

bool
AquaSimUWAodvHeader::IsUnknownDestSeqNo() const
{
  return (m_flags & FLAG_UNKNOWN_DEST_SEQ) != 0;
}

uint32_t
AquaSimUWAodvHeader::GetSerializedSize() const
{
  return 26;
}

void
AquaSimUWAodvHeader::Serialize(Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8(m_type);
  i.WriteU8(m_flags);
  i.WriteU16(m_hopCount);
  i.WriteU32(m_requestId);
  i.WriteU16(m_origin.GetAsInt());
  i.WriteU16(m_destination.GetAsInt());
  i.WriteU32(m_originSeqNo);
  i.WriteU32(m_destSeqNo);
  i.WriteU32(m_lifetimeMs);
  i.WriteU16(m_hopLimit);
}

uint32_t
AquaSimUWAodvHeader::Deserialize(Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_type = i.ReadU8();
  m_flags = i.ReadU8();
  m_hopCount = i.ReadU16();
  m_requestId = i.ReadU32();
  m_origin = AquaSimAddress(i.ReadU16());
  m_destination = AquaSimAddress(i.ReadU16());
  m_originSeqNo = i.ReadU32();
  m_destSeqNo = i.ReadU32();
  m_lifetimeMs = i.ReadU32();
  m_hopLimit = i.ReadU16();
  return GetSerializedSize();
}

void
AquaSimUWAodvHeader::Print(std::ostream& os) const
{
  os << "UWAODV type=" << static_cast<uint32_t>(m_type)
     << " hops=" << m_hopCount
     << " rreqId=" << m_requestId
     << " origin=" << m_origin
     << " destination=" << m_destination
     << " originSeq=" << m_originSeqNo
     << " destSeq=" << m_destSeqNo
     << " lifetimeMs=" << m_lifetimeMs
     << " hopLimit=" << m_hopLimit
     << " flags=" << static_cast<uint32_t>(m_flags);
}

AquaSimUWAodvRouting::AquaSimUWAodvRouting()
  : m_nextRreqId(0),
    m_sequenceNumber(0),
    m_maxQueueLen(64),
    m_maxRreqAttempts(3),
    m_maxHopCount(32),
    m_ttlStart(1),
    m_ttlIncrement(2),
    m_ttlThreshold(7),
    m_netDiameter(32),
    m_enableRreqCollection(false),
    m_rreqTimeout(Seconds(3.0)),
    m_rrepWaitTime(Seconds(0.5)),
    m_routeLifetime(Seconds(120.0)),
    m_rreqTx(0),
    m_rreqRx(0),
    m_rrepTx(0),
    m_rrepRx(0),
    m_rerrTx(0),
    m_rerrRx(0),
    m_queuedPackets(0),
    m_queueDrops(0),
    m_forwardedData(0),
    m_deliveredData(0),
    m_noRouteDrops(0)
{
}

AquaSimUWAodvRouting::~AquaSimUWAodvRouting()
{
}

TypeId
AquaSimUWAodvRouting::GetTypeId()
{
  static TypeId tid = TypeId("ns3::AquaSimUWAodvRouting")
    .SetParent<AquaSimRouting>()
    .AddConstructor<AquaSimUWAodvRouting>()
    .AddAttribute("RreqTimeout",
                  "Bounded wait before retrying or failing route discovery.",
                  TimeValue(Seconds(3.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_rreqTimeout),
                  MakeTimeChecker())
    .AddAttribute("RouteLifetime",
                  "Time a learned route remains valid.",
                  TimeValue(Seconds(120.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_routeLifetime),
                  MakeTimeChecker())
    .AddAttribute("MaxQueueLen",
                  "Maximum pending data packets per destination.",
                  UintegerValue(64),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_maxQueueLen),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("MaxRreqAttempts",
                  "Maximum total RREQ attempts per pending destination.",
                  UintegerValue(3),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_maxRreqAttempts),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("MaxHopCount",
                  "Maximum hop count for RREQ forwarding.",
                  UintegerValue(32),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_maxHopCount),
                  MakeUintegerChecker<uint16_t>())
    .AddAttribute("TtlStart",
                  "Initial hop limit for expanding-ring RREQ discovery.",
                  UintegerValue(1),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_ttlStart),
                  MakeUintegerChecker<uint16_t>())
    .AddAttribute("TtlIncrement",
                  "Hop-limit increment for expanding-ring RREQ discovery.",
                  UintegerValue(2),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_ttlIncrement),
                  MakeUintegerChecker<uint16_t>())
    .AddAttribute("TtlThreshold",
                  "Hop-limit threshold before using NetDiameter for RREQ discovery.",
                  UintegerValue(7),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_ttlThreshold),
                  MakeUintegerChecker<uint16_t>())
    .AddAttribute("NetDiameter",
                  "Network-wide hop limit used after expanding-ring RREQ attempts exceed TtlThreshold.",
                  UintegerValue(32),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_netDiameter),
                  MakeUintegerChecker<uint16_t>())
    .AddAttribute("EnableRreqCollection",
                  "If true, a destination waits briefly to collect duplicate RREQ candidates before replying.",
                  BooleanValue(false),
                  MakeBooleanAccessor(&AquaSimUWAodvRouting::m_enableRreqCollection),
                  MakeBooleanChecker())
    .AddAttribute("RrepWaitTime",
                  "Destination-side wait window for collecting RREQ candidates before sending RREP.",
                  TimeValue(Seconds(0.5)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_rrepWaitTime),
                  MakeTimeChecker())
    .AddTraceSource("RreqTx",
                    "Number of RREQ packets transmitted.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_rreqTx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("RreqRx",
                    "Number of RREQ packets received.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_rreqRx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("RrepTx",
                    "Number of RREP packets transmitted.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_rrepTx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("RrepRx",
                    "Number of RREP packets received.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_rrepRx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("RerrTx",
                    "Number of RERR packets transmitted.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_rerrTx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("RerrRx",
                    "Number of RERR packets received.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_rerrRx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("QueuedPackets",
                    "Number of data packets queued during route discovery.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_queuedPackets),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("QueueDrops",
                    "Number of data packets dropped because a pending queue was full.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_queueDrops),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("ForwardedData",
                    "Number of data packets sent to the lower layer.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_forwardedData),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("DeliveredData",
                    "Number of data packets delivered to the upper layer.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_deliveredData),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("NoRouteDrops",
                    "Number of data packets dropped after route discovery failed.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_noRouteDrops),
                    "ns3::TracedValueCallback::Uint32");
  return tid;
}

int64_t
AquaSimUWAodvRouting::AssignStreams(int64_t stream)
{
  NS_LOG_FUNCTION(this << stream);
  return 0;
}

bool
AquaSimUWAodvRouting::Recv(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION(this << packet << dest << protocolNumber);
  if (!packet)
    {
      return false;
    }

  AquaSimHeader ash;
  packet->PeekHeader(ash);

  NS_LOG_DEBUG("local=" << GetLocalAddress()
              << " recv packet"
              << " daddr=" << ash.GetDAddr()
              << " saddr=" << ash.GetSAddr()
              << " nextHop=" << ash.GetNextHop()
              << " isAodv=" << IsAodvPacket(packet));

  if (IsAodvPacket(packet))
    {
      return RecvAodvPacket(packet);
    }

  return RouteOutput(packet, dest);
}

AquaSimAddress
AquaSimUWAodvRouting::GetLocalAddress() const
{
  return AquaSimAddress::ConvertFrom(m_device->GetAddress());
}

bool
AquaSimUWAodvRouting::HasValidRoute(AquaSimAddress destination) const
{
  RouteEntry entry;
  return LookupRoute(destination, entry);
}

bool
AquaSimUWAodvRouting::LookupRoute(AquaSimAddress destination, RouteEntry& entry) const
{
  std::map<AquaSimAddress, RouteEntry>::const_iterator it = m_routeTable.find(destination);
  if (it == m_routeTable.end() || !it->second.valid || it->second.expire < Simulator::Now())
    {
      return false;
    }

  entry = it->second;
  return true;
}

bool
AquaSimUWAodvRouting::LookupAnyRoute(AquaSimAddress destination, RouteEntry& entry) const
{
  std::map<AquaSimAddress, RouteEntry>::const_iterator it = m_routeTable.find(destination);
  if (it == m_routeTable.end())
    {
      return false;
    }

  entry = it->second;
  return true;
}

bool
AquaSimUWAodvRouting::IsSeqNoNewer(uint32_t incomingSeqNo, uint32_t currentSeqNo) const
{
  return static_cast<int32_t>(incomingSeqNo - currentSeqNo) > 0;
}

bool
AquaSimUWAodvRouting::ShouldUpdateRoute(const RouteEntry& current,
                                        uint32_t incomingSeqNo,
                                        bool incomingValidSeqNo,
                                        uint16_t incomingHopCount) const
{
  if (!current.valid || current.expire < Simulator::Now())
    {
      return true;
    }

  if (incomingValidSeqNo)
    {
      if (!current.validSeqNo)
        {
          return true;
        }

      if (IsSeqNoNewer(incomingSeqNo, current.destSeqNo))
        {
          return true;
        }

      if (incomingSeqNo == current.destSeqNo && incomingHopCount < current.hopCount)
        {
          return true;
        }

      return false;
    }

  return !current.validSeqNo && incomingHopCount < current.hopCount;
}

bool
AquaSimUWAodvRouting::RouteIsFreshEnough(const RouteEntry& route,
                                         const AquaSimUWAodvHeader& rreq) const
{
  if (!route.valid || route.expire < Simulator::Now())
    {
      return false;
    }

  if (rreq.IsUnknownDestSeqNo())
    {
      return true;
    }

  return route.validSeqNo &&
         (route.destSeqNo == rreq.GetDestSeqNo() ||
          IsSeqNoNewer(route.destSeqNo, rreq.GetDestSeqNo()));
}

void
AquaSimUWAodvRouting::UpdateRoute(AquaSimAddress destination,
                                  AquaSimAddress nextHop,
                                  uint16_t hopCount,
                                  uint32_t destSeqNo,
                                  bool validSeqNo,
                                  Time lifetime)
{
  if (destination == AquaSimAddress::GetBroadcast() || destination == GetLocalAddress())
    {
      return;
    }

  RouteEntry current;
  if (LookupAnyRoute(destination, current) &&
      !ShouldUpdateRoute(current, destSeqNo, validSeqNo, hopCount))
    {
      if (current.valid && current.hopCount == hopCount &&
          (!validSeqNo || !current.validSeqNo || current.destSeqNo == destSeqNo))
        {
          current.expire = Simulator::Now() + lifetime;
          m_routeTable[destination] = current;
        }
      return;
    }

  RouteEntry entry;
  entry.nextHop = nextHop;
  entry.hopCount = hopCount;
  entry.destSeqNo = destSeqNo;
  entry.validSeqNo = validSeqNo;
  entry.valid = true;
  entry.expire = Simulator::Now() + lifetime;
  if (LookupAnyRoute(destination, current))
    {
      entry.precursors = current.precursors;
    }
  m_routeTable[destination] = entry;
}

bool
AquaSimUWAodvRouting::RecvAodvPacket(Ptr<Packet> packet)
{
  AquaSimHeader ash;
  AquaSimUWAodvHeader aodv;
  packet->RemoveHeader(ash);
  packet->RemoveHeader(aodv);
  NS_LOG_DEBUG("local=" << GetLocalAddress()
            << " aodv type=" << static_cast<uint32_t>(aodv.GetType())
            << " origin=" << aodv.GetOrigin()
            << " dest=" << aodv.GetDestination()
            << " hop=" << aodv.GetHopCount()
            << " ash.saddr=" << ash.GetSAddr()
            << " ash.daddr=" << ash.GetDAddr()
            << " ash.nextHop=" << ash.GetNextHop());
  if (!IsPacketForMe(ash))
    {
      return false;
    }

  AquaSimAddress previousHop = ash.GetSAddr();
  switch (aodv.GetType())
    {
    case AquaSimUWAodvHeader::UWAODV_DATA:
      return RecvData(packet, ash, aodv);
    case AquaSimUWAodvHeader::UWAODV_RREQ:
      ++m_rreqRx;
      return RecvRreq(packet, ash, aodv, previousHop);
    case AquaSimUWAodvHeader::UWAODV_RREP:
      ++m_rrepRx;
      return RecvRrep(packet, ash, aodv, previousHop);
    case AquaSimUWAodvHeader::UWAODV_RERR:
      ++m_rerrRx;
      return RecvRerr(packet, ash, aodv, previousHop);
    default:
      NS_LOG_DEBUG("Dropping unknown UWAODV packet type "
                   << static_cast<uint32_t>(aodv.GetType()));
      return false;
    }
}

bool
AquaSimUWAodvRouting::RecvData(Ptr<Packet> packet,
                               AquaSimHeader ash,
                               AquaSimUWAodvHeader aodv)
{
  UpdateRoute(aodv.GetOrigin(),
              ash.GetSAddr(),
              aodv.GetHopCount() + 1,
              aodv.GetOriginSeqNo(),
              true,
              m_routeLifetime);

  if (aodv.GetDestination() == GetLocalAddress() ||
      IsBroadcastDestination(aodv.GetDestination()))
    {
      return DeliverDataPacket(packet, ash, aodv);
    }

  if (ash.GetNumForwards() >= m_maxHopCount)
    {
      ++m_noRouteDrops;
      return false;
    }

  AddPrecursor(aodv.GetDestination(), ash.GetSAddr());
  ash.SetNumForwards(ash.GetNumForwards() + 1);
  aodv.SetHopCount(aodv.GetHopCount() + 1);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);
  return ForwardDataPacket(packet, aodv.GetDestination());
}

bool
AquaSimUWAodvRouting::RecvRreq(Ptr<Packet> packet,
                               AquaSimHeader ash,
                               AquaSimUWAodvHeader aodv,
                               AquaSimAddress previousHop)
{
  RequestKey key(aodv.GetOrigin().GetAsInt(), aodv.GetRequestId());
  bool isDestination = aodv.GetDestination() == GetLocalAddress();
  bool collectAtDestination = isDestination && m_enableRreqCollection;

  if (collectAtDestination &&
      m_seenRreqs.find(key) != m_seenRreqs.end() &&
      m_rreqCollections.find(key) == m_rreqCollections.end())
    {
      return false;
    }

  if (!collectAtDestination && m_seenRreqs.find(key) != m_seenRreqs.end())
    {
      return false;
    }
  if (!collectAtDestination)
    {
      m_seenRreqs.insert(key);
    }

  if (aodv.GetOrigin() == GetLocalAddress())
    {
      return false;
    }

  UpdateRoute(aodv.GetOrigin(),
              previousHop,
              aodv.GetHopCount() + 1,
              aodv.GetOriginSeqNo(),
              true,
              m_routeLifetime);

  if (isDestination)
    {
      if (!aodv.IsUnknownDestSeqNo() &&
          (aodv.GetDestSeqNo() == m_sequenceNumber ||
           IsSeqNoNewer(aodv.GetDestSeqNo(), m_sequenceNumber)))
        {
          m_sequenceNumber = aodv.GetDestSeqNo() + 1;
        }

      if (m_enableRreqCollection)
        {
          QueueRrepCandidate(key, aodv, previousHop);
          return true;
        }

      m_seenRreqs.insert(key);
      SendRrep(aodv.GetOrigin(), GetLocalAddress(), previousHop);
      return true;
    }

  RouteEntry destinationRoute;
  if (LookupRoute(aodv.GetDestination(), destinationRoute) &&
      RouteIsFreshEnough(destinationRoute, aodv))
    {
      SendRrep(aodv.GetOrigin(), aodv.GetDestination());
      return true;
    }

  uint16_t effectiveHopLimit = aodv.GetHopLimit() == 0 ? m_maxHopCount : aodv.GetHopLimit();
  if (effectiveHopLimit > m_maxHopCount)
    {
      effectiveHopLimit = m_maxHopCount;
    }
  if (aodv.GetHopCount() >= effectiveHopLimit)
    {
      return false;
    }

  ForwardRreq(packet, ash, aodv);
  return true;
}

bool
AquaSimUWAodvRouting::RecvRrep(Ptr<Packet> packet,
                               AquaSimHeader ash,
                               AquaSimUWAodvHeader aodv,
                               AquaSimAddress previousHop)
{
  UpdateRoute(aodv.GetDestination(),
              previousHop,
              aodv.GetHopCount() + 1,
              aodv.GetDestSeqNo(),
              true,
              MilliSeconds(aodv.GetLifetime()));
  SendQueuedPackets(aodv.GetDestination());

  if (aodv.GetOrigin() == GetLocalAddress())
    {
      m_activeDiscoveries.erase(aodv.GetDestination());
      m_rreqAttempts.erase(aodv.GetDestination());
      m_rreqHopLimits.erase(aodv.GetDestination());
      return true;
    }

  RouteEntry route;
  if (!LookupRoute(aodv.GetOrigin(), route))
    {
      ++m_noRouteDrops;
      return false;
    }

  AddPrecursor(aodv.GetDestination(), route.nextHop);
  AddPrecursor(aodv.GetOrigin(), previousHop);
  ForwardRrep(packet, ash, aodv, route.nextHop);
  return true;
}

bool
AquaSimUWAodvRouting::RecvRerr(Ptr<Packet> packet,
                               AquaSimHeader ash,
                               AquaSimUWAodvHeader aodv,
                               AquaSimAddress previousHop)
{
  (void)packet;
  (void)ash;

  AquaSimAddress unreachableDestination = aodv.GetDestination();
  uint32_t unreachableDestSeqNo = aodv.GetDestSeqNo();

  if (!ShouldAcceptRerr(unreachableDestination, unreachableDestSeqNo))
    {
      return false;
    }

  std::set<AquaSimAddress> precursors =
    InvalidateRoute(unreachableDestination, unreachableDestSeqNo, true);
  precursors.erase(previousHop);
  ForwardRerr(unreachableDestination, unreachableDestSeqNo, precursors);
  return true;
}

bool
AquaSimUWAodvRouting::RouteOutput(Ptr<Packet> packet, const Address& dest)
{
  AquaSimHeader ash;
  packet->RemoveHeader(ash);
  AquaSimAddress destination = ash.GetDAddr();
  if (destination == AquaSimAddress(-1) && AquaSimAddress::IsMatchingType(dest))
    {
      destination = AquaSimAddress::ConvertFrom(dest);
    }

  PrepareDataPacket(packet, ash, destination);

  if (destination == GetLocalAddress())
    {
      AquaSimUWAodvHeader aodv;
      packet->RemoveHeader(ash);
      packet->RemoveHeader(aodv);
      return DeliverDataPacket(packet, ash, aodv);
    }

  if (IsBroadcastDestination(destination))
    {
      return BroadcastDataPacket(packet);
    }

  if (ForwardDataPacket(packet, destination))
    {
      return true;
    }

  if (!EnqueuePacket(destination, packet))
    {
      ++m_noRouteDrops;
      return false;
    }

  if (m_activeDiscoveries.find(destination) == m_activeDiscoveries.end())
    {
      StartRouteDiscovery(destination);
    }
  return true;
}

void
AquaSimUWAodvRouting::PrepareDataPacket(Ptr<Packet> packet,
                                        AquaSimHeader& ash,
                                        AquaSimAddress destination)
{
  AquaSimUWAodvHeader aodv;
  aodv.SetType(AquaSimUWAodvHeader::UWAODV_DATA);
  aodv.SetHopCount(0);
  aodv.SetHopLimit(0);
  aodv.SetRequestId(0);
  aodv.SetOrigin(GetLocalAddress());
  aodv.SetDestination(destination);
  aodv.SetOriginSeqNo(m_sequenceNumber);
  aodv.SetDestSeqNo(0);
  aodv.SetLifetime(static_cast<uint32_t>(m_routeLifetime.GetMilliSeconds()));

  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(AquaSimAddress::GetBroadcast());
  ash.SetNumForwards(0);
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(destination);
  ash.SetErrorFlag(false);
  ash.SetSize(packet->GetSize() + aodv.GetSerializedSize());
  ash.SetUId(packet->GetUid());
  ash.SetTimeStamp(Simulator::Now());

  AddAodvTag(packet);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);
}

bool
AquaSimUWAodvRouting::ForwardDataPacket(Ptr<Packet> packet, AquaSimAddress destination)
{
  RouteEntry route;
  if (!LookupRoute(destination, route))
    {
      RouteEntry staleRoute;
      if (LookupAnyRoute(destination, staleRoute) && staleRoute.valid)
        {
          std::set<AquaSimAddress> precursors =
            InvalidateRoute(destination, staleRoute.destSeqNo, staleRoute.validSeqNo);
          SendRerr(destination, staleRoute.destSeqNo, precursors);
        }
      return false;
    }

  AquaSimHeader ash;
  packet->RemoveHeader(ash);
  AquaSimUWAodvHeader aodv;
  packet->RemoveHeader(aodv);
  if (aodv.GetType() == AquaSimUWAodvHeader::UWAODV_DATA &&
      aodv.GetOrigin() == GetLocalAddress())
    {
      aodv.SetOriginSeqNo(m_sequenceNumber);
    }
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(route.nextHop);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  ++m_forwardedData;
  return SendDown(packet, route.nextHop, Seconds(0));
}

bool
AquaSimUWAodvRouting::BroadcastDataPacket(Ptr<Packet> packet)
{
  AquaSimHeader ash;
  packet->RemoveHeader(ash);
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(AquaSimAddress::GetBroadcast());
  packet->AddHeader(ash);

  ++m_forwardedData;
  return SendDown(packet, AquaSimAddress::GetBroadcast(), Seconds(0));
}

bool
AquaSimUWAodvRouting::DeliverDataPacket(Ptr<Packet> packet,
                                        AquaSimHeader ash,
                                        AquaSimUWAodvHeader aodv)
{
  ash.SetDirection(AquaSimHeader::UP);
  ash.SetSAddr(aodv.GetOrigin());
  ash.SetDAddr(aodv.GetDestination());
  packet->AddHeader(ash);
  NS_LOG_DEBUG("DELIVER local=" << GetLocalAddress()
            << " origin=" << aodv.GetOrigin()
            << " dest=" << aodv.GetDestination());
  ++m_deliveredData;
  return SendUp(packet);
}

bool
AquaSimUWAodvRouting::EnqueuePacket(AquaSimAddress destination, Ptr<Packet> packet)
{
  std::deque<Ptr<Packet>>& queue = m_pendingQueue[destination];
  if (queue.size() >= m_maxQueueLen)
    {
      ++m_queueDrops;
      return false;
    }

  queue.push_back(packet);
  ++m_queuedPackets;
  return true;
}

void
AquaSimUWAodvRouting::SendQueuedPackets(AquaSimAddress destination)
{
  std::map<AquaSimAddress, std::deque<Ptr<Packet>>>::iterator it =
    m_pendingQueue.find(destination);
  if (it == m_pendingQueue.end())
    {
      return;
    }

  while (!it->second.empty() && HasValidRoute(destination))
    {
      Ptr<Packet> packet = it->second.front();
      it->second.pop_front();
      ForwardDataPacket(packet, destination);
    }

  if (it->second.empty())
    {
      m_pendingQueue.erase(it);
    }
}

void
AquaSimUWAodvRouting::DropQueuedPackets(AquaSimAddress destination)
{
  std::map<AquaSimAddress, std::deque<Ptr<Packet>>>::iterator it =
    m_pendingQueue.find(destination);
  if (it == m_pendingQueue.end())
    {
      return;
    }

  m_noRouteDrops = m_noRouteDrops.Get() + static_cast<uint32_t>(it->second.size());
  m_pendingQueue.erase(it);
}

void
AquaSimUWAodvRouting::StartRouteDiscovery(AquaSimAddress destination)
{
  m_activeDiscoveries.insert(destination);
  m_rreqAttempts[destination] = 0;
  SendRreq(destination);
}

void
AquaSimUWAodvRouting::SendRreq(AquaSimAddress destination)
{
  uint32_t attempt = ++m_rreqAttempts[destination];
  uint16_t hopLimit = GetRreqHopLimit(destination, attempt);
  ++m_sequenceNumber;

  RouteEntry knownDestinationRoute;
  bool hasKnownDestSeqNo =
    LookupAnyRoute(destination, knownDestinationRoute) && knownDestinationRoute.validSeqNo;

  Ptr<Packet> packet = Create<Packet>();
  AquaSimUWAodvHeader aodv;
  aodv.SetType(AquaSimUWAodvHeader::UWAODV_RREQ);
  aodv.SetHopCount(0);
  aodv.SetHopLimit(hopLimit);
  aodv.SetRequestId(++m_nextRreqId);
  aodv.SetOrigin(GetLocalAddress());
  aodv.SetDestination(destination);
  aodv.SetOriginSeqNo(m_sequenceNumber);
  aodv.SetDestSeqNo(hasKnownDestSeqNo ? knownDestinationRoute.destSeqNo : 0);
  aodv.SetUnknownDestSeqNo(!hasKnownDestSeqNo);
  aodv.SetLifetime(0);

  AquaSimHeader ash;
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(AquaSimAddress::GetBroadcast());
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(AquaSimAddress::GetBroadcast());
  ash.SetSize(aodv.GetSerializedSize() + ash.GetSerializedSize());
  ash.SetUId(packet->GetUid());
  ash.SetTimeStamp(Simulator::Now());

  AddAodvTag(packet);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  ++m_rreqTx;
  SendDown(packet, AquaSimAddress::GetBroadcast(), Seconds(0));
  Simulator::Schedule(m_rreqTimeout,
                      &AquaSimUWAodvRouting::RouteRequestTimeout,
                      this,
                      destination,
                      attempt);
}

uint16_t
AquaSimUWAodvRouting::GetRreqHopLimit(AquaSimAddress destination, uint32_t attempt)
{
  uint16_t netDiameter = m_netDiameter == 0 ? m_maxHopCount : m_netDiameter;
  if (netDiameter > m_maxHopCount)
    {
      netDiameter = m_maxHopCount;
    }

  if (m_ttlStart == 0 || m_ttlIncrement == 0)
    {
      m_rreqHopLimits[destination] = netDiameter;
      return netDiameter;
    }

  uint16_t hopLimit = m_ttlStart;
  std::map<AquaSimAddress, uint16_t>::const_iterator it = m_rreqHopLimits.find(destination);
  if (attempt > 1 && it != m_rreqHopLimits.end())
    {
      uint32_t expanded = static_cast<uint32_t>(it->second) + m_ttlIncrement;
      hopLimit = expanded > 0xffff ? 0xffff : static_cast<uint16_t>(expanded);
    }

  if (hopLimit > m_ttlThreshold)
    {
      hopLimit = netDiameter;
    }
  if (hopLimit > netDiameter)
    {
      hopLimit = netDiameter;
    }

  m_rreqHopLimits[destination] = hopLimit;
  return hopLimit;
}

void
AquaSimUWAodvRouting::SendRrep(AquaSimAddress origin, AquaSimAddress destination)
{
  RouteEntry reverseRoute;
  if (!LookupRoute(origin, reverseRoute))
    {
      ++m_noRouteDrops;
      return;
    }

  SendRrep(origin, destination, reverseRoute.nextHop);
}

void
AquaSimUWAodvRouting::SendRrep(AquaSimAddress origin,
                               AquaSimAddress destination,
                               AquaSimAddress nextHop)
{
  RouteEntry destinationRoute;
  bool replyingAsDestination = destination == GetLocalAddress();
  uint16_t destinationHopCount = 0;
  uint32_t destinationSeqNo = m_sequenceNumber;
  Time lifetime = m_routeLifetime;

  if (replyingAsDestination)
    {
      if (m_sequenceNumber == 0)
        {
          ++m_sequenceNumber;
        }
      destinationSeqNo = m_sequenceNumber;
    }
  else if (LookupRoute(destination, destinationRoute))
    {
      destinationHopCount = destinationRoute.hopCount;
      destinationSeqNo = destinationRoute.destSeqNo;
      lifetime = destinationRoute.expire > Simulator::Now()
                   ? destinationRoute.expire - Simulator::Now()
                   : Seconds(0);
    }
  else
    {
      ++m_noRouteDrops;
      return;
    }

  Ptr<Packet> packet = Create<Packet>();
  AquaSimUWAodvHeader aodv;
  aodv.SetType(AquaSimUWAodvHeader::UWAODV_RREP);
  aodv.SetHopCount(destinationHopCount);
  aodv.SetHopLimit(0);
  aodv.SetRequestId(0);
  aodv.SetOrigin(origin);
  aodv.SetDestination(destination);
  aodv.SetOriginSeqNo(0);
  aodv.SetDestSeqNo(destinationSeqNo);
  aodv.SetLifetime(static_cast<uint32_t>(lifetime.GetMilliSeconds()));

  AquaSimHeader ash;
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(nextHop);
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(origin);
  ash.SetSize(aodv.GetSerializedSize() + ash.GetSerializedSize());
  ash.SetUId(packet->GetUid());
  ash.SetTimeStamp(Simulator::Now());

  AddAodvTag(packet);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  ++m_rrepTx;
  SendDown(packet, nextHop, Seconds(0));
}

void
AquaSimUWAodvRouting::QueueRrepCandidate(const RequestKey& key,
                                         const AquaSimUWAodvHeader& aodv,
                                         AquaSimAddress previousHop)
{
  RreqCandidate candidate;
  candidate.previousHop = previousHop;
  candidate.hopCount = aodv.GetHopCount() + 1;
  candidate.originSeqNo = aodv.GetOriginSeqNo();
  candidate.receiveTime = Simulator::Now();

  std::map<RequestKey, RreqCollection>::iterator it = m_rreqCollections.find(key);
  if (it == m_rreqCollections.end())
    {
      RreqCollection collection;
      collection.best = candidate;
      collection.hasCandidate = true;
      m_rreqCollections[key] = collection;
      Simulator::Schedule(m_rrepWaitTime, &AquaSimUWAodvRouting::SendCollectedRrep, this, key);
      return;
    }

  if (!it->second.hasCandidate || IsBetterRreqCandidate(candidate, it->second.best))
    {
      it->second.best = candidate;
      it->second.hasCandidate = true;
    }
}

void
AquaSimUWAodvRouting::SendCollectedRrep(RequestKey key)
{
  std::map<RequestKey, RreqCollection>::iterator it = m_rreqCollections.find(key);
  if (it == m_rreqCollections.end() || !it->second.hasCandidate)
    {
      return;
    }

  AquaSimAddress origin(key.first);
  RreqCandidate candidate = it->second.best;
  m_rreqCollections.erase(it);
  m_seenRreqs.insert(key);

  UpdateRoute(origin,
              candidate.previousHop,
              candidate.hopCount,
              candidate.originSeqNo,
              true,
              m_routeLifetime);
  SendRrep(origin, GetLocalAddress(), candidate.previousHop);
}

bool
AquaSimUWAodvRouting::IsBetterRreqCandidate(const RreqCandidate& candidate,
                                            const RreqCandidate& current) const
{
  if (IsSeqNoNewer(candidate.originSeqNo, current.originSeqNo))
    {
      return true;
    }
  if (candidate.originSeqNo != current.originSeqNo &&
      !IsSeqNoNewer(candidate.originSeqNo, current.originSeqNo))
    {
      return false;
    }
  if (candidate.hopCount < current.hopCount)
    {
      return true;
    }
  if (candidate.hopCount > current.hopCount)
    {
      return false;
    }
  return candidate.receiveTime < current.receiveTime;
}

void
AquaSimUWAodvRouting::ForwardRreq(Ptr<Packet> packet,
                                  AquaSimHeader ash,
                                  AquaSimUWAodvHeader aodv)
{
  aodv.SetHopCount(aodv.GetHopCount() + 1);
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(AquaSimAddress::GetBroadcast());
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(AquaSimAddress::GetBroadcast());
  ash.SetNumForwards(ash.GetNumForwards() + 1);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  ++m_rreqTx;
  SendDown(packet, AquaSimAddress::GetBroadcast(), Seconds(0));
}

void
AquaSimUWAodvRouting::ForwardRrep(Ptr<Packet> packet,
                                  AquaSimHeader ash,
                                  AquaSimUWAodvHeader aodv,
                                  AquaSimAddress nextHop)
{
  aodv.SetHopCount(aodv.GetHopCount() + 1);
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(nextHop);
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(aodv.GetOrigin());
  ash.SetNumForwards(ash.GetNumForwards() + 1);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  ++m_rrepTx;
  SendDown(packet, nextHop, Seconds(0));
}

void
AquaSimUWAodvRouting::SendRerr(AquaSimAddress unreachableDestination,
                               uint32_t unreachableDestSeqNo,
                               const std::set<AquaSimAddress>& precursors)
{
  ForwardRerr(unreachableDestination, unreachableDestSeqNo, precursors);
}

void
AquaSimUWAodvRouting::ForwardRerr(AquaSimAddress unreachableDestination,
                                  uint32_t unreachableDestSeqNo,
                                  const std::set<AquaSimAddress>& precursors)
{
  for (std::set<AquaSimAddress>::const_iterator it = precursors.begin();
       it != precursors.end();
       ++it)
    {
      if (*it == GetLocalAddress() || *it == AquaSimAddress::GetBroadcast())
        {
          continue;
        }

      Ptr<Packet> packet = Create<Packet>();
      AquaSimUWAodvHeader aodv;
      aodv.SetType(AquaSimUWAodvHeader::UWAODV_RERR);
      aodv.SetHopCount(0);
      aodv.SetHopLimit(0);
      aodv.SetRequestId(0);
      aodv.SetOrigin(GetLocalAddress());
      aodv.SetDestination(unreachableDestination);
      aodv.SetOriginSeqNo(m_sequenceNumber);
      aodv.SetDestSeqNo(unreachableDestSeqNo);
      aodv.SetLifetime(0);

      AquaSimHeader ash;
      ash.SetDirection(AquaSimHeader::DOWN);
      ash.SetNextHop(*it);
      ash.SetSAddr(GetLocalAddress());
      ash.SetDAddr(*it);
      ash.SetSize(aodv.GetSerializedSize() + ash.GetSerializedSize());
      ash.SetUId(packet->GetUid());
      ash.SetTimeStamp(Simulator::Now());

      AddAodvTag(packet);
      packet->AddHeader(aodv);
      packet->AddHeader(ash);

      ++m_rerrTx;
      SendDown(packet, *it, Seconds(0));
    }
}

void
AquaSimUWAodvRouting::AddPrecursor(AquaSimAddress destination, AquaSimAddress precursor)
{
  if (precursor == GetLocalAddress() ||
      precursor == AquaSimAddress::GetBroadcast() ||
      IsBroadcastDestination(destination))
    {
      return;
    }

  std::map<AquaSimAddress, RouteEntry>::iterator it = m_routeTable.find(destination);
  if (it == m_routeTable.end())
    {
      return;
    }

  it->second.precursors.insert(precursor);
}

std::set<AquaSimAddress>
AquaSimUWAodvRouting::InvalidateRoute(AquaSimAddress destination,
                                      uint32_t destSeqNo,
                                      bool validSeqNo)
{
  std::set<AquaSimAddress> precursors;
  std::map<AquaSimAddress, RouteEntry>::iterator it = m_routeTable.find(destination);
  if (it == m_routeTable.end())
    {
      return precursors;
    }

  precursors = it->second.precursors;
  it->second.valid = false;
  it->second.expire = Simulator::Now();
  if (validSeqNo &&
      (!it->second.validSeqNo ||
       destSeqNo == it->second.destSeqNo ||
       IsSeqNoNewer(destSeqNo, it->second.destSeqNo)))
    {
      it->second.destSeqNo = destSeqNo;
      it->second.validSeqNo = true;
    }
  return precursors;
}

bool
AquaSimUWAodvRouting::ShouldAcceptRerr(AquaSimAddress destination, uint32_t destSeqNo) const
{
  RouteEntry route;
  if (!LookupAnyRoute(destination, route))
    {
      return false;
    }
  if (!route.validSeqNo)
    {
      return true;
    }
  return destSeqNo == route.destSeqNo || IsSeqNoNewer(destSeqNo, route.destSeqNo);
}

void
AquaSimUWAodvRouting::RouteRequestTimeout(AquaSimAddress destination, uint32_t attempt)
{
  if (m_activeDiscoveries.find(destination) == m_activeDiscoveries.end() ||
      HasValidRoute(destination) ||
      m_rreqAttempts[destination] != attempt)
    {
      SendQueuedPackets(destination);
      return;
    }

  if (attempt < m_maxRreqAttempts)
    {
      SendRreq(destination);
      return;
    }

  m_activeDiscoveries.erase(destination);
  m_rreqAttempts.erase(destination);
  m_rreqHopLimits.erase(destination);
  DropQueuedPackets(destination);
}

void
AquaSimUWAodvRouting::AddAodvTag(Ptr<Packet> packet)
{
  AquaSimPtTag ptag;
  if (packet->PeekPacketTag(ptag))
    {
      packet->RemovePacketTag(ptag);
    }
  ptag.SetPacketType(AquaSimPtTag::PT_UW_AODV);
  packet->AddPacketTag(ptag);
}

bool
AquaSimUWAodvRouting::IsAodvPacket(Ptr<Packet> packet) const
{
  AquaSimPtTag ptag;
  return packet->PeekPacketTag(ptag) && ptag.GetPacketType() == AquaSimPtTag::PT_UW_AODV;
}

bool
AquaSimUWAodvRouting::IsPacketForMe(AquaSimHeader ash) const
{
  return ash.GetNextHop() == AquaSimAddress::GetBroadcast() ||
         ash.GetNextHop() == GetLocalAddress();
}

bool
AquaSimUWAodvRouting::IsBroadcastDestination(AquaSimAddress destination) const
{
  return destination == AquaSimAddress::GetBroadcast() ||
         destination == AquaSimAddress(static_cast<uint16_t>(0xffff));
}

void
AquaSimUWAodvRouting::DoDispose()
{
  m_routeTable.clear();
  m_pendingQueue.clear();
  m_rreqAttempts.clear();
  m_rreqHopLimits.clear();
  m_rreqCollections.clear();
  m_activeDiscoveries.clear();
  m_seenRreqs.clear();
  AquaSimRouting::DoDispose();
}

} // namespace ns3
