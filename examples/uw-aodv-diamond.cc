/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/applications-module.h"
#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UwAodvDiamond");

static uint32_t g_deliveredPackets = 0;
static uint32_t g_rreqTx = 0;
static uint32_t g_rreqRx = 0;
static uint32_t g_rrepTx = 0;
static uint32_t g_rrepRx = 0;
static uint32_t g_forwardedData = 0;
static uint32_t g_noRouteDrops = 0;

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
CountForwardedData(uint32_t oldValue, uint32_t newValue)
{
  g_forwardedData += newValue - oldValue;
}

static void
CountNoRouteDrops(uint32_t oldValue, uint32_t newValue)
{
  g_noRouteDrops += newValue - oldValue;
}

int
main(int argc, char* argv[])
{
  double simStop = 60.0;
  uint32_t packetSize = 32;
  uint32_t dataRate = 200;
  double txRange = 1000.0;
  bool enableRreqCollection = true;
  double rreqTimeout = 3.0;
  double rrepWaitTime = 0.5;

  CommandLine cmd;
  cmd.AddValue("simStop", "Simulation stop time in seconds", simStop);
  cmd.AddValue("packetSize", "Application packet size in bytes", packetSize);
  cmd.AddValue("dataRate", "OnOff data rate in bit/s", dataRate);
  cmd.AddValue("txRange", "Acoustic transmission range in meters", txRange);
  cmd.AddValue("enableRreqCollection", "Enable destination-side RREQ candidate collection", enableRreqCollection);
  cmd.AddValue("rreqTimeout", "Route discovery retry timeout in seconds", rreqTimeout);
  cmd.AddValue("rrepWaitTime", "Destination-side RREQ collection window in seconds", rrepWaitTime);
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
                      "EnableRreqCollection",
                      BooleanValue(enableRreqCollection),
                      "RrepWaitTime",
                      TimeValue(Seconds(rrepWaitTime)));

  NetDeviceContainer devices;
  for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
      Ptr<AquaSimNetDevice> device = CreateObject<AquaSimNetDevice>();
      devices.Add(asHelper.Create(nodes.Get(i), device));
      device->GetPhy()->SetTransRange(txRange);
    }

  Ptr<ListPositionAllocator> positions = CreateObject<ListPositionAllocator>();
  positions->Add(Vector(0.0, 0.0, 0.0));       // source
  positions->Add(Vector(750.0, 250.0, 0.0));   // upper relay
  positions->Add(Vector(900.0, -350.0, 0.0));  // lower relay
  positions->Add(Vector(1600.0, 0.0, 0.0));    // destination

  MobilityHelper mobility;
  mobility.SetPositionAllocator(positions);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes);

  PacketSocketAddress socket;
  socket.SetAllDevices();
  socket.SetPhysicalAddress(devices.Get(3)->GetAddress());
  socket.SetProtocol(0);

  OnOffHelper app("ns3::PacketSocketFactory", Address(socket));
  app.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  app.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  app.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
  app.SetAttribute("PacketSize", UintegerValue(packetSize));

  ApplicationContainer apps = app.Install(nodes.Get(0));
  apps.Start(Seconds(2.0));
  apps.Stop(Seconds(simStop - 1.0));

  Ptr<AquaSimNetDevice> sinkDevice = DynamicCast<AquaSimNetDevice>(devices.Get(3));
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
      routing->TraceConnectWithoutContext("ForwardedData", MakeCallback(&CountForwardedData));
      routing->TraceConnectWithoutContext("NoRouteDrops", MakeCallback(&CountNoRouteDrops));
    }

  Simulator::Stop(Seconds(simStop));
  Simulator::Run();
  Simulator::Destroy();

  std::cout << "UW-AODV diamond routing delivered packets: " << g_deliveredPackets << std::endl;
  std::cout << "UW-AODV diamond counters:"
            << " RreqTx=" << g_rreqTx
            << " RreqRx=" << g_rreqRx
            << " RrepTx=" << g_rrepTx
            << " RrepRx=" << g_rrepRx
            << " ForwardedData=" << g_forwardedData
            << " NoRouteDrops=" << g_noRouteDrops
            << std::endl;
  asHelper.GetChannel()->PrintCounters();
  return g_deliveredPackets == 0 ? 1 : 0;
}
