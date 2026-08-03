/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"

#include <cmath>
#include <iostream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UwTargetBaseline");

namespace
{

uint32_t g_deliveredPackets = 0;
uint32_t g_rreqTx = 0;
uint32_t g_rreqRx = 0;
uint32_t g_rrepTx = 0;
uint32_t g_rrepRx = 0;
uint32_t g_forwardedData = 0;
uint32_t g_deliveredData = 0;
uint32_t g_noRouteDrops = 0;
uint32_t g_queueDrops = 0;
uint32_t g_duplicateDataDrops = 0;
uint32_t g_destinationIndex = 0;

void
CountDelivery(Ptr<const Packet> packet)
{
  (void)packet;
  ++g_deliveredPackets;
  std::cout << "UW target baseline: destination node " << g_destinationIndex
            << " received data at "
            << Simulator::Now().GetSeconds() << " s" << std::endl;
}

void
SendOnePacket(Ptr<Socket> socket, uint32_t packetSize)
{
  socket->Send(Create<Packet>(packetSize));
}

void
CountRreqTx(uint32_t oldValue, uint32_t newValue)
{
  g_rreqTx += newValue - oldValue;
}

void
CountRreqRx(uint32_t oldValue, uint32_t newValue)
{
  g_rreqRx += newValue - oldValue;
}

void
CountRrepTx(uint32_t oldValue, uint32_t newValue)
{
  g_rrepTx += newValue - oldValue;
}

void
CountRrepRx(uint32_t oldValue, uint32_t newValue)
{
  g_rrepRx += newValue - oldValue;
}

void
CountForwardedData(uint32_t oldValue, uint32_t newValue)
{
  g_forwardedData += newValue - oldValue;
}

void
CountDeliveredData(uint32_t oldValue, uint32_t newValue)
{
  g_deliveredData += newValue - oldValue;
}

void
CountNoRouteDrops(uint32_t oldValue, uint32_t newValue)
{
  g_noRouteDrops += newValue - oldValue;
}

void
CountQueueDrops(uint32_t oldValue, uint32_t newValue)
{
  g_queueDrops += newValue - oldValue;
}

void
CountDuplicateDataDrops(uint32_t oldValue, uint32_t newValue)
{
  g_duplicateDataDrops += newValue - oldValue;
}

} // namespace

