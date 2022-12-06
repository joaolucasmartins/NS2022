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

#include "SbahnNetworkGenerator.h"

#include <fstream>
#include <sstream>

Define_Module(SbahnNetworkGenerator);

void SbahnNetworkGenerator::initialize(int stage)
{
    if (!par("disable"))
        parseFile();
}

void SbahnNetworkGenerator::parseFile() {
    cModule *parent = getParentModule();

    std::string filename = par("filename");
    std::ifstream in(filename, std::ios::in);
    if (in.fail())
        throw cRuntimeError("Cannot open file '%s'", filename.c_str());

    int maxLat, maxLon, nNodes, nEdges;
    in >> maxLat >> maxLon >> nNodes;

    for (int i=0; i<nNodes; ++i) {
        int id, degree, lat, lon;
        std::string name;

        in >> id >> degree >> lat >> lon;
        std::getline(in, name);

        addStop(parent, id, degree, lon, lat, name);
    }

    in >> nEdges;
    for (int i=0; i<nEdges; ++i) {
        int a, b, routes;
        in >> a >> b >> routes;

        addConnection(a, b, routes);
    }

    stops.clear();
}

void SbahnNetworkGenerator::addStop(cModule *node, int id, int degree, int lat, int lon, const std::string &name) {
    // find factory object
    cModuleType *moduleType = cModuleType::get("tum.sbahn.SbahnStop");

    // create (possibly compound) module and build its submodules (if any)
    cModule *module = moduleType->create(("s_" + std::to_string(id)).c_str(), node);

    // set up parameters and gate sizes before we set up its submodules
    // parameters: x="74874"; y="5988"; stopName="Flughafen Terminal";
    module->par("x") = std::to_string(lon);
    module->par("y") = std::to_string(lat);
    module->par("stopName") = name;
    module->finalizeParameters();

    module->setGateSize("port", degree);

    // create internals, and schedule activation message
    module->buildInside();
    module->scheduleStart(simTime());

    // add to map
    stops[id] = module;
}

void SbahnNetworkGenerator::addConnection(int a, int b, int routes) {
    cGate *aGateIn, *aGateOut, *bGateIn, *bGateOut;
    stops[a]->getOrCreateFirstUnconnectedGatePair("port", false, false, aGateIn, aGateOut);
    stops[b]->getOrCreateFirstUnconnectedGatePair("port", false, false, bGateIn, bGateOut);

    aGateOut->connectTo(bGateIn);
    bGateOut->connectTo(aGateIn);

    aGateOut->getDisplayString().setTagArg("ls", 0, "#008D4F");
    aGateOut->getDisplayString().setTagArg("ls", 2, "d");

    bGateOut->getDisplayString().setTagArg("ls", 0, "#008D4F");
    bGateOut->getDisplayString().setTagArg("ls", 2, "da");
}

int SbahnNetworkGenerator::numInitStages() const {
    return 1;
}
