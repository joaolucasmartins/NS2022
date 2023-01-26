//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __TRAINULTRAPRECISEMONITORING_TUMCLIENT_H_
#define __TRAINULTRAPRECISEMONITORING_TUMCLIENT_H_

#include <string>
#include <vector>
#include <omnetpp.h>

#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"

using namespace omnetpp;

class TumClientApp5G : public cSimpleModule
{
  // communication to device app and mec app
  inet::UdpSocket socket;
  inet::TcpSocket appSocket;

  int size_;
  simtime_t period_;
  int appLocalPort_;
  int deviceLocalPort_;
  int deviceAppPort_;
  inet::L3Address deviceAppAddress_;

  char *sourceSimbolicAddress;     // Ue[x]
  char *deviceSimbolicAppAddress_; // meHost.virtualisationInfrastructure

  // MEC application endPoint (returned by the device app)
  inet::L3Address mecAppAddress_;
  int mecAppPort_;

  cModule* ue;
  std::string mecAppName;

  // scheduling
  cMessage *selfStart_;
  cMessage *selfStop_;
  cMessage *selfMecAppStart_;

  // uses to write in a log a file
  bool log;

  cHistogram timeToResponseStats;
  cOutVector timeToResponseVec;
  simtime_t timestampReq;

  int numRequestsToSend = 0; // requests to send in this session

  static simsignal_t connectSignal;
public:
  ~TumClientApp5G();
  TumClientApp5G();

protected:
  void sendRequest();
  void receiveResponse();

  /* TCP Utility functions */
  virtual void connect();
  virtual void close();
  virtual void sendPacket(Packet *pkt);

  virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
  void initialize(int stage);
  virtual void handleMessage(cMessage *msg);
  virtual void finish();

  void sendStartMEWarningAlertApp();
  void sendStopMEWarningAlertApp();

  void handleAckStartMEWarningAlertApp(cMessage *msg);
  void handleInfoMEWarningAlertApp(cMessage *msg);
  void handleAckStopMEWarningAlertApp(cMessage *msg);

  vector<int> tracksToRequest;
};

#endif
