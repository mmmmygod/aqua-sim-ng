/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UwAodvBroadcast");

static uint32_t g_relayDelivered = 0;
static uint32_t g_sinkDelivered = 0;
static uint32_t g_forwardedData = 0;
static uint32_t g_duplicateDataDrops = 0;

static void
CountRelayDelivery(Ptr<const Packet> packet)
{
  (void)packet;
  ++g_relayDelivered;
}

static void
CountSinkDelivery(Ptr<const Packet> packet)
{
  (void)packet;
  ++g_sinkDelivered;
}

static void
CountForwardedData(uint32_t oldValue, uint32_t newValue)
{
  g_forwardedData += newValue - oldValue;
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
  double simStop = 10.0;
  uint32_t packetSize = 32;
  double spacing = 800.0;
  double txRange = 1000.0;
  bool enableBroadcast = true;

  CommandLine cmd;
  cmd.AddValue("simStop", "Simulation stop time in seconds", simStop);
  cmd.AddValue("packetSize", "Broadcast DATA packet size in bytes", packetSize);
  cmd.AddValue("spacing", "Distance between adjacent line nodes in meters", spacing);
  cmd.AddValue("txRange", "Acoustic transmission range in meters", txRange);
  cmd.AddValue("enableBroadcast", "Enable forwarding of received broadcast DATA packets", enableBroadcast);
  cmd.Parse(argc, argv);

  NodeContainer nodes;
  nodes.Create(3);

  PacketSocketHelper socketHelper;
  socketHelper.Install(nodes);

  AquaSimChannelHelper channel = AquaSimChannelHelper::Default();
  channel.SetPropagation("ns3::AquaSimRangePropagation");

  AquaSimHelper asHelper = AquaSimHelper::Default();
  asHelper.SetChannel(channel.Create());
  asHelper.SetMac("ns3::AquaSimBroadcastMac");
  asHelper.SetRouting("ns3::AquaSimUWAodvRouting",
                      "EnableBroadcast",
                      BooleanValue(enableBroadcast),
                      "MaxHopCount",
                      UintegerValue(1));

  NetDeviceContainer devices;
  for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
      Ptr<AquaSimNetDevice> device = CreateObject<AquaSimNetDevice>();
      devices.Add(asHelper.Create(nodes.Get(i), device));
      device->GetPhy()->SetTransRange(txRange);
    }

  Ptr<ListPositionAllocator> positions = CreateObject<ListPositionAllocator>();
  positions->Add(Vector(0.0, 0.0, 0.0));
  positions->Add(Vector(spacing, 0.0, 0.0));
  positions->Add(Vector(2.0 * spacing, 0.0, 0.0));

  MobilityHelper mobility;
  mobility.SetPositionAllocator(positions);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes);

  PacketSocketAddress destination;
  destination.SetAllDevices();
  destination.SetPhysicalAddress(AquaSimAddress::GetBroadcast());
  destination.SetProtocol(0);

  TypeId psfid = TypeId::LookupByName("ns3::PacketSocketFactory");
  Ptr<Socket> sourceSocket = Socket::CreateSocket(nodes.Get(0), psfid);
  sourceSocket->Bind();
  sourceSocket->Connect(destination);
  Simulator::Schedule(Seconds(1.0), &SendOnePacket, sourceSocket, packetSize);

  Ptr<AquaSimNetDevice> relay = DynamicCast<AquaSimNetDevice>(devices.Get(1));
  Ptr<AquaSimNetDevice> sink = DynamicCast<AquaSimNetDevice>(devices.Get(2));
  relay->GetRouting()->TraceConnectWithoutContext("PacketReceived",
                                                   MakeCallback(&CountRelayDelivery));
  sink->GetRouting()->TraceConnectWithoutContext("PacketReceived",
                                                  MakeCallback(&CountSinkDelivery));

  for (uint32_t i = 0; i < devices.GetN(); ++i)
    {
      Ptr<AquaSimNetDevice> device = DynamicCast<AquaSimNetDevice>(devices.Get(i));
      Ptr<AquaSimRouting> routing = device->GetRouting();
      routing->TraceConnectWithoutContext("ForwardedData", MakeCallback(&CountForwardedData));
      routing->TraceConnectWithoutContext("DuplicateDataDrops",
                                          MakeCallback(&CountDuplicateDataDrops));
    }

  Simulator::Stop(Seconds(simStop));
  Simulator::Run();
  Simulator::Destroy();

  const bool relayDelivered = g_relayDelivered == 1;
  const bool sinkDelivered = enableBroadcast ? g_sinkDelivered == 1 : g_sinkDelivered == 0;
  const bool forwardingObserved = enableBroadcast ? g_forwardedData >= 2 : g_forwardedData == 1;
  const bool passed = relayDelivered && sinkDelivered && forwardingObserved;

  std::cout << "UW-AODV broadcast deliveries: relay=" << g_relayDelivered
            << " sink=" << g_sinkDelivered << std::endl;
  std::cout << "UW-AODV broadcast counters: ForwardedData=" << g_forwardedData
            << " DuplicateDataDrops=" << g_duplicateDataDrops
            << " EnableBroadcast=" << enableBroadcast << std::endl;
  std::cout << "UW-AODV broadcast check: " << (passed ? "PASS" : "FAIL") << std::endl;
  asHelper.GetChannel()->PrintCounters();
  return passed ? 0 : 1;
}
