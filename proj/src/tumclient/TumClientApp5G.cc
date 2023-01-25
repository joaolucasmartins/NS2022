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

#include <fstream>

using namespace inet;
using namespace std;

Define_Module(TumClientApp5G);

TumClientApp5G::TumClientApp5G()
{
    selfStart_ = NULL;
    selfStop_ = NULL;
}

TumClientApp5G::~TumClientApp5G()
{
    cancelAndDelete(selfStart_);
    cancelAndDelete(selfStop_);
    cancelAndDelete(selfMecAppStart_);
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
    numRequestsToSend = par("numRequestsPerSession");
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
    period_ = par("period");
    localPort_ = par("localPort");
    deviceAppPort_ = par("deviceAppPort");
    sourceSimbolicAddress = (char *)getParentModule()->getFullName();
    deviceSimbolicAppAddress_ = (char *)par("deviceAppAddress").stringValue();
    deviceAppAddress_ = inet::L3AddressResolver().resolve(deviceSimbolicAppAddress_);

    // binding socket
    socket.setOutputGate(gate("socketOut"));
    socket.bind(localPort_);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);

    ue = this->getParentModule();
    mecAppName = par("mecAppName").stringValue();

    // initializing the auto-scheduling messages
    selfStart_ = new cMessage("selfStart");
    selfStop_ = new cMessage("selfStop");
    selfMecAppStart_ = new cMessage("selfMecAppStart");

    // starting TumClientApp5G
    simtime_t startTime = par("startTime");
    EV << "TumClientApp5G::initialize - starting sendStartMEWarningAlertApp() in " << startTime << " seconds " << endl;
    scheduleAt(simTime() + startTime, selfStart_);

    // testing
    EV << "TumClientApp5G::initialize - sourceAddress: " << sourceSimbolicAddress << " [" << inet::L3AddressResolver().resolve(sourceSimbolicAddress).str() << "]" << endl;
    EV << "TumClientApp5G::initialize - destAddress: " << deviceSimbolicAppAddress_ << " [" << deviceAppAddress_.str() << "]" << endl;
    EV << "TumClientApp5G::initialize - binding to port: local:" << localPort_ << " , dest:" << deviceAppPort_ << endl;
}

// Application Functions

void TumClientApp5G::sendRequest()
{
    const auto &payload = makeShared<ClientPacket>();
    this->timestampReq = simTime();
    std::cout << "Req" << std::endl;
    Packet *packet = new Packet("data");
    payload->setTracks(this->tracksToRequest);
    payload->setChunkLength(B(1));
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

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

    socket.sendTo(packet, mecAppAddress_, mecAppPort_);
    EV_INFO << "sending client request with " << this->tracksToRequest.size() << " tracks\n";
}

void TumClientApp5G::receiveResponse()
{
    timeToResponseStats.collect(omnetpp::simTime() - this->timestampReq);
    timeToResponseVec.record(omnetpp::simTime() - this->timestampReq);

    if (numRequestsToSend > 0)
    {
        EV_INFO << "reply arrived\n";
        --numRequestsToSend;

        // if (timeoutMsg)
        //{
        // simtime_t d = par("thinkTime");
        // rescheduleAfterOrDeleteTimer(d, MSGKIND_SEND);
        //}
    }
    else
    {
        EV_INFO << "reply to last request arrived, closing\n";
    }
}

// MEC Functions

