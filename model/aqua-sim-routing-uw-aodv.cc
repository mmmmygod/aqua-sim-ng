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
NS_OBJECT_ENSURE_REGISTERED(AquaSimUWAodvRerrListHeader);
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

void
AquaSimUWAodvHeader::SetAckRequired(bool required)
{
  if (required)
    {
      m_flags |= FLAG_ACK_REQUIRED;
    }
  else
    {
      m_flags = static_cast<uint8_t>(m_flags & ~FLAG_ACK_REQUIRED);
    }
}

void
AquaSimUWAodvHeader::SetGratuitousRrep(bool gratuitous)
{
  if (gratuitous)
    {
      m_flags |= FLAG_GRATUITOUS_RREP;
    }
  else
    {
      m_flags = static_cast<uint8_t>(m_flags & ~FLAG_GRATUITOUS_RREP);
    }
}

void
AquaSimUWAodvHeader::SetDestinationOnly(bool destinationOnly)
{
  if (destinationOnly)
    {
      m_flags |= FLAG_DESTINATION_ONLY;
    }
  else
    {
      m_flags = static_cast<uint8_t>(m_flags & ~FLAG_DESTINATION_ONLY);
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

bool
AquaSimUWAodvHeader::IsAckRequired() const
{
  return (m_flags & FLAG_ACK_REQUIRED) != 0;
}

bool
AquaSimUWAodvHeader::IsGratuitousRrep() const
{
  return (m_flags & FLAG_GRATUITOUS_RREP) != 0;
}

bool
AquaSimUWAodvHeader::IsDestinationOnly() const
{
  return (m_flags & FLAG_DESTINATION_ONLY) != 0;
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

AquaSimUWAodvRerrListHeader::AquaSimUWAodvRerrListHeader()
{
}

AquaSimUWAodvRerrListHeader::~AquaSimUWAodvRerrListHeader()
{
}

TypeId
AquaSimUWAodvRerrListHeader::GetTypeId()
{
  static TypeId tid = TypeId("ns3::AquaSimUWAodvRerrListHeader")
    .SetParent<Header>()
    .AddConstructor<AquaSimUWAodvRerrListHeader>();
  return tid;
}

TypeId
AquaSimUWAodvRerrListHeader::GetInstanceTypeId() const
{
  return GetTypeId();
}

void
AquaSimUWAodvRerrListHeader::AddUnreachableDestination(AquaSimAddress destination,
                                                       uint32_t destSeqNo)
{
  m_unreachable.push_back(std::make_pair(destination, destSeqNo));
}

const std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination>&
AquaSimUWAodvRerrListHeader::GetUnreachableDestinations() const
{
  return m_unreachable;
}

uint32_t
AquaSimUWAodvRerrListHeader::GetSerializedSize() const
{
  return 2 + static_cast<uint32_t>(m_unreachable.size()) * 6;
}

void
AquaSimUWAodvRerrListHeader::Serialize(Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU16(static_cast<uint16_t>(m_unreachable.size()));
  for (std::vector<UnreachableDestination>::const_iterator it = m_unreachable.begin();
       it != m_unreachable.end();
       ++it)
    {
      i.WriteU16(it->first.GetAsInt());
      i.WriteU32(it->second);
    }
}

uint32_t
AquaSimUWAodvRerrListHeader::Deserialize(Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint16_t count = i.ReadU16();
  m_unreachable.clear();
  for (uint16_t n = 0; n < count; ++n)
    {
      AquaSimAddress destination(i.ReadU16());
      uint32_t destSeqNo = i.ReadU32();
      m_unreachable.push_back(std::make_pair(destination, destSeqNo));
    }
  return GetSerializedSize();
}

void
AquaSimUWAodvRerrListHeader::Print(std::ostream& os) const
{
  os << "UWAODV-RERR count=" << m_unreachable.size();
  for (std::vector<UnreachableDestination>::const_iterator it = m_unreachable.begin();
       it != m_unreachable.end();
       ++it)
    {
      os << " unreachable=" << it->first << ":" << it->second;
    }
}

AquaSimUWAodvRouting::RouteEntry::RouteEntry()
  : nextHop(AquaSimAddress()),
    hopCount(0),
    destSeqNo(0),
    validSeqNo(false),
    state(ROUTE_INVALID),
    valid(false),
    expire(Seconds(0))
{
}

AquaSimUWAodvRouting::AquaSimUWAodvRouting()
  : m_nextRreqId(0),
    m_sequenceNumber(0),
    m_maxQueueLen(64),
    m_maxQueueTime(Seconds(30.0)),
    m_maxRreqAttempts(3),
    m_maxHopCount(32),
    m_ttlStart(1),
    m_ttlIncrement(2),
    m_ttlThreshold(7),
    m_netDiameter(32),
    m_enableRreqCollection(false),
    m_gratuitousReply(true),
    m_destinationOnly(false),
    m_enableBroadcast(true),
    m_rreqTimeout(Seconds(3.0)),
    m_nodeTraversalTime(Seconds(0.5)),
    m_timeoutBuffer(2),
    m_rreqJitter(Seconds(0.1)),
    m_rrepWaitTime(Seconds(0.5)),
    m_routeLifetime(Seconds(120.0)),
    m_myRouteTimeout(Seconds(0.0)),
    m_deletePeriod(Seconds(600.0)),
    m_rreqRateLimit(10),
    m_rerrRateLimit(10),
    m_rreqCount(0),
    m_rerrCount(0),
    m_rreqRateLimitWindowStart(Seconds(0)),
    m_rerrRateLimitWindowStart(Seconds(0)),
    m_enableRrepAck(true),
    m_nextHopWait(Seconds(0.0)),
    m_blacklistTimeout(Seconds(0.0)),
    m_enableHello(false),
    m_helloStarted(false),
    m_helloInterval(Seconds(10.0)),
    m_lastBroadcastTime(Seconds(0.0)),
    m_allowedHelloLoss(2),
    m_uniformRandomVariable(CreateObject<UniformRandomVariable>()),
    m_rreqTx(0),
    m_rreqRx(0),
    m_rrepTx(0),
    m_rrepRx(0),
    m_rerrTx(0),
    m_rerrRx(0),
    m_rrepAckTx(0),
    m_rrepAckRx(0),
    m_blacklistAdds(0),
    m_helloTx(0),
    m_helloRx(0),
    m_queuedPackets(0),
    m_queueDrops(0),
    m_forwardedData(0),
    m_deliveredData(0),
    m_noRouteDrops(0),
    m_duplicateDataDrops(0)
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
                  "Minimum bounded wait before retrying or failing route discovery.",
                  TimeValue(Seconds(3.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_rreqTimeout),
                  MakeTimeChecker())
    .AddAttribute("NodeTraversalTime",
                  "Estimated one-hop traversal time used to compute expanding-ring RREQ timeout.",
                  TimeValue(Seconds(0.5)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_nodeTraversalTime),
                  MakeTimeChecker())
    .AddAttribute("TimeoutBuffer",
                  "Extra hop-count buffer included in expanding-ring RREQ timeout calculation.",
                  UintegerValue(2),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_timeoutBuffer),
                  MakeUintegerChecker<uint16_t>())
    .AddAttribute("RreqJitter",
                  "Maximum random delay added before broadcasting or forwarding a RREQ.",
                  TimeValue(Seconds(0.1)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_rreqJitter),
                  MakeTimeChecker())
    .AddAttribute("RouteLifetime",
                  "Legacy alias for ActiveRouteTimeout: time an active learned route remains valid after use.",
                  TimeValue(Seconds(120.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_routeLifetime),
                  MakeTimeChecker())
    .AddAttribute("ActiveRouteTimeout",
                  "Time an active route remains valid after it is used, matching classic AODV semantics.",
                  TimeValue(Seconds(120.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_routeLifetime),
                  MakeTimeChecker())
    .AddAttribute("MyRouteTimeout",
                  "Lifetime advertised in RREP generated by the destination. A zero value computes "
                  "2 * max(ActiveRouteTimeout, PathDiscoveryTime) from current UW-AODV parameters.",
                  TimeValue(Seconds(0.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_myRouteTimeout),
                  MakeTimeChecker())
    .AddAttribute("DeletePeriod",
                  "Time an invalid route is retained before it is removed from the routing table.",
                  TimeValue(Seconds(600.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_deletePeriod),
                  MakeTimeChecker())
    .AddAttribute("MaxQueueLen",
                  "Maximum pending data packets per destination.",
                  UintegerValue(64),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_maxQueueLen),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("MaxQueueTime",
                  "Maximum time a data packet may wait in the pending queue during route discovery.",
                  TimeValue(Seconds(30.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_maxQueueTime),
                  MakeTimeChecker())
    .AddAttribute("MaxRreqAttempts",
                  "Maximum total RREQ attempts per pending destination.",
                  UintegerValue(3),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_maxRreqAttempts),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("RreqRateLimit",
                  "Maximum number of RREQ packets this node may transmit per second.",
                  UintegerValue(10),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_rreqRateLimit),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("RerrRateLimit",
                  "Maximum number of RERR packets this node may transmit per second.",
                  UintegerValue(10),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_rerrRateLimit),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("RreqRetries",
                  "AODV-style alias for the maximum total RREQ attempts per pending destination.",
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
    .AddAttribute("GratuitousReply",
                  "If true, request gratuitous RREP generation when an intermediate node replies to a RREQ.",
                  BooleanValue(true),
                  MakeBooleanAccessor(&AquaSimUWAodvRouting::m_gratuitousReply),
                  MakeBooleanChecker())
    .AddAttribute("DestinationOnly",
                  "If true, only the requested destination may reply to RREQ packets.",
                  BooleanValue(false),
                  MakeBooleanAccessor(&AquaSimUWAodvRouting::m_destinationOnly),
                  MakeBooleanChecker())
    .AddAttribute("EnableBroadcast",
                  "If true, locally deliver and forward received broadcast DATA packets, "
                  "matching classic AODV broadcast forwarding behavior.",
                  BooleanValue(true),
                  MakeBooleanAccessor(&AquaSimUWAodvRouting::m_enableBroadcast),
                  MakeBooleanChecker())
    .AddAttribute("EnableRrepAck",
                  "If true, require RREP_ACK for classic AODV intermediate replies that may traverse a unidirectional link.",
                  BooleanValue(true),
                  MakeBooleanAccessor(&AquaSimUWAodvRouting::m_enableRrepAck),
                  MakeBooleanChecker())
    .AddAttribute("NextHopWait",
                  "Time to wait for a RREP_ACK before blacklisting a possibly unidirectional "
                  "neighbor. A zero value computes 10 ms + NodeTraversalTime.",
                  TimeValue(Seconds(0.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_nextHopWait),
                  MakeTimeChecker())
    .AddAttribute("BlacklistTimeout",
                  "Time a neighbor remains blacklisted after missing a required RREP_ACK. A zero "
                  "value computes RreqRetries * NetTraversalTime using the current parameters.",
                  TimeValue(Seconds(0.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_blacklistTimeout),
                  MakeTimeChecker())
    .AddAttribute("EnableHello",
                  "If true, periodically broadcast HELLO packets and expire stale neighbors.",
                  BooleanValue(false),
                  MakeBooleanAccessor(&AquaSimUWAodvRouting::m_enableHello),
                  MakeBooleanChecker())
    .AddAttribute("HelloInterval",
                  "Interval between optional HELLO broadcasts.",
                  TimeValue(Seconds(10.0)),
                  MakeTimeAccessor(&AquaSimUWAodvRouting::m_helloInterval),
                  MakeTimeChecker())
    .AddAttribute("AllowedHelloLoss",
                  "Number of missed HELLO intervals before a neighbor is considered lost.",
                  UintegerValue(2),
                  MakeUintegerAccessor(&AquaSimUWAodvRouting::m_allowedHelloLoss),
                  MakeUintegerChecker<uint16_t>())
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
    .AddTraceSource("RrepAckTx",
                    "Number of RREP_ACK packets transmitted.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_rrepAckTx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("RrepAckRx",
                    "Number of RREP_ACK packets received.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_rrepAckRx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("BlacklistAdds",
                    "Number of times a neighbor is added to the RREP_ACK blacklist.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_blacklistAdds),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("HelloTx",
                    "Number of HELLO packets transmitted.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_helloTx),
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("HelloRx",
                    "Number of HELLO packets received.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_helloRx),
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
                    "ns3::TracedValueCallback::Uint32")
    .AddTraceSource("DuplicateDataDrops",
                    "Number of duplicate data packets dropped by the AODV duplicate packet cache.",
                    MakeTraceSourceAccessor(&AquaSimUWAodvRouting::m_duplicateDataDrops),
                    "ns3::TracedValueCallback::Uint32");
  return tid;
}

int64_t
AquaSimUWAodvRouting::AssignStreams(int64_t stream)
{
  NS_LOG_FUNCTION(this << stream);
  m_uniformRandomVariable->SetStream(stream);
  return 1;
}

bool
AquaSimUWAodvRouting::Recv(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION(this << packet << dest << protocolNumber);
  if (!packet)
    {
      return false;
    }

  PurgeRouteTable();
  PurgeExpiredQueuedPackets();
  PurgeSeenDataPackets();
  MaybeStartHello();

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
  if (it == m_routeTable.end() || !IsRouteUsable(it->second))
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
AquaSimUWAodvRouting::IsRouteUsable(const RouteEntry& route) const
{
  return IsRouteActiveState(route) && route.expire >= Simulator::Now();
}

bool
AquaSimUWAodvRouting::IsRouteActiveState(const RouteEntry& route) const
{
  return route.state == ROUTE_VALID && route.valid;
}

void
AquaSimUWAodvRouting::SetRouteState(RouteEntry& route, RouteState state) const
{
  route.state = state;
  route.valid = (state == ROUTE_VALID);
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
  if (!IsRouteUsable(current))
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
  if (!IsRouteUsable(route))
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

bool
AquaSimUWAodvRouting::CanSendIntermediateRrep(const RouteEntry& route,
                                              const AquaSimUWAodvHeader& rreq,
                                              AquaSimAddress previousHop) const
{
  return !rreq.IsDestinationOnly() &&
         route.nextHop != previousHop &&
         RouteIsFreshEnough(route, rreq);
}

bool
AquaSimUWAodvRouting::ShouldDropRreqToAvoidLoop(const RouteEntry& route,
                                                AquaSimAddress previousHop) const
{
  return IsRouteUsable(route) && route.nextHop == previousHop;
}

bool
AquaSimUWAodvRouting::ShouldRefreshForwardedRreqDestSeqNo(
  const RouteEntry& route,
  const AquaSimUWAodvHeader& rreq,
  AquaSimAddress previousHop) const
{
  return rreq.IsDestinationOnly() &&
         route.nextHop != previousHop &&
         route.validSeqNo &&
         RouteIsFreshEnough(route, rreq);
}

bool
AquaSimUWAodvRouting::ShouldSendGratuitousRrep(const RouteEntry& route,
                                               const AquaSimUWAodvHeader& rreq,
                                               AquaSimAddress previousHop) const
{
  return rreq.IsGratuitousRrep() && CanSendIntermediateRrep(route, rreq, previousHop);
}

void
AquaSimUWAodvRouting::UpdateOwnSeqNoBeforeDestinationRrep(const AquaSimUWAodvHeader& rreq)
{
  if (!rreq.IsUnknownDestSeqNo() && rreq.GetDestSeqNo() == m_sequenceNumber + 1)
    {
      ++m_sequenceNumber;
    }
}

Time
AquaSimUWAodvRouting::GetNetTraversalTime() const
{
  uint16_t netDiameter = m_netDiameter == 0 ? m_maxHopCount : m_netDiameter;
  return m_nodeTraversalTime * static_cast<int64_t>(2 * netDiameter);
}

Time
AquaSimUWAodvRouting::GetPathDiscoveryTime() const
{
  return GetNetTraversalTime() * 2;
}

Time
AquaSimUWAodvRouting::GetMyRouteTimeout() const
{
  if (m_myRouteTimeout.IsStrictlyPositive())
    {
      return m_myRouteTimeout;
    }

  Time base = m_routeLifetime > GetPathDiscoveryTime() ? m_routeLifetime : GetPathDiscoveryTime();
  return base * 2;
}

Time
AquaSimUWAodvRouting::GetNextHopWait() const
{
  if (m_nextHopWait.IsStrictlyPositive())
    {
      return m_nextHopWait;
    }
  return MilliSeconds(10) + m_nodeTraversalTime;
}

Time
AquaSimUWAodvRouting::GetBlacklistTimeout() const
{
  if (m_blacklistTimeout.IsStrictlyPositive())
    {
      return m_blacklistTimeout;
    }

  uint32_t retransmissions = m_maxRreqAttempts > 0 ? m_maxRreqAttempts - 1 : 0;
  return GetNetTraversalTime() * static_cast<int64_t>(retransmissions);
}

Time
AquaSimUWAodvRouting::GetReverseRouteLifetime(uint16_t reverseHopCount) const
{
  Time lifetime =
    GetPathDiscoveryTime() - m_nodeTraversalTime * static_cast<int64_t>(2 * reverseHopCount);
  return lifetime.IsPositive() ? lifetime : Seconds(0);
}

void
AquaSimUWAodvRouting::PurgeRouteTable()
{
  for (std::map<AquaSimAddress, RouteEntry>::iterator it = m_routeTable.begin();
       it != m_routeTable.end();)
    {
      if (it->second.expire > Simulator::Now())
        {
          ++it;
          continue;
        }

      if (it->second.state == ROUTE_INVALID)
        {
          m_routeTable.erase(it++);
          continue;
        }

      if (it->second.state == ROUTE_VALID || it->second.state == ROUTE_IN_SEARCH)
        {
          SetRouteState(it->second, ROUTE_INVALID);
          it->second.expire = Simulator::Now() + m_deletePeriod;
        }
      ++it;
    }
}

void
AquaSimUWAodvRouting::UpdateRoute(AquaSimAddress destination,
                                  AquaSimAddress nextHop,
                                  uint16_t hopCount,
                                  uint32_t destSeqNo,
                                  bool validSeqNo,
                                  Time lifetime)
{
  if (destination == AquaSimAddress::GetBroadcast() ||
      (m_device && destination == GetLocalAddress()))
    {
      return;
    }

  RouteEntry current;
  if (LookupAnyRoute(destination, current) &&
      !ShouldUpdateRoute(current, destSeqNo, validSeqNo, hopCount))
    {
      if (IsRouteActiveState(current) && current.hopCount == hopCount &&
          (!validSeqNo || !current.validSeqNo || current.destSeqNo == destSeqNo))
        {
          Time refreshedExpire = Simulator::Now() + lifetime;
          current.expire = current.expire > refreshedExpire ? current.expire : refreshedExpire;
          m_routeTable[destination] = current;
        }
      return;
    }

  RouteEntry entry;
  entry.nextHop = nextHop;
  entry.hopCount = hopCount;
  entry.destSeqNo = destSeqNo;
  entry.validSeqNo = validSeqNo;
  SetRouteState(entry, ROUTE_VALID);
  entry.expire = Simulator::Now() + lifetime;
  if (LookupAnyRoute(destination, current))
    {
      entry.precursors = current.precursors;
    }
  m_routeTable[destination] = entry;
}

void
AquaSimUWAodvRouting::RefreshRouteLifetime(AquaSimAddress destination, Time lifetime)
{
  std::map<AquaSimAddress, RouteEntry>::iterator it = m_routeTable.find(destination);
  if (it == m_routeTable.end() || !IsRouteActiveState(it->second))
    {
      return;
    }

  Time refreshedExpire = Simulator::Now() + lifetime;
  if (refreshedExpire > it->second.expire)
    {
      it->second.expire = refreshedExpire;
    }
}

void
AquaSimUWAodvRouting::RefreshActiveRouteUse(AquaSimAddress destination, AquaSimAddress nextHop)
{
  RefreshRouteLifetime(destination, m_routeLifetime);
  if (nextHop != destination &&
      nextHop != GetLocalAddress() &&
      nextHop != AquaSimAddress::GetBroadcast())
    {
      UpdateRoute(nextHop, nextHop, 1, 0, false, m_routeLifetime);
    }
  else
    {
      RefreshRouteLifetime(nextHop, m_routeLifetime);
    }
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
  UpdateNeighbor(previousHop);
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
    case AquaSimUWAodvHeader::UWAODV_HELLO:
      ++m_helloRx;
      return RecvHello(packet, ash, aodv, previousHop);
    case AquaSimUWAodvHeader::UWAODV_RREP_ACK:
      ++m_rrepAckRx;
      return RecvRrepAck(packet, ash, aodv, previousHop);
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
  DataKey key(aodv.GetOrigin().GetAsInt(), packet->GetUid());
  if (HasSeenDataPacket(key))
    {
      ++m_duplicateDataDrops;
      NS_LOG_DEBUG("local=" << GetLocalAddress()
                    << " dropping duplicate DATA origin=" << aodv.GetOrigin()
                    << " uid=" << packet->GetUid());
      return false;
    }
  RememberDataPacket(key);

  UpdateRoute(aodv.GetOrigin(),
              ash.GetSAddr(),
              aodv.GetHopCount() + 1,
              aodv.GetOriginSeqNo(),
              true,
              m_routeLifetime);

  if (aodv.GetDestination() == GetLocalAddress())
    {
      return DeliverDataPacket(packet, ash, aodv);
    }

  if (IsBroadcastDestination(aodv.GetDestination()))
    {
      // A packet may return to its origin after one or more broadcast relays.
      // Classic AODV drops this copy rather than delivering or forwarding it.
      if (aodv.GetOrigin() == GetLocalAddress())
        {
          return true;
        }

      // Local delivery consumes the AquaSim header, so forward an independent
      // copy of the payload packet when broadcast forwarding is enabled.
      bool delivered = DeliverDataPacket(packet->Copy(), ash, aodv);
      if (!m_enableBroadcast || ash.GetNumForwards() >= m_maxHopCount)
        {
          return delivered;
        }

      ash.SetNumForwards(ash.GetNumForwards() + 1);
      aodv.SetHopCount(aodv.GetHopCount() + 1);
      packet->AddHeader(aodv);
      packet->AddHeader(ash);
      return BroadcastDataPacket(packet) || delivered;
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
  PurgeBlacklist();
  PurgeSeenRreqs();
  if (IsBlacklisted(previousHop))
    {
      NS_LOG_DEBUG("local=" << GetLocalAddress()
                    << " dropping RREQ from blacklisted neighbor " << previousHop);
      return false;
    }

  RequestKey key(aodv.GetOrigin().GetAsInt(), aodv.GetRequestId());
  bool isDestination = aodv.GetDestination() == GetLocalAddress();
  bool collectAtDestination = isDestination && m_enableRreqCollection;

  if (collectAtDestination &&
      HasSeenRreq(key) &&
      m_rreqCollections.find(key) == m_rreqCollections.end())
    {
      return false;
    }

  if (!collectAtDestination && HasSeenRreq(key))
    {
      return false;
    }
  if (!collectAtDestination)
    {
      RememberRreq(key);
    }

  if (aodv.GetOrigin() == GetLocalAddress())
    {
      return false;
    }

  uint16_t reverseHopCount = aodv.GetHopCount() + 1;
  UpdateRoute(aodv.GetOrigin(),
              previousHop,
              reverseHopCount,
              aodv.GetOriginSeqNo(),
              true,
              GetReverseRouteLifetime(reverseHopCount));

  if (isDestination)
    {
      UpdateOwnSeqNoBeforeDestinationRrep(aodv);

      if (m_enableRreqCollection)
        {
          QueueRrepCandidate(key, aodv, previousHop);
          return true;
        }

      RememberRreq(key);
      SendRrep(aodv.GetOrigin(), GetLocalAddress(), previousHop);
      return true;
    }

  RouteEntry destinationRoute;
  if (LookupRoute(aodv.GetDestination(), destinationRoute))
    {
      if (ShouldDropRreqToAvoidLoop(destinationRoute, previousHop))
        {
          NS_LOG_DEBUG("local=" << GetLocalAddress()
                        << " dropping RREQ from " << previousHop
                        << " because destination route next hop matches previous hop");
          return false;
        }

      if (CanSendIntermediateRrep(destinationRoute, aodv, previousHop))
        {
          SendRrep(aodv.GetOrigin(), aodv.GetDestination());
          if (ShouldSendGratuitousRrep(destinationRoute, aodv, previousHop))
            {
              SendGratuitousRrep(aodv.GetOrigin(), aodv.GetDestination());
            }
          return true;
        }

      if (ShouldRefreshForwardedRreqDestSeqNo(destinationRoute, aodv, previousHop))
        {
          aodv.SetDestSeqNo(destinationRoute.destSeqNo);
          aodv.SetUnknownDestSeqNo(false);
        }
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
  bool ackRequired = aodv.IsAckRequired();
  if (ackRequired)
    {
      SendRrepAck(previousHop);
      aodv.SetAckRequired(false);
    }

  UpdateRoute(aodv.GetDestination(),
              previousHop,
              aodv.GetHopCount() + 1,
              aodv.GetDestSeqNo(),
              true,
              MilliSeconds(aodv.GetLifetime()));
  if (aodv.GetOrigin() == GetLocalAddress())
    {
      ClearRouteDiscovery(aodv.GetDestination());
      SendQueuedPackets(aodv.GetDestination());
      return true;
    }

  RouteEntry route;
  if (!LookupRoute(aodv.GetOrigin(), route))
    {
      ++m_noRouteDrops;
      return false;
    }

  RefreshActiveRouteUse(aodv.GetOrigin(), route.nextHop);
  RefreshActiveRouteUse(aodv.GetDestination(), previousHop);
  AddPrecursor(aodv.GetDestination(), route.nextHop);
  AddPrecursor(aodv.GetOrigin(), previousHop);
  AddPrecursor(previousHop, route.nextHop);
  AddPrecursor(route.nextHop, previousHop);
  ForwardRrep(packet, ash, aodv, route.nextHop);
  return true;
}

bool
AquaSimUWAodvRouting::RecvRrepAck(Ptr<Packet> packet,
                                  AquaSimHeader ash,
                                  AquaSimUWAodvHeader aodv,
                                  AquaSimAddress previousHop)
{
  (void)packet;
  (void)ash;

  std::map<AquaSimAddress, EventId>::iterator it = m_rrepAckTimers.find(previousHop);
  if (it != m_rrepAckTimers.end())
    {
      if (it->second.IsRunning())
        {
          it->second.Cancel();
        }
      m_rrepAckTimers.erase(it);
    }

  std::map<AquaSimAddress, RouteEntry>::iterator routeIt = m_routeTable.find(previousHop);
  if (routeIt != m_routeTable.end())
    {
      SetRouteState(routeIt->second, ROUTE_VALID);
      RefreshRouteLifetime(previousHop, m_routeLifetime);
    }

  NS_LOG_DEBUG("local=" << (m_device ? GetLocalAddress() : AquaSimAddress())
                << " received RREP_ACK from " << previousHop
                << " for destination=" << aodv.GetDestination());
  return true;
}

bool
AquaSimUWAodvRouting::RecvRerr(Ptr<Packet> packet,
                               AquaSimHeader ash,
                               AquaSimUWAodvHeader aodv,
                               AquaSimAddress previousHop)
{
  (void)ash;

  std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination> unreachable;
  if (packet->GetSize() > 0)
    {
      AquaSimUWAodvRerrListHeader rerrList;
      packet->RemoveHeader(rerrList);
      unreachable = rerrList.GetUnreachableDestinations();
    }

  if (unreachable.empty())
    {
      unreachable.push_back(std::make_pair(aodv.GetDestination(), aodv.GetDestSeqNo()));
    }

  std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination> accepted;
  std::set<AquaSimAddress> precursors;
  for (std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination>::const_iterator it =
         unreachable.begin();
       it != unreachable.end();
       ++it)
    {
      if (!ShouldAcceptRerr(it->first, it->second, previousHop))
        {
          continue;
        }

      std::set<AquaSimAddress> routePrecursors = InvalidateRoute(it->first, it->second, true);
      precursors.insert(routePrecursors.begin(), routePrecursors.end());
      accepted.push_back(*it);
    }

  if (accepted.empty())
    {
      return false;
    }

  precursors.erase(previousHop);
  ForwardRerr(accepted, precursors);
  return true;
}

bool
AquaSimUWAodvRouting::RecvHello(Ptr<Packet> packet,
                                AquaSimHeader ash,
                                AquaSimUWAodvHeader aodv,
                                AquaSimAddress previousHop)
{
  (void)packet;
  (void)ash;

  UpdateNeighbor(previousHop);
  UpdateRoute(aodv.GetOrigin(),
              previousHop,
              1,
              aodv.GetDestSeqNo(),
              true,
              MilliSeconds(aodv.GetLifetime()));
  return true;
}

bool
AquaSimUWAodvRouting::RouteOutput(Ptr<Packet> packet, const Address& dest)
{
  MaybeStartHello();

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

  if (!IsRouteDiscoveryActive(destination))
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
      NS_LOG_DEBUG("local=" << GetLocalAddress()
                   << " no usable route while forwarding DATA to=" << destination);
      AquaSimAddress origin;
      Ptr<Packet> packetCopy = packet->Copy();
      AquaSimHeader copiedAsh;
      AquaSimUWAodvHeader copiedAodv;
      packetCopy->RemoveHeader(copiedAsh);
      packetCopy->RemoveHeader(copiedAodv);
      if (copiedAodv.GetType() == AquaSimUWAodvHeader::UWAODV_DATA)
        {
          origin = copiedAodv.GetOrigin();
        }

      // A locally originated packet without a route is the normal route
      // discovery case.  RouteOutput will queue it and start an RREQ below;
      // emitting an RERR here is both contrary to AODV semantics and, under
      // TDMA, can delay the RREQ by a whole frame.
      if (origin == GetLocalAddress())
        {
          NS_LOG_DEBUG("local=" << GetLocalAddress()
                       << " has no route for locally originated DATA to=" << destination
                       << "; deferring to route discovery");
          return false;
        }

      RouteEntry staleRoute;
      bool hasKnownRoute = LookupAnyRoute(destination, staleRoute);
      if (hasKnownRoute && IsRouteActiveState(staleRoute))
        {
          std::set<AquaSimAddress> precursors =
            InvalidateRoute(destination, staleRoute.destSeqNo, staleRoute.validSeqNo);
          SendRerr(destination, staleRoute.destSeqNo, precursors);
          SendRerrWhenNoRouteToForward(destination, staleRoute.destSeqNo, origin);
        }
      else if (hasKnownRoute && staleRoute.validSeqNo)
        {
          SendRerrWhenNoRouteToForward(destination, staleRoute.destSeqNo, origin);
        }
      else
        {
          SendRerrWhenNoRouteToForward(destination, 0, origin);
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
  RefreshActiveRouteUse(destination, route.nextHop);
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
  PurgeExpiredQueuedPackets(destination);
  if (m_maxQueueLen == 0)
    {
      ++m_queueDrops;
      return false;
    }

  std::deque<PendingPacket>& queue = m_pendingQueue[destination];
  if (queue.size() >= m_maxQueueLen)
    {
      queue.pop_front();
      ++m_queueDrops;
    }

  PendingPacket pending;
  pending.packet = packet;
  pending.expire = Simulator::Now() + m_maxQueueTime;
  queue.push_back(pending);
  ++m_queuedPackets;
  return true;
}

void
AquaSimUWAodvRouting::PurgeExpiredQueuedPackets(AquaSimAddress destination)
{
  std::map<AquaSimAddress, std::deque<PendingPacket>>::iterator it =
    m_pendingQueue.find(destination);
  if (it == m_pendingQueue.end())
    {
      return;
    }

  while (!it->second.empty() && it->second.front().expire <= Simulator::Now())
    {
      NS_LOG_DEBUG("local=" << GetLocalAddress()
                   << " dropping queued DATA for=" << destination
                   << " because queue lifetime expired at="
                   << it->second.front().expire.GetSeconds());
      it->second.pop_front();
      ++m_noRouteDrops;
    }

  if (it->second.empty())
    {
      m_pendingQueue.erase(it);
    }
}

void
AquaSimUWAodvRouting::PurgeExpiredQueuedPackets()
{
  std::vector<AquaSimAddress> destinations;
  for (std::map<AquaSimAddress, std::deque<PendingPacket>>::const_iterator it =
         m_pendingQueue.begin();
       it != m_pendingQueue.end();
       ++it)
    {
      destinations.push_back(it->first);
    }

  for (std::vector<AquaSimAddress>::const_iterator it = destinations.begin();
       it != destinations.end();
       ++it)
    {
      PurgeExpiredQueuedPackets(*it);
    }
}

void
AquaSimUWAodvRouting::SendQueuedPackets(AquaSimAddress destination)
{
  PurgeExpiredQueuedPackets(destination);
  std::map<AquaSimAddress, std::deque<PendingPacket>>::iterator it =
    m_pendingQueue.find(destination);
  if (it == m_pendingQueue.end())
    {
      NS_LOG_DEBUG("local=" << GetLocalAddress()
                   << " has no queued DATA to release for=" << destination);
      return;
    }

  NS_LOG_DEBUG("local=" << GetLocalAddress()
               << " releasing " << it->second.size()
               << " queued DATA packet(s) for=" << destination
               << " routeUsable=" << HasValidRoute(destination));
  while (!it->second.empty() && HasValidRoute(destination))
    {
      Ptr<Packet> packet = it->second.front().packet;
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
  PurgeExpiredQueuedPackets(destination);
  std::map<AquaSimAddress, std::deque<PendingPacket>>::iterator it =
    m_pendingQueue.find(destination);
  if (it == m_pendingQueue.end())
    {
      return;
    }

  m_noRouteDrops = m_noRouteDrops.Get() + static_cast<uint32_t>(it->second.size());
  m_pendingQueue.erase(it);
}

bool
AquaSimUWAodvRouting::IsRouteDiscoveryActive(AquaSimAddress destination) const
{
  return m_activeDiscoveries.find(destination) != m_activeDiscoveries.end();
}

void
AquaSimUWAodvRouting::ClearRouteDiscovery(AquaSimAddress destination)
{
  m_activeDiscoveries.erase(destination);
  m_rreqAttempts.erase(destination);
  m_rreqHopLimits.erase(destination);

  std::map<AquaSimAddress, RouteEntry>::iterator it = m_routeTable.find(destination);
  if (it != m_routeTable.end() && it->second.state == ROUTE_IN_SEARCH)
    {
      SetRouteState(it->second, ROUTE_INVALID);
      it->second.expire = Simulator::Now() + m_deletePeriod;
    }
}

void
AquaSimUWAodvRouting::StartRouteDiscovery(AquaSimAddress destination)
{
  if (IsRouteDiscoveryActive(destination))
    {
      return;
    }

  m_activeDiscoveries.insert(destination);
  m_rreqAttempts[destination] = 0;
  m_rreqHopLimits.erase(destination);

  RouteEntry& route = m_routeTable[destination];
  if (!IsRouteUsable(route))
    {
      SetRouteState(route, ROUTE_IN_SEARCH);
      route.expire = Simulator::Now() + GetPathDiscoveryTime();
    }

  SendRreq(destination);
}

void
AquaSimUWAodvRouting::SendRreq(AquaSimAddress destination)
{
  if (!IsRouteDiscoveryActive(destination))
    {
      return;
    }
  if (HasValidRoute(destination))
    {
      ClearRouteDiscovery(destination);
      SendQueuedPackets(destination);
      return;
    }

  if (!CanSendRreqNow())
    {
      Simulator::Schedule(GetRreqRateLimitDelay() + MicroSeconds(100),
                          &AquaSimUWAodvRouting::SendRreq,
                          this,
                          destination);
      return;
    }

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
  aodv.SetGratuitousRrep(m_gratuitousReply);
  aodv.SetDestinationOnly(m_destinationOnly);
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

  NoteRreqSent();
  NoteBroadcastSent();
  ++m_rreqTx;
  SendDown(packet, AquaSimAddress::GetBroadcast(), GetRreqJitter());
  Simulator::Schedule(GetRreqTimeout(hopLimit),
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

Time
AquaSimUWAodvRouting::GetRreqTimeout(uint16_t hopLimit) const
{
  uint32_t bufferedHopLimit = static_cast<uint32_t>(hopLimit) + m_timeoutBuffer;
  Time ringTraversalTime =
    m_nodeTraversalTime * static_cast<int64_t>(2 * bufferedHopLimit);
  return ringTraversalTime > m_rreqTimeout ? ringTraversalTime : m_rreqTimeout;
}

Time
AquaSimUWAodvRouting::GetRreqJitter() const
{
  if (m_rreqJitter.IsZero() || m_rreqJitter.IsNegative())
    {
      return Seconds(0);
    }

  return Seconds(m_uniformRandomVariable->GetValue(0.0, m_rreqJitter.GetSeconds()));
}

bool
AquaSimUWAodvRouting::CanSendRreqNow() const
{
  return m_rreqRateLimit == 0 ||
         Simulator::Now() - m_rreqRateLimitWindowStart >= Seconds(1) ||
         m_rreqCount < m_rreqRateLimit;
}

bool
AquaSimUWAodvRouting::CanSendRerrNow() const
{
  return m_rerrRateLimit == 0 ||
         Simulator::Now() - m_rerrRateLimitWindowStart >= Seconds(1) ||
         m_rerrCount < m_rerrRateLimit;
}

void
AquaSimUWAodvRouting::NoteRreqSent()
{
  if (m_rreqRateLimit == 0)
    {
      return;
    }
  if (Simulator::Now() - m_rreqRateLimitWindowStart >= Seconds(1))
    {
      m_rreqRateLimitWindowStart = Simulator::Now();
      m_rreqCount = 0;
    }
  ++m_rreqCount;
}

void
AquaSimUWAodvRouting::NoteRerrSent()
{
  if (m_rerrRateLimit == 0)
    {
      return;
    }
  if (Simulator::Now() - m_rerrRateLimitWindowStart >= Seconds(1))
    {
      m_rerrRateLimitWindowStart = Simulator::Now();
      m_rerrCount = 0;
    }
  ++m_rerrCount;
}

Time
AquaSimUWAodvRouting::GetRreqRateLimitDelay() const
{
  if (CanSendRreqNow())
    {
      return Seconds(0);
    }
  Time nextWindow = m_rreqRateLimitWindowStart + Seconds(1);
  return nextWindow > Simulator::Now() ? nextWindow - Simulator::Now() : Seconds(0);
}

Time
AquaSimUWAodvRouting::GetRerrRateLimitDelay() const
{
  if (CanSendRerrNow())
    {
      return Seconds(0);
    }
  Time nextWindow = m_rerrRateLimitWindowStart + Seconds(1);
  return nextWindow > Simulator::Now() ? nextWindow - Simulator::Now() : Seconds(0);
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
  bool requireAck = false;

  if (replyingAsDestination)
    {
      if (m_sequenceNumber == 0)
        {
          ++m_sequenceNumber;
        }
      destinationSeqNo = m_sequenceNumber;
      lifetime = GetMyRouteTimeout();
    }
  else if (LookupRoute(destination, destinationRoute))
    {
      destinationHopCount = destinationRoute.hopCount;
      destinationSeqNo = destinationRoute.destSeqNo;
      lifetime = destinationRoute.expire > Simulator::Now()
                   ? destinationRoute.expire - Simulator::Now()
                   : Seconds(0);
      requireAck = m_enableRrepAck && destinationRoute.hopCount == 1;
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
  aodv.SetAckRequired(requireAck);

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

  if (requireAck)
    {
      RequireRrepAck(nextHop);
    }
}

void
AquaSimUWAodvRouting::SendGratuitousRrep(AquaSimAddress origin, AquaSimAddress destination)
{
  RouteEntry routeToOrigin;
  RouteEntry routeToDestination;
  if (!LookupRoute(origin, routeToOrigin) || !LookupRoute(destination, routeToDestination))
    {
      return;
    }

  Time lifetime = routeToOrigin.expire > Simulator::Now()
                    ? routeToOrigin.expire - Simulator::Now()
                    : Seconds(0);

  Ptr<Packet> packet = Create<Packet>();
  AquaSimUWAodvHeader aodv;
  aodv.SetType(AquaSimUWAodvHeader::UWAODV_RREP);
  aodv.SetHopCount(routeToOrigin.hopCount);
  aodv.SetHopLimit(0);
  aodv.SetRequestId(0);
  aodv.SetOrigin(destination);
  aodv.SetDestination(origin);
  aodv.SetOriginSeqNo(0);
  aodv.SetDestSeqNo(routeToOrigin.destSeqNo);
  aodv.SetLifetime(static_cast<uint32_t>(lifetime.GetMilliSeconds()));

  AquaSimHeader ash;
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(routeToDestination.nextHop);
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(destination);
  ash.SetSize(aodv.GetSerializedSize() + ash.GetSerializedSize());
  ash.SetUId(packet->GetUid());
  ash.SetTimeStamp(Simulator::Now());

  AddAodvTag(packet);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  ++m_rrepTx;
  SendDown(packet, routeToDestination.nextHop, Seconds(0));
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
  RememberRreq(key);

  UpdateRoute(origin,
              candidate.previousHop,
              candidate.hopCount,
              candidate.originSeqNo,
              true,
              GetReverseRouteLifetime(candidate.hopCount));
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
  if (!CanSendRreqNow())
    {
      return;
    }

  aodv.SetHopCount(aodv.GetHopCount() + 1);
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(AquaSimAddress::GetBroadcast());
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(AquaSimAddress::GetBroadcast());
  ash.SetNumForwards(ash.GetNumForwards() + 1);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  NoteRreqSent();
  NoteBroadcastSent();
  ++m_rreqTx;
  SendDown(packet, AquaSimAddress::GetBroadcast(), GetRreqJitter());
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
  std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination> unreachable;
  unreachable.push_back(std::make_pair(unreachableDestination, unreachableDestSeqNo));
  SendRerr(unreachable, precursors);
}

void
AquaSimUWAodvRouting::SendRerr(
  const std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination>& unreachable,
  const std::set<AquaSimAddress>& precursors)
{
  ForwardRerr(unreachable, precursors);
}

void
AquaSimUWAodvRouting::SendRerrWhenNoRouteToForward(AquaSimAddress unreachableDestination,
                                                   uint32_t unreachableDestSeqNo,
                                                   AquaSimAddress origin)
{
  if (origin == AquaSimAddress::GetBroadcast() || IsBroadcastDestination(origin))
    {
      return;
    }

  RouteEntry routeToOrigin;
  if (!LookupRoute(origin, routeToOrigin))
    {
      BroadcastRerr(unreachableDestination, unreachableDestSeqNo);
      return;
    }

  std::set<AquaSimAddress> precursors;
  precursors.insert(routeToOrigin.nextHop);
  SendRerr(unreachableDestination, unreachableDestSeqNo, precursors);
}

void
AquaSimUWAodvRouting::BroadcastRerr(AquaSimAddress unreachableDestination,
                                    uint32_t unreachableDestSeqNo)
{
  if (!m_device || !CanSendRerrNow())
    {
      return;
    }

  Ptr<Packet> packet = Create<Packet>();
  AquaSimUWAodvHeader aodv;
  aodv.SetType(AquaSimUWAodvHeader::UWAODV_RERR);
  aodv.SetHopCount(0);
  aodv.SetHopLimit(1);
  aodv.SetRequestId(1);
  aodv.SetOrigin(GetLocalAddress());
  aodv.SetDestination(unreachableDestination);
  aodv.SetOriginSeqNo(m_sequenceNumber);
  aodv.SetDestSeqNo(unreachableDestSeqNo);
  aodv.SetLifetime(0);

  AquaSimUWAodvRerrListHeader rerrList;
  rerrList.AddUnreachableDestination(unreachableDestination, unreachableDestSeqNo);

  AquaSimHeader ash;
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(AquaSimAddress::GetBroadcast());
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(AquaSimAddress::GetBroadcast());
  ash.SetSize(aodv.GetSerializedSize() + rerrList.GetSerializedSize() + ash.GetSerializedSize());
  ash.SetUId(packet->GetUid());
  ash.SetTimeStamp(Simulator::Now());

  AddAodvTag(packet);
  packet->AddHeader(rerrList);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  NoteRerrSent();
  ++m_rerrTx;
  SendDown(packet, AquaSimAddress::GetBroadcast(), Seconds(0));
}

void
AquaSimUWAodvRouting::SendRrepAck(AquaSimAddress nextHop)
{
  if (nextHop == GetLocalAddress() || nextHop == AquaSimAddress::GetBroadcast())
    {
      return;
    }

  Ptr<Packet> packet = Create<Packet>();
  AquaSimUWAodvHeader aodv;
  aodv.SetType(AquaSimUWAodvHeader::UWAODV_RREP_ACK);
  aodv.SetHopCount(0);
  aodv.SetHopLimit(1);
  aodv.SetRequestId(0);
  aodv.SetOrigin(GetLocalAddress());
  aodv.SetDestination(nextHop);
  aodv.SetOriginSeqNo(m_sequenceNumber);
  aodv.SetDestSeqNo(0);
  aodv.SetLifetime(0);

  AquaSimHeader ash;
  ash.SetDirection(AquaSimHeader::DOWN);
  ash.SetNextHop(nextHop);
  ash.SetSAddr(GetLocalAddress());
  ash.SetDAddr(nextHop);
  ash.SetSize(aodv.GetSerializedSize() + ash.GetSerializedSize());
  ash.SetUId(packet->GetUid());
  ash.SetTimeStamp(Simulator::Now());

  AddAodvTag(packet);
  packet->AddHeader(aodv);
  packet->AddHeader(ash);

  ++m_rrepAckTx;
  SendDown(packet, nextHop, Seconds(0));
}

void
AquaSimUWAodvRouting::RequireRrepAck(AquaSimAddress nextHop)
{
  std::map<AquaSimAddress, EventId>::iterator it = m_rrepAckTimers.find(nextHop);
  if (it != m_rrepAckTimers.end() && it->second.IsRunning())
    {
      it->second.Cancel();
    }

  m_rrepAckTimers[nextHop] =
    Simulator::Schedule(GetNextHopWait(), &AquaSimUWAodvRouting::RrepAckTimeout, this, nextHop);
}

void
AquaSimUWAodvRouting::RrepAckTimeout(AquaSimAddress nextHop)
{
  m_rrepAckTimers.erase(nextHop);
  BlacklistNeighbor(nextHop);
}

void
AquaSimUWAodvRouting::BlacklistNeighbor(AquaSimAddress neighbor)
{
  if ((m_device && neighbor == GetLocalAddress()) || neighbor == AquaSimAddress::GetBroadcast())
    {
      return;
    }

  m_blacklist[neighbor] = Simulator::Now() + GetBlacklistTimeout();
  ++m_blacklistAdds;
  NS_LOG_DEBUG("local=" << GetLocalAddress()
                << " blacklisted neighbor=" << neighbor
                << " until=" << m_blacklist[neighbor].GetSeconds());
}

bool
AquaSimUWAodvRouting::IsBlacklisted(AquaSimAddress neighbor) const
{
  std::map<AquaSimAddress, Time>::const_iterator it = m_blacklist.find(neighbor);
  return it != m_blacklist.end() && it->second > Simulator::Now();
}

void
AquaSimUWAodvRouting::PurgeBlacklist()
{
  std::vector<AquaSimAddress> expired;
  for (std::map<AquaSimAddress, Time>::const_iterator it = m_blacklist.begin();
       it != m_blacklist.end();
       ++it)
    {
      if (it->second <= Simulator::Now())
        {
          expired.push_back(it->first);
        }
    }

  for (std::vector<AquaSimAddress>::const_iterator it = expired.begin();
       it != expired.end();
       ++it)
    {
      m_blacklist.erase(*it);
    }
}

void
AquaSimUWAodvRouting::RememberRreq(const RequestKey& key)
{
  m_seenRreqs[key] = Simulator::Now() + GetPathDiscoveryTime();
}

bool
AquaSimUWAodvRouting::HasSeenRreq(const RequestKey& key) const
{
  std::map<RequestKey, Time>::const_iterator it = m_seenRreqs.find(key);
  return it != m_seenRreqs.end() && it->second > Simulator::Now();
}

void
AquaSimUWAodvRouting::PurgeSeenRreqs()
{
  std::vector<RequestKey> expired;
  for (std::map<RequestKey, Time>::const_iterator it = m_seenRreqs.begin();
       it != m_seenRreqs.end();
       ++it)
    {
      if (it->second <= Simulator::Now())
        {
          expired.push_back(it->first);
        }
    }

  for (std::vector<RequestKey>::const_iterator it = expired.begin();
       it != expired.end();
       ++it)
    {
      m_seenRreqs.erase(*it);
    }
}

void
AquaSimUWAodvRouting::RememberDataPacket(const DataKey& key)
{
  m_seenDataPackets[key] = Simulator::Now() + GetPathDiscoveryTime();
}

bool
AquaSimUWAodvRouting::HasSeenDataPacket(const DataKey& key) const
{
  std::map<DataKey, Time>::const_iterator it = m_seenDataPackets.find(key);
  return it != m_seenDataPackets.end() && it->second > Simulator::Now();
}

void
AquaSimUWAodvRouting::PurgeSeenDataPackets()
{
  std::vector<DataKey> expired;
  for (std::map<DataKey, Time>::const_iterator it = m_seenDataPackets.begin();
       it != m_seenDataPackets.end();
       ++it)
    {
      if (it->second <= Simulator::Now())
        {
          expired.push_back(it->first);
        }
    }

  for (std::vector<DataKey>::const_iterator it = expired.begin();
       it != expired.end();
       ++it)
    {
      m_seenDataPackets.erase(*it);
    }
}

void
AquaSimUWAodvRouting::ForwardRerr(AquaSimAddress unreachableDestination,
                                  uint32_t unreachableDestSeqNo,
                                  const std::set<AquaSimAddress>& precursors)
{
  std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination> unreachable;
  unreachable.push_back(std::make_pair(unreachableDestination, unreachableDestSeqNo));
  ForwardRerr(unreachable, precursors);
}

void
AquaSimUWAodvRouting::ForwardRerr(
  const std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination>& unreachable,
  const std::set<AquaSimAddress>& precursors)
{
  if (unreachable.empty())
    {
      return;
    }
  if (!CanSendRerrNow())
    {
      return;
    }

  for (std::set<AquaSimAddress>::const_iterator it = precursors.begin();
       it != precursors.end();
       ++it)
    {
      if (*it == GetLocalAddress() || *it == AquaSimAddress::GetBroadcast())
        {
          continue;
        }
      if (!CanSendRerrNow())
        {
          return;
        }

      Ptr<Packet> packet = Create<Packet>();
      AquaSimUWAodvHeader aodv;
      aodv.SetType(AquaSimUWAodvHeader::UWAODV_RERR);
      aodv.SetHopCount(0);
      aodv.SetHopLimit(0);
      aodv.SetRequestId(static_cast<uint32_t>(unreachable.size()));
      aodv.SetOrigin(GetLocalAddress());
      aodv.SetDestination(unreachable.front().first);
      aodv.SetOriginSeqNo(m_sequenceNumber);
      aodv.SetDestSeqNo(unreachable.front().second);
      aodv.SetLifetime(0);

      AquaSimUWAodvRerrListHeader rerrList;
      for (std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination>::const_iterator unreachableIt =
             unreachable.begin();
           unreachableIt != unreachable.end();
           ++unreachableIt)
        {
          rerrList.AddUnreachableDestination(unreachableIt->first, unreachableIt->second);
        }

      AquaSimHeader ash;
      ash.SetDirection(AquaSimHeader::DOWN);
      ash.SetNextHop(*it);
      ash.SetSAddr(GetLocalAddress());
      ash.SetDAddr(*it);
      ash.SetSize(aodv.GetSerializedSize() + rerrList.GetSerializedSize() + ash.GetSerializedSize());
      ash.SetUId(packet->GetUid());
      ash.SetTimeStamp(Simulator::Now());

      AddAodvTag(packet);
      packet->AddHeader(rerrList);
      packet->AddHeader(aodv);
      packet->AddHeader(ash);

      NoteRerrSent();
      ++m_rerrTx;
      SendDown(packet, *it, Seconds(0));
    }
}

void
AquaSimUWAodvRouting::AddPrecursor(AquaSimAddress destination, AquaSimAddress precursor)
{
  if ((m_device && precursor == GetLocalAddress()) ||
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
  SetRouteState(it->second, ROUTE_INVALID);
  it->second.expire = Simulator::Now() + m_deletePeriod;
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
AquaSimUWAodvRouting::ShouldAcceptRerr(AquaSimAddress destination,
                                       uint32_t destSeqNo,
                                       AquaSimAddress previousHop) const
{
  RouteEntry route;
  if (!LookupAnyRoute(destination, route))
    {
      return false;
    }
  if (!IsRouteActiveState(route) || route.nextHop != previousHop)
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
AquaSimUWAodvRouting::MaybeStartHello()
{
  if (!m_enableHello || m_helloStarted)
    {
      return;
    }

  m_helloStarted = true;
  Simulator::Schedule(GetRreqJitter(), &AquaSimUWAodvRouting::SendHello, this);
}

void
AquaSimUWAodvRouting::SendHello()
{
  if (!m_enableHello)
    {
      return;
    }

  PurgeDeadNeighbors();
  Time helloDelay = GetHelloDelay();
  if (helloDelay.IsStrictlyPositive())
    {
      NS_LOG_DEBUG("local=" << GetLocalAddress()
                    << " deferring HELLO for " << helloDelay.GetSeconds()
                    << "s because a broadcast was sent recently");
      m_lastBroadcastTime = Seconds(0);
      Simulator::Schedule(helloDelay, &AquaSimUWAodvRouting::SendHello, this);
      return;
    }
  m_lastBroadcastTime = Seconds(0);

  Ptr<Packet> packet = Create<Packet>();
  AquaSimUWAodvHeader aodv;
  aodv.SetType(AquaSimUWAodvHeader::UWAODV_HELLO);
  aodv.SetHopCount(0);
  aodv.SetHopLimit(1);
  aodv.SetRequestId(0);
  aodv.SetOrigin(GetLocalAddress());
  aodv.SetDestination(GetLocalAddress());
  aodv.SetOriginSeqNo(m_sequenceNumber);
  aodv.SetDestSeqNo(m_sequenceNumber);
  Time helloLifetime = m_helloInterval * static_cast<int64_t>(m_allowedHelloLoss);
  aodv.SetLifetime(static_cast<uint32_t>(helloLifetime.GetMilliSeconds()));

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

  ++m_helloTx;
  NoteBroadcastSent();
  SendDown(packet, AquaSimAddress::GetBroadcast(), Seconds(0));
  Simulator::Schedule(m_helloInterval, &AquaSimUWAodvRouting::SendHello, this);
}

void
AquaSimUWAodvRouting::NoteBroadcastSent()
{
  m_lastBroadcastTime = Simulator::Now();
}

Time
AquaSimUWAodvRouting::GetHelloDelay() const
{
  if (m_lastBroadcastTime <= Seconds(0))
    {
      return Seconds(0);
    }

  Time offset = Simulator::Now() - m_lastBroadcastTime;
  if (offset >= m_helloInterval)
    {
      return Seconds(0);
    }
  return m_helloInterval - offset;
}

void
AquaSimUWAodvRouting::UpdateNeighbor(AquaSimAddress neighbor)
{
  if (!m_enableHello ||
      neighbor == GetLocalAddress() ||
      neighbor == AquaSimAddress::GetBroadcast())
    {
      return;
    }

  m_neighbors[neighbor] = Simulator::Now();
}

void
AquaSimUWAodvRouting::PurgeDeadNeighbors()
{
  if (!m_enableHello)
    {
      return;
    }

  Time allowedLossTime = Seconds(m_helloInterval.GetSeconds() * m_allowedHelloLoss);
  std::vector<AquaSimAddress> lostNeighbors;
  for (std::map<AquaSimAddress, Time>::const_iterator it = m_neighbors.begin();
       it != m_neighbors.end();
       ++it)
    {
      if (Simulator::Now() - it->second > allowedLossTime)
        {
          lostNeighbors.push_back(it->first);
        }
    }

  for (std::vector<AquaSimAddress>::const_iterator it = lostNeighbors.begin();
       it != lostNeighbors.end();
       ++it)
    {
      m_neighbors.erase(*it);
      InvalidateRoutesViaNeighbor(*it);
    }
}

void
AquaSimUWAodvRouting::InvalidateRoutesViaNeighbor(AquaSimAddress neighbor)
{
  std::vector<AquaSimAddress> affectedDestinations;
  for (std::map<AquaSimAddress, RouteEntry>::const_iterator it = m_routeTable.begin();
       it != m_routeTable.end();
       ++it)
    {
      if (IsRouteActiveState(it->second) && it->second.nextHop == neighbor)
        {
          affectedDestinations.push_back(it->first);
        }
    }

  std::vector<AquaSimUWAodvRerrListHeader::UnreachableDestination> unreachable;
  std::set<AquaSimAddress> precursors;
  for (std::vector<AquaSimAddress>::const_iterator it = affectedDestinations.begin();
       it != affectedDestinations.end();
       ++it)
    {
      RouteEntry route;
      if (!LookupAnyRoute(*it, route) || !IsRouteActiveState(route))
        {
          continue;
        }

      uint32_t unreachableSeqNo = route.validSeqNo ? route.destSeqNo + 1 : m_sequenceNumber;
      std::set<AquaSimAddress> routePrecursors = InvalidateRoute(*it, unreachableSeqNo, true);
      precursors.insert(routePrecursors.begin(), routePrecursors.end());
      unreachable.push_back(std::make_pair(*it, unreachableSeqNo));
    }

  precursors.erase(neighbor);
  SendRerr(unreachable, precursors);
}

void
AquaSimUWAodvRouting::RouteRequestTimeout(AquaSimAddress destination, uint32_t attempt)
{
  if (!IsRouteDiscoveryActive(destination))
    {
      return;
    }

  if (HasValidRoute(destination))
    {
      ClearRouteDiscovery(destination);
      SendQueuedPackets(destination);
      return;
    }

  std::map<AquaSimAddress, uint32_t>::const_iterator attemptsIt =
    m_rreqAttempts.find(destination);
  if (attemptsIt == m_rreqAttempts.end() || attemptsIt->second != attempt)
    {
      return;
    }

  if (attempt < m_maxRreqAttempts)
    {
      SendRreq(destination);
      return;
    }

  ClearRouteDiscovery(destination);
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
  m_neighbors.clear();
  m_blacklist.clear();
  for (std::map<AquaSimAddress, EventId>::iterator it = m_rrepAckTimers.begin();
       it != m_rrepAckTimers.end();
       ++it)
    {
      if (it->second.IsRunning())
        {
          it->second.Cancel();
        }
    }
  m_rrepAckTimers.clear();
  m_activeDiscoveries.clear();
  m_seenRreqs.clear();
  m_seenDataPackets.clear();
  m_enableHello = false;
  m_helloStarted = false;
  AquaSimRouting::DoDispose();
}

} // namespace ns3
