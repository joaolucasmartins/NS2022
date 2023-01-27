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

#include <inet/common/TimeTag_m.h>
#include <inet/common/packet/chunk/BytesChunk.h>

#include <inet/networklayer/common/L3AddressTag_m.h>
#include <inet/transportlayer/common/L4PortTag_m.h>

#include <apps/mec/DeviceApp/DeviceAppMessages/DeviceAppPacket_m.h>
#include <apps/mec/DeviceApp/DeviceAppMessages/DeviceAppPacket_Types.h>

#include "../common/ClientResponsePacket.h"
#include "../common/ClientPacket.h"
#include "TumClientApp5G.h"

#include <iostream>
#include <fstream>

using namespace inet;
using namespace std;

simsignal_t TumClientApp5G::connectSignal = registerSignal("connect");

Define_Module(TumClientApp5G);

TumClientApp5G::TumClientApp5G()
{
    selfStart_ = NULL;
    selfStop_ = NULL;
    selfSend_ = NULL;
}

TumClientApp5G::~TumClientApp5G()
{
    cancelAndDelete(selfStart_);
    cancelAndDelete(selfStop_);
    cancelAndDelete(selfSend_);
}

void TumClientApp5G::initialize(int stage)
{
    EV << "TumClientApp5G::initialize - stage " << stage << endl;
    cSimpleModule::initialize(stage);
    // avoid multiple initializations
    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;

    // Statistics
    timeToResponseVec.setName("timeToResponse");

    // App Params
    numRequestsToSend = par("numRequests");
    const string tracksToRequestParameter = par("tracksToRequest");
    istringstream ss{tracksToRequestParameter};
    int track_no;
    this->tracksToRequest = vector<int>();
    while (ss >> track_no)
    {
        tracksToRequest.push_back(track_no);
    }

    log = par("logger").boolValue();

    // retrieve parameters
    size_ = par("packetSize");
    deviceLocalPort_ = par("deviceLocalPort");
    deviceAppPort_ = par("deviceAppPort");
    sourceSimbolicAddress = (char *)getParentModule()->getFullName();
    deviceSimbolicAppAddress_ = (char *)par("deviceAppAddress").stringValue();
    deviceAppAddress_ = inet::L3AddressResolver().resolve(deviceSimbolicAppAddress_);

    // binding socket
    socket.setOutputGate(gate("socketOut"));
    socket.bind(deviceLocalPort_);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);

    ue = this->getParentModule();
    mecAppName = par("mecAppName").stringValue();

    // initializing the auto-scheduling messages
    selfStart_ = new cMessage("selfStart");
    selfStop_ = new cMessage("selfStop");
    selfSend_ = new cMessage("selfSend");

    // starting TumClientApp5G
    simtime_t startTime = par("startTime");
    EV << "TumClientApp5G::initialize - starting sendStartMETumClientApp() in " << startTime << " seconds " << endl;
    scheduleAt(simTime() + startTime, selfStart_);

    // TCP config
    appSocket.setCallback(this);
    appSocket.setOutputGate(gate("socketOut"));

    // testing
    EV << "TumClientApp5G::initialize - sourceAddress: " << sourceSimbolicAddress << " [" << inet::L3AddressResolver().resolve(sourceSimbolicAddress).str() << "]" << endl;
    EV << "TumClientApp5G::initialize - destAddress: " << deviceSimbolicAppAddress_ << " [" << deviceAppAddress_.str() << "]" << endl;
    EV << "TumClientApp5G::initialize - binding app to port: local:" << deviceLocalPort_ << " , dest:" << deviceAppPort_ << endl;
}


void TumClientApp5G::handleMessage(cMessage *msg)
{
    EV << "TumClientApp5G::handleMessage" << endl;

    if (msg->isSelfMessage())
        handleSelfMessage(msg);
    else if (appSocket.belongsToSocket(msg))
        handleTcpMessage(msg);
    else if (socket.belongsToSocket(msg))
        handleUdpMessage(msg);
}


void TumClientApp5G::handleSelfMessage(cMessage *msg) {
    if (!strcmp(msg->getName(), "selfStart"))
        sendStartMETumClientApp();
    else if (!strcmp(msg->getName(), "selfStop"))
        sendStopMETumClientApp();
    else if (!strcmp(msg->getName(), "selfSend"))
        sendRequest();
    else
        throw cRuntimeError("TumClientApp5G::handleMessage - \tWARNING: Unrecognized self message");
}