void TumClientApp5G::handleMessage(cMessage *msg)
{
    EV << "TumClientApp5G::handleMessage" << endl;
    // Sender Side
    if (msg->isSelfMessage())
    {
        if (!strcmp(msg->getName(), "selfStart"))
            sendStartMEWarningAlertApp();

        else if (!strcmp(msg->getName(), "selfStop"))
            sendStopMEWarningAlertApp();

        else if (!strcmp(msg->getName(), "selfMecAppStart"))
        {
            sendRequest();
            scheduleAt(simTime() + period_, selfMecAppStart_);
        }

        else
            throw cRuntimeError("TumClientApp5G::handleMessage - \tWARNING: Unrecognized self message");
    }
    // Receiver Side
    else
    {
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
                handleAckStartMEWarningAlertApp(msg);

            else if (!strcmp(mePkt->getType(), ACK_STOP_MECAPP))
                handleAckStopMEWarningAlertApp(msg);

            else
            {
                throw cRuntimeError("TumClientApp5G::handleMessage - \tFATAL! Error, DeviceAppPacket type %s not recognized", mePkt->getType());
            }
        }
        // From MEC application
        else
        {

            auto mePkt = packet->peekAtFront<ClientResponsePacket>();
            if (mePkt == 0)
                throw cRuntimeError("TumClientApp5G::handleMessage - \tFATAL! Error when casting to ClientResponsePacket");

            receiveResponse();

            // auto mePkt = packet->peekAtFront<WarningAppPacket>();
            // if (mePkt == 0)
            // throw cRuntimeError("TumClientApp5G::handleMessage - \tFATAL! Error when casting to WarningAppPacket");

            // if (!strcmp(mePkt->getType(), WARNING_ALERT))
            // handleInfoMEWarningAlertApp(msg);
            // else if (!strcmp(mePkt->getType(), START_NACK))
            //{
            // EV << "TumClientApp5G::handleMessage - MEC app did not started correctly, trying to start again" << endl;
            //}
            // else if (!strcmp(mePkt->getType(), START_ACK))
            //{
            // EV << "TumClientApp5G::handleMessage - MEC app started correctly" << endl;
            // if (selfMecAppStart_->isScheduled())
            //{
            // cancelEvent(selfMecAppStart_);
            //}
            //}
            // else
            //{
            // throw cRuntimeError("TumClientApp5G::handleMessage - \tFATAL! Error, WarningAppPacket type %s not recognized", mePkt->getType());
            //}
        }
        delete msg;
    }
}

void TumClientApp5G::finish()
{
}
/*
 * -----------------------------------------------Sender Side------------------------------------------
 */
void TumClientApp5G::sendStartMEWarningAlertApp()
{
    inet::Packet *packet = new inet::Packet("WarningAlertPacketStart");
    auto start = inet::makeShared<DeviceAppStartPacket>();

    // instantiation requirements and info
    start->setType(START_MECAPP);
    start->setMecAppName(mecAppName.c_str());
    // start->setMecAppProvider("lte.apps.mec.warningAlert_rest.MEWarningAlertApp_rest_External");

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

    // rescheduling
    scheduleAt(simTime() + period_, selfStart_);
}

void TumClientApp5G::sendStopMEWarningAlertApp()
{
    EV << "TumClientApp5G::sendStopMEWarningAlertApp - SENDING type WarningAlertPacket\n";

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
    scheduleAt(simTime() + period_, selfStop_);
}

/*
 * ---------------------------------------------Receiver Side------------------------------------------
 */
void TumClientApp5G::handleAckStartMEWarningAlertApp(cMessage *msg)
{
    inet::Packet *packet = check_and_cast<inet::Packet *>(msg);
    auto pkt = packet->peekAtFront<DeviceAppStartAckPacket>();

    if (pkt->getResult() == true)
    {
        mecAppAddress_ = L3AddressResolver().resolve(pkt->getIpAddress());
        mecAppPort_ = pkt->getPort();
        EV << "TumClientApp5G::handleAckStartMEWarningAlertApp - Received " << pkt->getType() << " type WarningAlertPacket. mecApp isntance is at: " << mecAppAddress_ << ":" << mecAppPort_ << endl;
        cancelEvent(selfStart_);
        // scheduling sendStopMEWarningAlertApp()
        if (!selfStop_->isScheduled())
        {
            simtime_t stopTime = par("stopTime");
            scheduleAt(simTime() + stopTime, selfStop_);
            EV << "TumClientApp5G::handleAckStartMEWarningAlertApp - Starting sendStopMEWarningAlertApp() in " << stopTime << " seconds " << endl;
        }
    }
    else
    {
        EV << "TumClientApp5G::handleAckStartMEWarningAlertApp - MEC application cannot be instantiated! Reason: " << pkt->getReason() << endl;
    }

    scheduleAt(simTime() + period_, selfMecAppStart_);
}

void TumClientApp5G::handleAckStopMEWarningAlertApp(cMessage *msg)
{

    inet::Packet *packet = check_and_cast<inet::Packet *>(msg);
    auto pkt = packet->peekAtFront<DeviceAppStopAckPacket>();

    EV << "TumClientApp5G::handleAckStopMEWarningAlertApp - Received " << pkt->getType() << " type WarningAlertPacket with result: " << pkt->getResult() << endl;
    if (pkt->getResult() == false)
        EV << "Reason: " << pkt->getReason() << endl;
    // updating runtime color of the car icon background
    ue->getDisplayString().setTagArg("i", 1, "white");

    cancelEvent(selfStop_);
}
