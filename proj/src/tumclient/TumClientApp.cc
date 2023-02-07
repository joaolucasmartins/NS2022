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

#include <sstream>

#include <inet/applications/tcpapp/GenericAppMsg_m.h>
#include <inet/common/ModuleAccess.h>
#include <inet/common/TimeTag_m.h>
#include <inet/common/lifecycle/ModuleOperations.h>
#include <inet/common/packet/Packet.h>

#include "../common/ClientPacket.h"
#include "TumClientApp.h"

#include <iostream>

#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1
#define MSGKIND_PRECONNECT    2

Define_Module(TumClientApp);

TumClientApp::~TumClientApp()
{
    cancelAndDelete(timeoutMsg);
}

void TumClientApp::initialize(int stage)
{
    TcpAppBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        numRequestsToSend = 0;
        earlySend = false;
        WATCH(numRequestsToSend);
        WATCH(earlySend);

        const string tracksToRequestParameter = par("tracksToRequest");
        timeToResponseVec.setName("timeToResponse");

        istringstream ss{tracksToRequestParameter};
        int track_no;
        this->tracksToRequest = vector<int>();
        while (ss >> track_no) {
            tracksToRequest.push_back(track_no);
        }

        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        timeoutMsg = new cMessage("timer");

        if (hasGUI())
            getParentModule()->getDisplayString().setTagArg("i2", 0, "status/hourglass");

    } else if (stage == INITSTAGE_LAST) {
        cModule* mobilityModule = getParentModule()->getSubmodule("mobility");
        if (mobilityModule->hasGate("direct")) {
            mobilityGate = mobilityModule->gate("direct");
        } else {
            mobilityGate = nullptr;
        }
    }
}

void TumClientApp::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t now = simTime();
    simtime_t start = std::max(startTime, now);
    if (timeoutMsg && ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime))) {
        timeoutMsg->setKind(MSGKIND_PRECONNECT);
        scheduleAt(start, timeoutMsg);
    }

    std::cout << "[APP] Lifecycle start" << std::endl;
}

void TumClientApp::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (socket.getState() == TcpSocket::CONNECTED || socket.getState() == TcpSocket::CONNECTING || socket.getState() == TcpSocket::PEER_CLOSED)
        close();

    std::cout << "[APP] Lifecycle stop" << std::endl;
}

void TumClientApp::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (operation->getRootModule() != getContainingNode(this))
        socket.destroy();

    std::cout << "[APP] Lifecycle crash" << std::endl;
}

void TumClientApp::sendRequest()
{
    const auto& payload = makeShared<ClientPacket>();
    this->timestampReq = simTime();

    Packet *packet = new Packet("data");
    payload->setTracks(this->tracksToRequest);
    payload->setChunkLength(B(1));
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    EV_INFO << "sending client request with " << this->tracksToRequest.size() << " tracks\n";
    sendPacket(packet);

    if (hasGUI()) {
        std::string s = "Session: " + to_string(numRequestsToSend) + "/" + to_string(sessionRequestsToSend);
        bubble(s.c_str());
    }

    std::cout << "[APP] Send request" << std::endl;
}

void TumClientApp::handleTimer(cMessage *msg)
{
    switch (msg->getKind()) {
        case MSGKIND_PRECONNECT:
            std::cout << "[APP] Pre-connect" << std::endl;

            if (mobilityGate != nullptr)
                sendDirect(new cMessage("START"), mobilityGate);

            rescheduleAfterOrDeleteTimer(0, MSGKIND_CONNECT);

            break;


        case MSGKIND_CONNECT:
            std::cout << "[APP] Connect" << std::endl;

            if (hasGUI())
                getParentModule()->getDisplayString().setTagArg("i2", 0, "status/green");

            connect(); // active OPEN

            // significance of earlySend: if true, data will be sent already
            // in the ACK of SYN, otherwise only in a separate packet (but still
            // immediately)
            if (earlySend)
                sendRequest();
            break;

        case MSGKIND_SEND:
            sendRequest();
            numRequestsToSend--;
            // no scheduleAt(): next request will be sent when reply to this one
            // arrives (see socketDataArrived())
            break;

        default:
            throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}

void TumClientApp::socketEstablished(TcpSocket *socket)
{
    TcpAppBase::socketEstablished(socket);

    std::cout << "[APP] Socket established" << endl;

    // determine number of requests in this session
    numRequestsToSend = par("numRequestsPerSession");
    sessionRequestsToSend = numRequestsToSend;
    if (numRequestsToSend < 1)
        numRequestsToSend = 1;

    sessionThinkTime = par("thinkTime");

    // perform first request if not already done (next one will be sent when reply arrives)
    if (!earlySend)
        sendRequest();

    numRequestsToSend--;
}

void TumClientApp::rescheduleAfterOrDeleteTimer(simtime_t d, short int msgKind)
{
    if (stopTime < SIMTIME_ZERO || d < stopTime) {
        timeoutMsg->setKind(msgKind);
        rescheduleAfter(d, timeoutMsg);
    }
    else {
        cancelAndDelete(timeoutMsg);
        timeoutMsg = nullptr;
    }
}

void TumClientApp::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
{
    TcpAppBase::socketDataArrived(socket, msg, urgent);
    simtime_t now = simTime();

    timeToResponseStats.collect(now - this->timestampReq);
    timeToResponseVec.record(now - this->timestampReq);


    if (numRequestsToSend > 0) {
        EV_INFO << "reply arrived\n";

        if (timeoutMsg) {
            rescheduleAfterOrDeleteTimer(sessionThinkTime, MSGKIND_SEND);
        }
    }
    else if (socket->getState() != TcpSocket::LOCALLY_CLOSED) {
        EV_INFO << "reply to last request arrived, closing session\n";
        close();
    }
}

void TumClientApp::close()
{
    TcpAppBase::close();
    cancelEvent(timeoutMsg);
}

void TumClientApp::socketClosed(TcpSocket *socket)
{
    TcpAppBase::socketClosed(socket);

    std::cout << "[APP] Socket closed" << endl;

    if (mobilityGate != nullptr)
        sendDirect(new cMessage("END"), mobilityGate);

    if (hasGUI())
        getParentModule()->getDisplayString().setTagArg("i2", 0, "status/hourglass");

    // start another session after a delay
    if (timeoutMsg) {
        simtime_t d = par("idleInterval");

        rescheduleAfterOrDeleteTimer(d, MSGKIND_PRECONNECT);
    } else {
        cRuntimeError("TumClientApp: timeoutMsg must always be defined");
    }
}

void TumClientApp::socketFailure(TcpSocket *socket, int code)
{
    TcpAppBase::socketFailure(socket, code);

    // reconnect after a delay
    if (timeoutMsg) {
        simtime_t d = par("reconnectInterval");
        rescheduleAfterOrDeleteTimer(d, MSGKIND_PRECONNECT);
    }
}

void TumClientApp::finish()
{
    timeToResponseStats.recordAs("timeToResponse");
}



