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

Define_Module(SbahnTrainGenerator);

void SbahnTrainGenerator::initialize(int stage)
{
    connectionType = par("connectionType").getValue().str();
    trainRouterPattern = par("trainRouterPattern").getValue().str();
    portName = par("portName").getValue().str();

    std::string trainFileName = par("trainFile");
    trainIn.open(trainFileName, std::ios::in);
    if (trainIn.fail())
        throw cRuntimeError("Cannot open file '%s'", trainFileName.c_str());

    trainIn >> nTrains;

    getParentModule()->addSubmoduleVector(VEC_NAME, 0);

    createNextTrains();
}

void SbahnTrainGenerator::createNextTrains() {
    cModule *parent = getParentModule();
    int startTime, endTime, route;
    simtime_t now = simTime();

    do {
        auto curPos = trainIn.tellg();
        trainIn >> startTime >> endTime >> route;

        simtime_t start = SimTime(startTime, SimTimeUnit::SIMTIME_S);

        if (start > now) {
            trainIn.seekg(curPos, std::ios::beg);
            break;
        }

        addTrain(parent, curTrainIndex++, route);
    } while (true);
}

void SbahnTrainGenerator::addTrain(cModule *parent, int bonnIndex, int route) {
    // find factory object
    cModuleType *moduleType = cModuleType::get("tum.tumtrain.TumTrain");

    // create (possibly compound) module and build its submodules (if any)
    parent->setSubmoduleVectorSize(VEC_NAME, 0 + 1);
    parent->getSubmoduleVectorSize(VEC_NAME);
    cModule* module = moduleType->create(VEC_NAME, parent, 0);

    // set up parameters and gate sizes before we set up its submodules
    module->par("trainId") = bonnIndex;
    module->par("trackId") = route;

    module->finalizeParameters();

    // setup gate
    cChannelType *channelType = cChannelType::get(connectionType.c_str());
    cChannel *c1 = channelType->create("channel"), *c2 = channelType->create("channel");

    // create connections
    cModule *router = getModuleByPath(trainRouterPattern.c_str());
    cGate *moduleGateIn, *moduleGateOut, *routerGateIn, *routerGateOut;
    module->getOrCreateFirstUnconnectedGatePair(portName.c_str(), false, false, moduleGateIn, moduleGateOut);
    router->getOrCreateFirstUnconnectedGatePair(portName.c_str(), false, false, routerGateIn, routerGateOut);

    moduleGateOut->connectTo(routerGateIn, c1);
    routerGateOut->connectTo(moduleGateIn, c2);

    moduleGateOut->getDisplayString().setTagArg("ls", 1, "0.5");
    routerGateOut->getDisplayString().setTagArg("ls", 1, "0.5");

    // create internals, and schedule activation message
    module->buildInside();

    module->scheduleStart(simTime());
}






void SbahnTrainGenerator::handleMessage(cMessage *msg) {

}






