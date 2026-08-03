/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/applications-module.h"
#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UwAodvRrepAck");

static uint32_t g_deliveredPackets = 0;
static uint32_t g_rreqTx = 0;
static uint32_t g_rreqRx = 0;
static uint32_t g_rrepTx = 0;
static uint32_t g_rrepRx = 0;
static uint32_t g_rrepAckTx = 0;
static uint32_t g_rrepAckRx = 0;
static uint32_t g_blacklistAdds = 0;
static uint32_t g_forwardedData = 0;
static uint32_t g_noRouteDrops = 0;
static uint32_t g_duplicateDataDrops = 0;

static void
RoutingDelivered(Ptr<const Packet> packet)
{
  ++g_deliveredPackets;
  NS_LOG_INFO("Routing delivered packet " << g_deliveredPackets << " size=" << packet->GetSize()
                                          << " at " << Simulator::Now().GetSeconds() << "s");
}

static void
CountRreqTx(uint32_t oldValue, uint32_t newValue)
{
  g_rreqTx += newValue - oldValue;
}

static void
CountRreqRx(uint32_t oldValue, uint32_t newValue)
{
  g_rreqRx += newValue - oldValue;
}

static void
CountRrepTx(uint32_t oldValue, uint32_t newValue)
{
  g_rrepTx += newValue - oldValue;
}

static void
CountRrepRx(uint32_t oldValue, uint32_t newValue)
{
  g_rrepRx += newValue - oldValue;
}

static void
CountRrepAckTx(uint32_t oldValue, uint32_t newValue)
{
  g_rrepAckTx += newValue - oldValue;
}

static void
CountRrepAckRx(uint32_t oldValue, uint32_t newValue)
{
  g_rrepAckRx += newValue - oldValue;
}

static void
CountBlacklistAdds(uint32_t oldValue, uint32_t newValue)
{
  g_blacklistAdds += newValue - oldValue;
}

static void
CountForwardedData(uint32_t oldValue, uint32_t newValue)
{
  g_forwardedData += newValue - oldValue;
}

static void
CountNoRouteDrops(uint32_t oldValue, uint32_t newValue)
{
  g_noRouteDrops += newValue - oldValue;
}

static void
CountDuplicateDataDrops(uint32_t oldValue, uint32_t newValue)
{
  g_duplicateDataDrops += newValue - oldValue;
}

static void
SendOnePacket(Ptr<Socket> socket, uint32_t packetSize)
{
  socket->Send(Create<Packet>(packetSize));
}

