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

#include "SbahnTrainGenerator.h"

#include <vector>
#include <iostream>

Define_Module(SbahnTrainGenerator);

void SbahnTrainGenerator::initialize(int stage)
{
    trainRouterPattern = par("trainRouterPattern").stringValue();
    portName = par("portName").stringValue();

    std::string trainFileName = par("trainFile");
    trainIn.open(trainFileName, std::ios::in);
    if (trainIn.fail())
        throw cRuntimeError("Cannot open file '%s'", trainFileName.c_str());

    trainIn >> nTrains >> peekedTime;

    getParentModule()->addSubmoduleVector(VEC_NAME, 4);
    nActiveTrains = 0;
    curTrainIndex = 0;

    createNextTrains();
}

void SbahnTrainGenerator::createNextTrains() {
    cModule *parent = getParentModule();
    int endTime, route;
    simtime_t now = simTime(), simtime_peeked = SimTime(peekedTime, SimTimeUnit::SIMTIME_S);

    while (simtime_peeked <= now) {
        trainIn >> endTime >> route;

        if (!trainIn.good()) {
            throw cRuntimeError("SbahnTrainGenerator: Train file has errors");
        }

        int slot = addTrain(parent, curTrainIndex++, route);
        nActiveTrains++;

        std::cout << "ADDED TRAIN" << std::endl;

        simtime_t simtime_end = SimTime(endTime, SimTimeUnit::SIMTIME_S);

        cMessage *delMsg = new cMessage(std::to_string(slot).c_str(), DELETE_TRAIN);
        scheduleAt(simtime_end, delMsg);

        std::cout << "SCHEDULED" << std::endl;

        trainIn >> peekedTime;
        simtime_peeked = SimTime(peekedTime, SimTimeUnit::SIMTIME_S);
    }

    cMessage *msg = new cMessage("new", NEW_TRAIN);
    scheduleAt(simtime_peeked, msg);
}

int SbahnTrainGenerator::getValidVectorIndex(cModule *parent) {
    int curSize = parent->getSubmoduleVectorSize(VEC_NAME);

    if (nActiveTrains >= curSize - 1) {
        curSize *= 2;
        parent->setSubmoduleVectorSize(VEC_NAME, curSize);
    }

    int slot = -1;
    for (int i=0; i < curSize; ++i) {
        if (parent->getSubmodule(VEC_NAME, i) == nullptr) {
            slot = i;
            break;
        }
    }

    if (slot == -1)
        throw cRuntimeError("SbahnTrainGenerator: Could not find free spot in vector\n");

    return slot;
}

int SbahnTrainGenerator::addTrain(cModule *parent, int bonnIndex, int route) {
    // find factory object
    cModuleType *moduleType = cModuleType::get("tum.tumtrain.TumTrain");

    // create (possibly compound) module and build its submodules (if any)
    int slot = getValidVectorIndex(parent);
    cModule* module = moduleType->create(VEC_NAME, parent, slot);

    if (module == nullptr) {
        throw cRuntimeError("Train module was not successfully instanced");
    }

    // set up parameters and gate sizes before we set up its submodules
    module->par("trainId") = bonnIndex;
    module->par("trackId") = route;

    module->finalizeParameters();

    // setup gate, emulating Eth100M
    cDatarateChannel *c1 = cDatarateChannel::create("channel"), *c2 = cDatarateChannel::create("channel");
    c1->setDatarate(100e6);
    c2->setDatarate(100e6);
    c1->setDelay(10 / 2e8);
    c2->setDelay(10 / 2e8);

    // create connections
    cModule *router = getModuleByPath(trainRouterPattern.c_str());
    cGate *moduleGateIn, *moduleGateOut, *routerGateIn, *routerGateOut;
    module->getOrCreateFirstUnconnectedGatePair(portName.c_str(), false, true, moduleGateIn, moduleGateOut);
    router->getOrCreateFirstUnconnectedGatePair(portName.c_str(), false, true, routerGateIn, routerGateOut);

    std::cout << moduleGateIn->getFullPath() << std::endl;

    if (moduleGateIn == nullptr || moduleGateOut == nullptr)
        throw cRuntimeError("SbahnTrainGenerator: Failed to retrieve the module's gates");
    if (routerGateIn == nullptr || routerGateOut == nullptr)
        throw cRuntimeError("SbahnTrainGenerator: Failed to retrieve the router's gates");

    moduleGateOut->connectTo(routerGateIn, c1);
    routerGateOut->connectTo(moduleGateIn, c2);

    moduleGateOut->getDisplayString().setTagArg("ls", 1, "0.5");
    routerGateOut->getDisplayString().setTagArg("ls", 1, "0.5");

    // create internals, and schedule activation message
    module->buildInside();
    module->scheduleStart(simTime());
    module->callInitialize();

    return slot;
}


void SbahnTrainGenerator::deleteTrain(int index) {
    cModule *train = getParentModule()->getSubmodule(VEC_NAME, index);

    // CALL disconnect() on the source gates, in order to ensure proper cleanup

    train->callFinish();
    train->deleteModule();
    nActiveTrains--;
}


void SbahnTrainGenerator::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        if (msg->getKind() == NEW_TRAIN) {
            createNextTrains();
        } else if (msg->getKind() == DELETE_TRAIN) {
            deleteTrain(atoi(msg->getName()));
        }
    }

    delete msg;
}






