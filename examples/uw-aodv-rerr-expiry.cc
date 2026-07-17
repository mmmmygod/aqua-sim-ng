/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/applications-module.h"
#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UwAodvRerrExpiry");

static uint32_t g_deliveredPackets = 0;
static uint32_t g_rreqTx = 0;
static uint32_t g_rreqRx = 0;
static uint32_t g_rrepTx = 0;
static uint32_t g_rrepRx = 0;
static uint32_t g_rerrTx = 0;
static uint32_t g_rerrRx = 0;
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
CountRerrTx(uint32_t oldValue, uint32_t newValue)
{
  g_rerrTx += newValue - oldValue;
}

static void
CountRerrRx(uint32_t oldValue, uint32_t newValue)
{
  g_rerrRx += newValue - oldValue;
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
  double simStop = 35.0;
  uint32_t packetSize = 32;
  uint32_t dataRate = 200;
  double spacing = 800.0;
  double txRange = 1000.0;
  double routeLifetime = 4.0;
  double rreqTimeout = 1.0;

  CommandLine cmd;
  cmd.AddValue("simStop", "Simulation stop time in seconds", simStop);
  cmd.AddValue("packetSize", "Application packet size in bytes", packetSize);
  cmd.AddValue("dataRate", "OnOff data rate in bit/s", dataRate);
  cmd.AddValue("spacing", "Distance between adjacent nodes in meters", spacing);
  cmd.AddValue("txRange", "Acoustic transmission range in meters", txRange);
  cmd.AddValue("routeLifetime", "Short route lifetime used to force route expiry in seconds", routeLifetime);
  cmd.AddValue("rreqTimeout", "Route discovery retry timeout in seconds", rreqTimeout);
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
                      TimeValue(Seconds(rreqTimeout)),
                      "RouteLifetime",
                      TimeValue(Seconds(routeLifetime)),
                      "MaxRreqAttempts",
                      UintegerValue(4));

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
      routing->TraceConnectWithoutContext("RerrTx", MakeCallback(&CountRerrTx));
      routing->TraceConnectWithoutContext("RerrRx", MakeCallback(&CountRerrRx));
      routing->TraceConnectWithoutContext("ForwardedData", MakeCallback(&CountForwardedData));
      routing->TraceConnectWithoutContext("NoRouteDrops", MakeCallback(&CountNoRouteDrops));
    }

  Simulator::Stop(Seconds(simStop));
  Simulator::Run();
  Simulator::Destroy();

  std::cout << "UW-AODV RERR expiry routing delivered packets: " << g_deliveredPackets << std::endl;
  std::cout << "UW-AODV RERR expiry counters:"
            << " RreqTx=" << g_rreqTx
            << " RreqRx=" << g_rreqRx
            << " RrepTx=" << g_rrepTx
            << " RrepRx=" << g_rrepRx
            << " RerrTx=" << g_rerrTx
            << " RerrRx=" << g_rerrRx
            << " ForwardedData=" << g_forwardedData
            << " NoRouteDrops=" << g_noRouteDrops
            << std::endl;
  asHelper.GetChannel()->PrintCounters();

  return (g_deliveredPackets > 0 && g_rerrTx > 0 && g_rerrRx > 0 && g_rreqTx > 2) ? 0 : 1;
}