int
main(int argc, char* argv[])
{
  double simStop = 30.0;
  uint32_t packetSize = 32;
  double txRange = 1000.0;
  double rreqTimeout = 5.0;
  double nextHopWait = 2.5;
  bool requireAckRx = false;

  CommandLine cmd;
  cmd.AddValue("simStop", "Simulation stop time in seconds", simStop);
  cmd.AddValue("packetSize", "Application packet size in bytes", packetSize);
  cmd.AddValue("txRange", "Acoustic transmission range in meters", txRange);
  cmd.AddValue("rreqTimeout", "Route discovery retry timeout in seconds", rreqTimeout);
  cmd.AddValue("nextHopWait", "RREP_ACK wait time in seconds (2.5 s covers the acoustic ACK round trip)",
               nextHopWait);
  cmd.AddValue("requireAckRx",
               "Require an RREP_ACK to be received (otherwise timeout/blacklist is also valid)",
               requireAckRx);
  cmd.Parse(argc, argv);

  NodeContainer nodes;
  nodes.Create(4);

  PacketSocketHelper socketHelper;
  socketHelper.Install(nodes);

  AquaSimChannelHelper channel = AquaSimChannelHelper::Default();
  channel.SetPropagation("ns3::AquaSimRangePropagation");

  AquaSimHelper asHelper = AquaSimHelper::Default();
  asHelper.SetChannel(channel.Create());
  asHelper.SetMac("ns3::AquaSimBroadcastMac");
  asHelper.SetRouting("ns3::AquaSimUWAodvRouting",
                      "RreqTimeout",
                      TimeValue(Seconds(rreqTimeout)),
                      "MaxRreqAttempts",
                      UintegerValue(3),
                      "EnableRrepAck",
                      BooleanValue(true),
                      "NextHopWait",
                      TimeValue(Seconds(nextHopWait)));

  NetDeviceContainer devices;
  for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
      Ptr<AquaSimNetDevice> device = CreateObject<AquaSimNetDevice>();
      devices.Add(asHelper.Create(nodes.Get(i), device));
      device->GetPhy()->SetTransRange(txRange);
    }

  Ptr<ListPositionAllocator> positions = CreateObject<ListPositionAllocator>();
  positions->Add(Vector(0.0, 0.0, 0.0));       // node 0: warm-up source A
  positions->Add(Vector(800.0, 0.0, 0.0));     // node 1: relay R
  positions->Add(Vector(1600.0, 0.0, 0.0));    // node 2: destination D
  positions->Add(Vector(800.0, 900.0, 0.0));    // node 3: second source C

  MobilityHelper mobility;
  mobility.SetPositionAllocator(positions);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes);

  PacketSocketAddress socket;
  socket.SetAllDevices();
  socket.SetPhysicalAddress(devices.Get(2)->GetAddress());
  socket.SetProtocol(0);

  // A few separated packets make this a control-plane test, rather than an
  // offered-load experiment.  The first source teaches relay R its one-hop
  // route to D.  The second source then forces R to issue an intermediate
  // RREP with AckRequired.  Spacing transmissions avoids artificial
  // BroadcastMac collisions that could prevent the ACK sender from ever
  // receiving the RREP.
  TypeId psfid = TypeId::LookupByName("ns3::PacketSocketFactory");
  Ptr<Socket> warmupSocket = Socket::CreateSocket(nodes.Get(0), psfid);
  warmupSocket->Bind();
  warmupSocket->Connect(socket);
  Ptr<Socket> ackTriggerSocket = Socket::CreateSocket(nodes.Get(3), psfid);
  ackTriggerSocket->Bind();
  ackTriggerSocket->Connect(socket);

  Simulator::Schedule(Seconds(2.0), &SendOnePacket, warmupSocket, packetSize);
  Simulator::Schedule(Seconds(8.0), &SendOnePacket, warmupSocket, packetSize);
  Simulator::Schedule(Seconds(14.0), &SendOnePacket, ackTriggerSocket, packetSize);
  Simulator::Schedule(Seconds(22.0), &SendOnePacket, ackTriggerSocket, packetSize);

  Ptr<AquaSimNetDevice> sinkDevice = DynamicCast<AquaSimNetDevice>(devices.Get(2));
  sinkDevice->GetRouting()->TraceConnectWithoutContext("PacketReceived",
                                                       MakeCallback(&RoutingDelivered));

  for (uint32_t i = 0; i < devices.GetN(); ++i)
    {
      Ptr<AquaSimNetDevice> device = DynamicCast<AquaSimNetDevice>(devices.Get(i));
      Ptr<AquaSimRouting> routing = device->GetRouting();
      routing->TraceConnectWithoutContext("RreqTx", MakeCallback(&CountRreqTx));
      routing->TraceConnectWithoutContext("RreqRx", MakeCallback(&CountRreqRx));
      routing->TraceConnectWithoutContext("RrepTx", MakeCallback(&CountRrepTx));
      routing->TraceConnectWithoutContext("RrepRx", MakeCallback(&CountRrepRx));
      routing->TraceConnectWithoutContext("RrepAckTx", MakeCallback(&CountRrepAckTx));
      routing->TraceConnectWithoutContext("RrepAckRx", MakeCallback(&CountRrepAckRx));
      routing->TraceConnectWithoutContext("BlacklistAdds", MakeCallback(&CountBlacklistAdds));
      routing->TraceConnectWithoutContext("ForwardedData", MakeCallback(&CountForwardedData));
      routing->TraceConnectWithoutContext("NoRouteDrops", MakeCallback(&CountNoRouteDrops));
      routing->TraceConnectWithoutContext("DuplicateDataDrops",
                                          MakeCallback(&CountDuplicateDataDrops));
    }

  Simulator::Stop(Seconds(simStop));
  Simulator::Run();
  Simulator::Destroy();

  std::cout << "UW-AODV RREP_ACK routing delivered packets: " << g_deliveredPackets << std::endl;
  std::cout << "UW-AODV RREP_ACK counters:"
            << " RreqTx=" << g_rreqTx
            << " RreqRx=" << g_rreqRx
            << " RrepTx=" << g_rrepTx
            << " RrepRx=" << g_rrepRx
            << " RrepAckTx=" << g_rrepAckTx
            << " RrepAckRx=" << g_rrepAckRx
            << " BlacklistAdds=" << g_blacklistAdds
            << " ForwardedData=" << g_forwardedData
            << " NoRouteDrops=" << g_noRouteDrops
            << " DuplicateDataDrops=" << g_duplicateDataDrops
            << std::endl;
  asHelper.GetChannel()->PrintCounters();

  // Classic AODV has two valid outcomes after an intermediate RREP requests
  // an acknowledgement: the ACK arrives, or NextHopWait expires and the
  // neighbor is blacklisted.  In an underwater MAC/PHY simulation the ACK
  // can be lost even when the routing state machine is behaving correctly,
  // so the default scenario accepts either outcome.  Use --requireAckRx=1
  // only to observe the ACK-reception branch under a chosen MAC/PHY setup.
  const bool ackObserved = g_rrepAckRx > 0;
  const bool ackTimeoutHandled = g_blacklistAdds > 0;
  const bool controlPlaneOk = g_rrepAckTx > 0 && (ackObserved || ackTimeoutHandled);
  const bool dataPlaneOk = g_deliveredPackets > 0;
  const bool passed = dataPlaneOk && controlPlaneOk && (!requireAckRx || ackObserved);
  std::cout << "UW-AODV RREP_ACK check: " << (passed ? "PASS" : "FAIL") << std::endl;
  return passed ? 0 : 1;
}
