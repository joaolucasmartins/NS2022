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

#ifndef __TRAINULTRAPRECISEMONITORING_TUMTRAINSERVERAPP_H_
#define __TRAINULTRAPRECISEMONITORING_TUMTRAINSERVERAPP_H_

#include <omnetpp.h>

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

#include "../TrainManager.h"

using namespace omnetpp;
using namespace inet;

/**
 * Consumes and prints packets received from the Udp module. See NED for more info.
 */
class TumTrainServerApp : public ApplicationBase, public UdpSocket::ICallback
{
protected:
    enum SelfMsgKinds { START = 1, STOP };

    UdpSocket socket;
    int localPort = -1;
    L3Address multicastGroup;
    simtime_t startTime;
    simtime_t stopTime;
    cMessage *selfMsg = nullptr;
    int numReceived = 0;

  public:
    TumTrainServerApp() {}
    virtual ~TumTrainServerApp();

  protected:
    virtual void processPacket(Packet *msg);
    virtual void setSocketOptions();

  protected:
    TrainManager* getTrainManager();

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

    virtual void processStart();
    virtual void processStop();

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
};

#endif
