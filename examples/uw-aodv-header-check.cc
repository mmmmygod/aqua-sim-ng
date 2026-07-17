/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/aqua-sim-ng-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"

#include <iostream>

using namespace ns3;

namespace
{

bool
Check(bool condition, const std::string& message)
{
  if (!condition)
    {
      std::cerr << "UW-AODV header check failed: " << message << std::endl;
      return false;
    }
  return true;
}

} // namespace

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AquaSimUWAodvHeader tx;
  tx.SetType(AquaSimUWAodvHeader::UWAODV_RREQ);
  tx.SetHopCount(7);
  tx.SetRequestId(12345);
  tx.SetOrigin(AquaSimAddress(1));
  tx.SetDestination(AquaSimAddress(3));
  tx.SetOriginSeqNo(100);
  tx.SetDestSeqNo(88);
  tx.SetLifetime(120000);
  tx.SetHopLimit(9);
  tx.SetUnknownDestSeqNo(true);

  Ptr<Packet> packet = Create<Packet>();
  packet->AddHeader(tx);

  AquaSimUWAodvHeader rx;
  packet->RemoveHeader(rx);

  bool ok = true;
  ok &= Check(tx.GetSerializedSize() == 26, "serialized size should include sequence and hop-limit fields");
  ok &= Check(rx.GetType() == AquaSimUWAodvHeader::UWAODV_RREQ, "type mismatch");
  ok &= Check(rx.GetHopCount() == 7, "hop count mismatch");
  ok &= Check(rx.GetRequestId() == 12345, "request id mismatch");
  ok &= Check(rx.GetOrigin() == AquaSimAddress(1), "origin mismatch");
  ok &= Check(rx.GetDestination() == AquaSimAddress(3), "destination mismatch");
  ok &= Check(rx.GetOriginSeqNo() == 100, "origin sequence number mismatch");
  ok &= Check(rx.GetDestSeqNo() == 88, "destination sequence number mismatch");
  ok &= Check(rx.GetLifetime() == 120000, "lifetime mismatch");
  ok &= Check(rx.GetHopLimit() == 9, "hop limit mismatch");
  ok &= Check(rx.IsUnknownDestSeqNo(), "unknown destination sequence flag mismatch");

  std::cout << "UW-AODV header serialization check: " << (ok ? "PASS" : "FAIL") << std::endl;
  return ok ? 0 : 1;
}