void TumClientApp5G::handleUdpMessage(cMessage *msg) {
    inet::Packet *packet = check_and_cast<inet::Packet *>(msg);

    inet::L3Address ipAdd = packet->getTag<L3AddressInd>()->getSrcAddress();
    // int port = packet->getTag<L4PortInd>()->getSrcPort();

    /*
     * From Device app
     * device app usually runs in the UE (loopback), but it could also run in other places
     */
    if (ipAdd == deviceAppAddress_ || ipAdd == inet::L3Address("127.0.0.1")) // dev app
    {
        auto mePkt = packet->peekAtFront<DeviceAppPacket>();

        if (mePkt == 0)
            throw cRuntimeError("TumClientApp5G::handleMessage - \tFATAL! Error when casting to DeviceAppPacket");

        if (!strcmp(mePkt->getType(), ACK_START_MECAPP))
            handleAckStartMETumClientApp(msg);

        else if (!strcmp(mePkt->getType(), ACK_STOP_MECAPP))
            handleAckStopMETumClientApp(msg);

        else
        {
            throw cRuntimeError("TumClientApp5G::handleMessage - \tFATAL! Error, DeviceAppPacket type %s not recognized", mePkt->getType());
        }
    }

    delete msg;
}


void TumClientApp5G::handleTcpMessage(cMessage *msg) {
    appSocket.processMessage(msg);
}


// MEC Functions
/*
 * -----------------------------------------------Sender Side------------------------------------------
 */
void TumClientApp5G::sendStartMETumClientApp()
{
    inet::Packet *packet = new inet::Packet("TumClientPacketStart");
    auto start = inet::makeShared<DeviceAppStartPacket>();

    // instantiation requirements and info
    start->setType(START_MECAPP);
    start->setMecAppName(mecAppName.c_str());
    // start->setMecAppProvider("lte.apps.mec.TumClient_rest.METumClientApp_rest_External");

    start->setChunkLength(inet::B(2 + mecAppName.size() + 1));
    start->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());

    packet->insertAtBack(start);

    socket.sendTo(packet, deviceAppAddress_, deviceAppPort_);

    if (log)
    {
        ofstream myfile;
        myfile.open("example.txt", ios::app);
        if (myfile.is_open())
        {
            myfile << "[" << omnetpp::simTime() << "] TumClientApp5G - UE sent start message to the Device App \n";
            myfile.close();
        }
    }

}

void TumClientApp5G::sendStopMETumClientApp()
{
    return;

    EV << "TumClientApp5G::sendStopMETumClientApp - SENDING type TumClientPacket\n";

    inet::Packet *packet = new inet::Packet("DeviceAppStopPacket");
    auto stop = inet::makeShared<DeviceAppStopPacket>();

    // termination requirements and info
    stop->setType(STOP_MECAPP);

    stop->setChunkLength(inet::B(size_));
    stop->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());

    packet->insertAtBack(stop);
    socket.sendTo(packet, deviceAppAddress_, deviceAppPort_);

    if (log)
    {
        ofstream myfile;
        myfile.open("example.txt", ios::app);
        if (myfile.is_open())
        {
            myfile << "[" << omnetpp::simTime() << "] TumClientApp5G - UE sent stop message to the Device App \n";
            myfile.close();
        }
    }

    // rescheduling
    if (selfStop_->isScheduled())
        cancelEvent(selfStop_);
}

/*
 * ---------------------------------------------Receiver Side------------------------------------------
 */
void TumClientApp5G::handleAckStartMETumClientApp(cMessage *msg)
{
    inet::Packet *packet = check_and_cast<inet::Packet *>(msg);
    auto pkt = packet->peekAtFront<DeviceAppStartAckPacket>();

    if (pkt->getResult() == true)
    {
        mecAppAddress_ = L3AddressResolver().resolve(pkt->getIpAddress());
        mecAppPort_ = pkt->getPort();
        EV << "TumClientApp5G::handleAckStartMETumClientApp - Received " << pkt->getType() << " type TumClientPacket. mecApp isntance is at: " << mecAppAddress_ << ":" << mecAppPort_ << endl;
        cancelEvent(selfStart_);
        this->connect();
    }
    else
    {
        EV << "TumClientApp5G::handleAckStartMETumClientApp - MEC application cannot be instantiated! Reason: " << pkt->getReason() << endl;
    }

}

void TumClientApp5G::handleAckStopMETumClientApp(cMessage *msg)
{
    inet::Packet *packet = check_and_cast<inet::Packet *>(msg);
    auto pkt = packet->peekAtFront<DeviceAppStopAckPacket>();

    EV << "TumClientApp5G::handleAckStopMETumClientApp - Received " << pkt->getType() << " type TumClientPacket with result: " << pkt->getResult() << endl;
    if (pkt->getResult() == false)
        EV << "Reason: " << pkt->getReason() << endl;
    // updating runtime color of the car icon background
    ue->getDisplayString().setTagArg("i", 1, "white");

    cancelEvent(selfStop_);
}

