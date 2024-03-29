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
#include <inet/applications/tcpapp/TcpAppBase.h>
#include <inet/common/lifecycle/ILifecycle.h>
#include <inet/common/lifecycle/NodeStatus.h>

using namespace omnetpp;
using namespace inet;
using namespace std;

class TumClientApp : public TcpAppBase {
  protected:
    cMessage *timeoutMsg = nullptr;
    bool earlySend = false; // if true, don't wait with sendRequest() until established()
    int numRequestsToSend = 0; // requests to send in this session (decreasing counter)
    int sessionRequestsToSend = 0; // requests to send in this session (static total for each session)
    vector<int> tracksToRequest;
    simtime_t timestampReq;

    simtime_t startTime, stopTime;
    simtime_t sessionThinkTime;

    cHistogram timeToResponseStats;
    cOutVector timeToResponseVec;

    cGate *mobilityGate = nullptr;

    virtual void sendRequest();
    virtual void rescheduleAfterOrDeleteTimer(simtime_t d, short int msgKind);

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleTimer(cMessage *msg) override;

    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void close() override;
    void finish() override;

  public:
    TumClientApp() {}
    virtual ~TumClientApp();
};

#endif