int
main(int argc, char* argv[])
{
  const uint32_t nodeCount = 6;
  const uint32_t centerIndex = 0;
  uint32_t sourceIndex = 1;
  std::string protocol = "aodv";
  std::string traffic = "downlink";
  double radius = 150.0;
  double txRange = 200.0;
  double propagationSpeed = 0.0;
  double auvSpeed = 1.0;
  double formationHeadingDeg = 0.0;
  double txPower = 20.0;
  double slotDuration = 10.0;
  double guardTime = 0.01;
  double firstPacketTime = 1.0;
  double simStop = 180.0;
  double maxQueueTime = 120.0;
  uint32_t packetSize = 32;
  double vbfWidth = 100.0;

  CommandLine cmd;
  cmd.AddValue("protocol", "Baseline routing protocol: aodv or vbf", protocol);
  cmd.AddValue("traffic", "Traffic direction: downlink (main AUV to peripheral) or uplink", traffic);
  cmd.AddValue("radius", "Radius R of the five peripheral nodes in meters", radius);
  cmd.AddValue("sourceIndex", "Peripheral node index used as source for uplink or destination for downlink (1 through 5)", sourceIndex);
  cmd.AddValue("txRange", "PHY transmission range in meters", txRange);
  cmd.AddValue("propagationSpeed", "Propagation speed override in m/s; zero uses Aqua-Sim acoustic speed", propagationSpeed);
  cmd.AddValue("auvSpeed", "Main AUV speed in m/s, recorded for the target scenario; a trajectory is configured separately", auvSpeed);
  cmd.AddValue("formationHeadingDeg", "Common heading of the translating AUV formation in degrees; zero is +X", formationHeadingDeg);
  cmd.AddValue("txPower", "AquaSim PHY transmitted signal power PT in W", txPower);
  cmd.AddValue("slotDuration", "TDMA slot duration in seconds", slotDuration);
  cmd.AddValue("guardTime", "TDMA guard interval in seconds", guardTime);
  cmd.AddValue("firstPacketTime", "Time at which the peripheral source sends one packet", firstPacketTime);
  cmd.AddValue("simStop", "Simulation stop time in seconds", simStop);
  cmd.AddValue("maxQueueTime", "Maximum AODV pending-data queue time in seconds", maxQueueTime);
  cmd.AddValue("packetSize", "Application payload size in bytes", packetSize);
  cmd.AddValue("vbfWidth", "VBF virtual-pipe width in meters", vbfWidth);
  cmd.Parse(argc, argv);

  if ((protocol != "aodv" && protocol != "vbf") ||
      (traffic != "downlink" && traffic != "uplink") || sourceIndex == centerIndex ||
      sourceIndex >= nodeCount || radius <= 0.0 || txRange <= 0.0 ||
      propagationSpeed < 0.0 || auvSpeed < 0.0 || txPower <= 0.0 || slotDuration <= 0.0 ||
      guardTime < 0.0 || firstPacketTime < 0.0 || simStop <= firstPacketTime ||
      maxQueueTime <= 0.0)
    {
      std::cerr << "Invalid target-baseline configuration" << std::endl;
      return 1;
    }

  const uint32_t sourceNodeIndex = traffic == "downlink" ? centerIndex : sourceIndex;
  const uint32_t destinationNodeIndex = traffic == "downlink" ? sourceIndex : centerIndex;
  constexpr double kPi = 3.14159265358979323846;
  Vector vbfTargetPosition(0.0, 0.0, 0.0);
  if (destinationNodeIndex != centerIndex)
    {
      const double angle = 2.0 * kPi * static_cast<double>(destinationNodeIndex - 1) /
                           static_cast<double>(nodeCount - 1);
      vbfTargetPosition = Vector(radius * std::cos(angle), radius * std::sin(angle), 0.0);
    }

  NodeContainer nodes;
  nodes.Create(nodeCount);

  PacketSocketHelper socketHelper;
  socketHelper.Install(nodes);

  AquaSimChannelHelper channel = AquaSimChannelHelper::Default();
  channel.SetPropagation("ns3::AquaSimRangePropagation",
                         "PropagationSpeed",
                         DoubleValue(propagationSpeed));

  AquaSimHelper asHelper = AquaSimHelper::Default();
  asHelper.SetChannel(channel.Create());
  asHelper.SetPhy("ns3::AquaSimPhyCmn", "PT", DoubleValue(txPower));
  asHelper.SetMac("ns3::AquaSimTdmaMac",
                  "TdmaSlotPeriod",
                  UintegerValue(nodeCount),
                  "TdmaSlotDuration",
                  TimeValue(Seconds(slotDuration)),
                  "TdmaGuardTime",
                  TimeValue(Seconds(guardTime)));

  if (protocol == "aodv")
    {
      // With six 10-second TDMA slots, one route discovery may span a full
      // 60-second frame even though acoustic propagation is fast.
      asHelper.SetRouting("ns3::AquaSimUWAodvRouting",
                          "RreqTimeout",
                          TimeValue(Seconds(180.0)),
                          "NodeTraversalTime",
                          TimeValue(Seconds(30.0)),
                          "ActiveRouteTimeout",
                          TimeValue(Seconds(300.0)),
                          "MyRouteTimeout",
                          TimeValue(Seconds(300.0)),
                          "MaxQueueTime",
                          TimeValue(Seconds(maxQueueTime)),
                          "MaxRreqAttempts",
                          UintegerValue(2),
                          "RreqJitter",
                          TimeValue(Seconds(0.0)));
    }
  else
    {
      asHelper.SetRouting("ns3::AquaSimVBF",
                          "Width",
                          DoubleValue(vbfWidth),
                          "TargetPos",
                          Vector3DValue(vbfTargetPosition));
    }

  NetDeviceContainer devices;
  for (uint32_t i = 0; i < nodeCount; ++i)
    {
      asHelper.SetMacAttribute("TdmaSlotNumber", UintegerValue(i));
      Ptr<AquaSimNetDevice> device = CreateObject<AquaSimNetDevice>();
      devices.Add(asHelper.Create(nodes.Get(i), device));
      device->GetPhy()->SetTransRange(txRange);
    }

  Ptr<ListPositionAllocator> positions = CreateObject<ListPositionAllocator>();
  positions->Add(Vector(0.0, 0.0, 0.0));
  for (uint32_t k = 0; k < nodeCount - 1; ++k)
    {
      const double angle = 2.0 * kPi * static_cast<double>(k) / static_cast<double>(nodeCount - 1);
      positions->Add(Vector(radius * std::cos(angle), radius * std::sin(angle), 0.0));
    }

  MobilityHelper mobility;
  mobility.SetPositionAllocator(positions);
  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(nodes);

  const double headingRad = formationHeadingDeg * kPi / 180.0;
  const Vector formationVelocity(auvSpeed * std::cos(headingRad),
                                 auvSpeed * std::sin(headingRad),
                                 0.0);
  for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
      Ptr<ConstantVelocityMobilityModel> model =
        nodes.Get(i)->GetObject<ConstantVelocityMobilityModel>();
      model->SetVelocity(formationVelocity);
    }

  PacketSocketAddress destination;
  destination.SetAllDevices();
  destination.SetPhysicalAddress(devices.Get(destinationNodeIndex)->GetAddress());
  destination.SetProtocol(0);

  TypeId psfid = TypeId::LookupByName("ns3::PacketSocketFactory");
  Ptr<Socket> sourceSocket = Socket::CreateSocket(nodes.Get(sourceNodeIndex), psfid);
  sourceSocket->Bind();
  sourceSocket->Connect(destination);
  Simulator::Schedule(Seconds(firstPacketTime), &SendOnePacket, sourceSocket, packetSize);

  g_destinationIndex = destinationNodeIndex;
  Ptr<AquaSimNetDevice> destinationDevice =
    DynamicCast<AquaSimNetDevice>(devices.Get(destinationNodeIndex));
  destinationDevice->GetRouting()->TraceConnectWithoutContext("PacketReceived",
                                                               MakeCallback(&CountDelivery));

  if (protocol == "aodv")
    {
      for (uint32_t i = 0; i < devices.GetN(); ++i)
        {
          Ptr<AquaSimNetDevice> device = DynamicCast<AquaSimNetDevice>(devices.Get(i));
          Ptr<AquaSimRouting> routing = device->GetRouting();
          routing->TraceConnectWithoutContext("RreqTx", MakeCallback(&CountRreqTx));
          routing->TraceConnectWithoutContext("RreqRx", MakeCallback(&CountRreqRx));
          routing->TraceConnectWithoutContext("RrepTx", MakeCallback(&CountRrepTx));
          routing->TraceConnectWithoutContext("RrepRx", MakeCallback(&CountRrepRx));
          routing->TraceConnectWithoutContext("ForwardedData", MakeCallback(&CountForwardedData));
          routing->TraceConnectWithoutContext("DeliveredData", MakeCallback(&CountDeliveredData));
          routing->TraceConnectWithoutContext("NoRouteDrops", MakeCallback(&CountNoRouteDrops));
          routing->TraceConnectWithoutContext("QueueDrops", MakeCallback(&CountQueueDrops));
          routing->TraceConnectWithoutContext("DuplicateDataDrops",
                                              MakeCallback(&CountDuplicateDataDrops));
        }
    }

  std::cout << "UW target baseline configuration: protocol=" << protocol
            << " nodes=6 center=(0,0,0) R=" << radius
            << " txRange=" << txRange
            << " propagationSpeed="
            << (propagationSpeed == 0.0 ? "acoustic-model" : std::to_string(propagationSpeed) + " m/s")
            << " formationVelocity=(" << formationVelocity.x << "," << formationVelocity.y << ",0) m/s"
            << " PT=" << txPower << " W"
            << " TDMA=6x" << slotDuration << "s + " << guardTime << "s guard"
            << " AODV queue=" << maxQueueTime << "s"
            << " traffic=" << traffic
            << " source=" << sourceNodeIndex
            << " destination=" << destinationNodeIndex << std::endl;

  Simulator::Stop(Seconds(simStop));
  Simulator::Run();
  Simulator::Destroy();

  std::cout << "UW target baseline result: protocol=" << protocol
            << " deliveredPackets=" << g_deliveredPackets
            << " (delivery is reported, not used as an assertion in this exploratory baseline)"
            << std::endl;
  if (protocol == "aodv")
    {
      std::cout << "UW target baseline AODV counters:"
                << " RreqTx=" << g_rreqTx
                << " RreqRx=" << g_rreqRx
                << " RrepTx=" << g_rrepTx
                << " RrepRx=" << g_rrepRx
                << " ForwardedData=" << g_forwardedData
                << " DeliveredData=" << g_deliveredData
                << " NoRouteDrops=" << g_noRouteDrops
                << " QueueDrops=" << g_queueDrops
                << " DuplicateDataDrops=" << g_duplicateDataDrops
                << std::endl;
    }
  asHelper.GetChannel()->PrintCounters();
  return 0;
}
