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

#include <inet/common/ModuleAccess.h>
#include <inet/common/ProtocolTag_m.h>
#include <inet/common/TimeTag_m.h>
#include <inet/common/lifecycle/NodeStatus.h>
#include <inet/common/packet/Message.h>
#include <inet/common/packet/chunk/ByteCountChunk.h>
#include <inet/common/socket/SocketTag_m.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/contract/tcp/TcpCommand_m.h>

#include "TumClientServerApp.h"
#include "../../common/ClientPacket.h"
#include "../../common/ClientResponsePacket.h"
#include "../../common/TrainInfo.h"

#include <iostream>

Define_Module(TumClientServerApp);

TrainManager* TumClientServerApp::getTrainManager() {
    cModule *module = getModuleByPath("server.trainManager");
    trainManager = static_cast<TrainManager*>(module);
    return trainManager;
}

void TumClientServerApp::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numTrainUpdatesServedVec.setName("serverSentTrainUpdates");
        numTrainUpdatesDroppedVec.setName("serverDroppedTrainUpdates");
        trainDropTimeLimit = par("trainDropTimeLimit");

        delay = par("replyDelay");
        maxMsgDelay = 0;
        std::cout << "Boop" << std::endl;
        // statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        WATCH(msgsRcvd);
        WATCH(msgsSent);
        WATCH(bytesRcvd);
        WATCH(bytesSent);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        getTrainManager();

        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void TumClientServerApp::sendOrSchedule(cMessage *msg, simtime_t delay)
{
    if (delay == 0)
        sendBack(msg);
    else
        scheduleAfter(delay, msg);
}

void TumClientServerApp::sendBack(cMessage *msg)
{
    Packet *packet = dynamic_cast<Packet *>(msg);

    if (packet) {
        msgsSent++;
        bytesSent += packet->getByteLength();
        emit(packetSentSignal, packet);

        EV_INFO << "sending \"" << packet->getName() << "\" to TCP, " << packet->getByteLength() << " bytes\n";
    }
    else {
        EV_INFO << "sending \"" << msg->getName() << "\" to TCP\n";
    }

    auto& tags = check_and_cast<ITaggedObject *>(msg)->getTags();
    tags.addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    send(msg, "socketOut");
}

void TumClientServerApp::filterPackets(map<int, vector<TrainInfo>> &trackInfo) {
    this->droppedUpdates = 0;
    for(auto it = trackInfo.begin(); it != trackInfo.end(); ++it ) {
        vector<TrainInfo>& trains = it->second;
        trains.erase(
            remove_if(trains.begin(), trains.end(), [&](TrainInfo info) {
                double timeDiffSec = (simTime() - info.getTime()).dbl();
                if (timeDiffSec > this->trainDropTimeLimit) { // Drop entries larger than 1 min
                    this->droppedUpdates++;
                    return true;
                }
                return false;
            }), trains.end()
        );
    }

    numTrainUpdatesDroppedVec.record(this->droppedUpdates);
    numTrainUpdatesDroppedStats.collect(this->droppedUpdates);
}

void TumClientServerApp::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        sendBack(msg);
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {
        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        sendOrSchedule(request, delay + maxMsgDelay);
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {
        Packet *packet = check_and_cast<Packet *>(msg);
        int connId = packet->getTag<SocketInd>()->getSocketId();
        ChunkQueue& queue = socketQueue[connId];
        auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
        queue.push(chunk);
        emit(packetReceivedSignal, packet);

        bool doClose = false;
        while (queue.has<ClientPacket>(b(-1))) {
            const auto& appmsg = queue.pop<ClientPacket>(b(-1));
            msgsRcvd++;
            bytesRcvd += B(appmsg->getChunkLength()).get();

            EV_INFO << "received request with " << appmsg->getTracks().size() << " tracks\n";
            // TODO simtime_t msgDelay = appmsg->getReplyDelay();
            // if (msgDelay > maxMsgDelay)
            //    maxMsgDelay = msgDelay;
            Packet *outPacket = new Packet(msg->getName(), TCP_C_SEND);
            outPacket->addTag<SocketReq>()->setSocketId(connId);


            // Generate response with requested track information
            TrainManager *manager = getTrainManager();
            const ClientPacket *clientMsg = static_cast<const ClientPacket*>(appmsg.get());
            map<int, vector<TrainInfo>> trackInfo = manager->getTrackInfo(clientMsg->getTracks());
            filterPackets(trackInfo);
            ClientResponsePacket responseMsg(trackInfo);
            EV_INFO << "------------------" << endl;
            printTrainInfo(trackInfo);
            EV_INFO << "------------------" << endl;

            const auto& payload = makeShared<ClientResponsePacket>(trackInfo);
            int trainUpdates = 0;
            for (const auto &it: trackInfo) {
                trainUpdates += it.second.size();
            }
            numTrainUpdatesServedVec.record(trainUpdates);
            numTrainUpdatesServedStats.collect(trainUpdates);
            payload->setChunkLength(B(1));
            payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
            outPacket->insertAtBack(payload);
            sendOrSchedule(outPacket, delay); // + msgDelay
        }
        delete msg;

        if (doClose) {
            auto request = new Request("close", TCP_C_CLOSE);
            TcpCommand *cmd = new TcpCommand();
            request->addTag<SocketReq>()->setSocketId(connId);
            request->setControlInfo(cmd);
            sendOrSchedule(request, delay + maxMsgDelay);
        }
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
        socket.processMessage(msg);
    else {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

void TumClientServerApp::refreshDisplay() const
{
    char buf[64];
    sprintf(buf, "rcvd: %ld pks %ld bytes\nsent: %ld pks %ld bytes", msgsRcvd, bytesRcvd, msgsSent, bytesSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void TumClientServerApp::finish()
{
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";

    EV_INFO << getFullPath() << ": Train Update count, min:    " << numTrainUpdatesServedStats.getMin() << endl;
    EV_INFO << getFullPath() << ": Train Update count, max:    " << numTrainUpdatesServedStats.getMax() << endl;
    EV_INFO << getFullPath() << ": Train Update count, mean:   " << numTrainUpdatesServedStats.getMean() << endl;
    EV_INFO << getFullPath() << ": Train Update count, stddev: " << numTrainUpdatesServedStats.getStddev() << endl;

    recordScalar("#bytesSent", bytesSent);
    recordScalar("#bytesRcvd", bytesRcvd);
    numTrainUpdatesServedStats.recordAs("serverSentTrainUpdates");
    numTrainUpdatesDroppedStats.recordAs("serverDroppedTrainUpdates");
}

