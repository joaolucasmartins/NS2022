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
    std::string trainFileName = par("trainFile");
    trainIn.open(trainFileName, std::ios::in);
    if (trainIn.fail())
        throw cRuntimeError("Cannot open file '%s'", trainFileName.c_str());

    trainIn >> nTrains;

    getParentModule()->addSubmoduleVector(VEC_NAME, 0);

//    createNextTrain();
    addTrain(getParentModule(), 0, 1);
}

void SbahnTrainGenerator::createNextTrains() {
    cModule *parent = getParentModule();
    int startTime, endTime, route;

    auto curPos = trainIn.tellg();
    trainIn >> startTime >> endTime >> route;

//    simtime_t now = simTime(), start = SimTime(startTime, SimTimeUnit::SIMTIME_S);
//    if ()

//    trainIn.seekg(__off, __dir)
}

void SbahnTrainGenerator::addTrain(cModule *parent, int bonnIndex, int route) {
    // find factory object
    cModuleType *moduleType = cModuleType::get("tum.tumtrain.TumTrain");

    // create (possibly compound) module and build its submodules (if any)
    parent->setSubmoduleVectorSize(VEC_NAME, 0 + 1);
    parent->getSubmoduleVectorSize(VEC_NAME);
    cModule* module = moduleType->create(VEC_NAME, parent, 0);

    // set up parameters and gate sizes before we set up its submodules
//        module->par("x") = std::to_string(lon);
//        module->par("y") = std::to_string(lat);
//        module->par("stopName") = name;

    module->par("trainId") = bonnIndex;
    module->par("trackId") = route;

    module->finalizeParameters();


    // create internals, and schedule activation message
    module->buildInside();

    module->scheduleStart(simTime());
}






void SbahnTrainGenerator::handleMessage(cMessage *msg) {

}






