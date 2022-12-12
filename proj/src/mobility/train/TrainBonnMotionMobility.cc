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

#include "TrainBonnMotionMobility.h"

#include <iostream>

Define_Module(TrainBonnMotionMobility);

void TrainBonnMotionMobility::initialize(int stage) {
    inet::LineSegmentsMobilityBase::initialize(stage);

    EV_TRACE << "initializing TrainBonnMotionMobility stage " << stage << endl;
    if (stage == inet::INITSTAGE_LOCAL) {
        is3D = par("is3D");
        int nodeId = getParentModule()->par("trainId");
        std::cout << "Bonn nodeId: " << std::endl;
        if (nodeId == -1)
            nodeId = getContainingNode(this)->getIndex();
        const char *fname = par("traceFile");
        const inet::BonnMotionFile *bmFile = inet::BonnMotionFileCache::getInstance()->getFile(fname);
        lines = bmFile->getLine(nodeId);
        if (!lines)
            throw cRuntimeError("Invalid nodeId %d -- no such line in file '%s'", nodeId, fname);
        currentLine = 0;
        computeMaxSpeed();
    }
}