//-------------------------- TCP Part -----------------------
void TumClientApp5G::connect()
{
    std::cout << "CONNECT" << std::endl;
    // we need a new connId if this is not the first connection
     appSocket.renewSocket();

    //const char *appLocalAddress = par("localAddress");
    //int applocalPort = par("applocalPort");
    //appSocket.bind(*appLocalAddress ? L3AddressResolver().resolve(appLocalAddress) : L3Address(), applocalPort);

    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        appSocket.setTimeToLive(timeToLive);

    int dscp = par("dscp");
    if (dscp != -1)
        appSocket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        appSocket.setTos(tos);

    // connect
    EV_INFO << "Connecting to mec host (" << mecAppAddress_ << ") port=" << mecAppPort_ << endl;

    appSocket.connect(mecAppAddress_, mecAppPort_);

    // TODO numSessions++;
    emit(connectSignal, 1L);
}

void TumClientApp5G::close()
{
    EV_INFO << "TumClientApp5G::close - issuing CLOSE command\n";
    std::cout << "CLOSING SOCKET" << std::endl;
    socket.close();
    emit(connectSignal, -1L);
}


void TumClientApp5G::sendRequest()
{
    const auto &payload = makeShared<ClientPacket>();
    this->timestampReq = simTime();
    std::cout << "Req " << numRequestsToSend << std::endl;
    Packet *packet = new Packet("data");
    payload->setTracks(this->tracksToRequest);
    payload->setChunkLength(B(1));
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    int numBytes = packet->getByteLength();
    emit(packetSentSignal, packet);

    // TODO packetsSent++;
    // TODO bytesSent += numBytes;

    if (log)
    {
        ofstream myfile;
        myfile.open("example.txt", ios::app);
        if (myfile.is_open())
        {
            myfile << "[" << omnetpp::simTime() << "] TumClientApp5G - UE sent start subscription message to the MEC application \n";
            myfile.close();
        }
    }

    appSocket.send(packet);
    EV_INFO << "TumClientApp5G::sendRequest - sending client request with " << this->tracksToRequest.size() << " tracks\n";
}


// ------------------- TCP Callbacks -------------------
void TumClientApp5G::socketEstablished(TcpSocket *)
{
    // *redefine* to perform or schedule first sending
    EV_INFO << "TumClientApp5G::socketEstablished - connected\n";

    numRequestsToSend = par("numRequests");
    if (numRequestsToSend < 1)
        numRequestsToSend = 1;

    std::cout << "Established: " << numRequestsToSend << std::endl;

    rescheduleAfter(0, selfSend_);
}

void TumClientApp5G::socketDataArrived(TcpSocket *, Packet *msg, bool)
{
    // *redefine* to perform or schedule next sending
//    packetsRcvd++;
//    bytesRcvd += msg->getByteLength();

    // TODO: SCHEDULE NEXT SEND

    emit(packetReceivedSignal, msg);

    timeToResponseStats.collect(omnetpp::simTime() - this->timestampReq);
    timeToResponseVec.record(omnetpp::simTime() - this->timestampReq);

    if (numRequestsToSend > 0) {
        EV_INFO << "TumClientApp5G::socketDataArrived - reply arrived\n";
        --numRequestsToSend;

         simtime_t d = par("thinkTime");
         rescheduleAfter(d, selfSend_);
    }
    else if (!selfSend_->isScheduled()) {
        simtime_t d = par("idleInterval");
        rescheduleAfter(d, selfSend_);
    }

//    if (appSocket.getState() != TcpSocket::LOCALLY_CLOSED) {
//        EV_INFO << "TumClientApp5G::socketDataArrived - reply to last request arrived\n";
//        close();
//        scheduleAfter(0, selfStop_);
//        cancelEvent(selfSend_);
//    }

    delete msg;
}

void TumClientApp5G::socketPeerClosed(TcpSocket *socket_)
{
    ASSERT(socket_ == &appSocket);
    // close the connection (if not already closed)
    if (appSocket.getState() == TcpSocket::PEER_CLOSED) {
        EV_INFO << "TumClientApp5G::socketPeerClosed - remote TCP closed, closing here as well\n";
        close();
    }
}

void TumClientApp5G::socketClosed(TcpSocket *)
{
    // *redefine* to start another session etc.
    EV_INFO << "TumClientApp5G::socketClosed - connection closed\n";

    std::cout << "Socket closed" << std::endl;

    // start another session after a delay
    simtime_t d = par("idleInterval");
    rescheduleAfter(d, selfStart_);
}

void TumClientApp5G::socketFailure(TcpSocket *, int code)
{
    // subclasses may override this function, and add code try to reconnect after a delay.
    EV_WARN << "TumClientApp5G::socketFailure - connection broken\n";
//    numBroken++;

    runtime_error("TumClientApp5G does not support TCP socket failures");
}












