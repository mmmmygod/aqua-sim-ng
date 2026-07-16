/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/applications-module.h"
#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UwAodvLine");

static uint32_t g_rxPackets = 0;
static uint32_t g_deliveredPackets = 0;

static void
RecvPacket(Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  while ((packet = socket->Recv()))
    {
      ++g_rxPackets;
      NS_LOG_INFO("Received packet " << g_rxPackets << " size=" << packet->GetSize()
                                     << " at " << Simulator::Now().GetSeconds() << "s");
    }
}

static void
RoutingDelivered(Ptr<const Packet> packet)
{
  ++g_deliveredPackets;
  NS_LOG_INFO("Routing delivered packet " << g_deliveredPackets << " size=" << packet->GetSize()
                                          << " at " << Simulator::Now().GetSeconds() << "s");
}

int
main(int argc, char* argv[])
{
  double simStop = 60.0;
  uint32_t packetSize = 32;
  uint32_t dataRate = 200;
  double spacing = 800.0;
  double txRange = 1000.0;

  CommandLine cmd;
  cmd.AddValue("simStop", "Simulation stop time in seconds", simStop);
  cmd.AddValue("packetSize", "Application packet size in bytes", packetSize);
  cmd.AddValue("dataRate", "OnOff data rate in bit/s", dataRate);
  cmd.AddValue("spacing", "Distance between adjacent nodes in meters", spacing);
  cmd.AddValue("txRange", "Acoustic transmission range in meters", txRange);
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
                      "RreqTimeout",
                      TimeValue(Seconds(1.0)),
                      "MaxRreqAttempts",
                      UintegerValue(3));

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

  PacketSocketAddress socket;
  socket.SetAllDevices();
  socket.SetPhysicalAddress(devices.Get(2)->GetAddress());
  socket.SetProtocol(0);

  OnOffHelper app("ns3::PacketSocketFactory", Address(socket));
  app.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  app.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  app.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
  app.SetAttribute("PacketSize", UintegerValue(packetSize));

  ApplicationContainer apps = app.Install(nodes.Get(0));
  apps.Start(Seconds(2.0));
  apps.Stop(Seconds(simStop - 1.0));

  TypeId psfid = TypeId::LookupByName("ns3::PacketSocketFactory");
  Ptr<Socket> sinkSocket = Socket::CreateSocket(nodes.Get(2), psfid);
  sinkSocket->Bind(socket);
  sinkSocket->SetRecvCallback(MakeCallback(&RecvPacket));

  Ptr<AquaSimNetDevice> sinkDevice = DynamicCast<AquaSimNetDevice>(devices.Get(2));
  sinkDevice->GetRouting()->TraceConnectWithoutContext("PacketReceived",
                                                       MakeCallback(&RoutingDelivered));

  Simulator::Stop(Seconds(simStop));
  Simulator::Run();
  Simulator::Destroy();

  std::cout << "UW-AODV line socket received packets: " << g_rxPackets << std::endl;
  std::cout << "UW-AODV line routing delivered packets: " << g_deliveredPackets << std::endl;
  asHelper.GetChannel()->PrintCounters();
  return g_deliveredPackets == 0 ? 1 : 0;
}
