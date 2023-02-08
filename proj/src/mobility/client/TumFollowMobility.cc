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

#include "TumFollowMobility.h"

#include <iostream>

using namespace omnetpp;

Define_Module(TumFollowMobility);

TumFollowMobility::TumFollowMobility(): MovingMobilityBase()
{
    lastPosition = restCoords;
    stationary = true;
    train = nullptr;
}

void TumFollowMobility::initialize(int stage)
{
    MovingMobilityBase::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        sg = static_cast<SbahnNetworkManager*>(getModuleByPath("sbahnNetworkGenerator"));
        WATCH(stationary);
        getParentModule()->getDisplayString().setTagArg("i", 0, "misc/person4");
    }
}

void TumFollowMobility::setInitialPosition()
{
    if (stationary) {
        lastPosition = restCoords;
        nextChange = -1;
    } else {
        // Pick random train :upside_down:
        SbahnNetworkManager::TrainRep t = sg->getRandomActiveTrain();
        train = static_cast<MobilityBase*>( t.train->getSubmodule("mobility") );
        nextChange = t.stopTime;
        lastPosition = train->getCurrentPosition();
    }
}

void TumFollowMobility::initializePosition()
{
    MovingMobilityBase::initializePosition(); // Internally calls setInitialPosition()
    lastVelocity = inet::Coord::ZERO;

    lastUpdate = simTime();
    scheduleUpdate();
}

void TumFollowMobility::handleMessage(cMessage *message)
{
    if (message->isSelfMessage()) {
        handleSelfMessage(message);
    }
    else {
//        std::cout << "[MOB] Got message: " << message->getName() << std::endl;
        if (!strcmp(message->getName(), "START")) {
            stationary = false;
            initializePosition();
        } else if (!strcmp(message->getName(), "END")) {
            stationary = true;
            nextChange = -1;
            initializePosition();
        } else
            throw cRuntimeError("Invalid msg: name='%s' kind=%d", message->getName(), message->getKind());

        delete message;
    }
}


void TumFollowMobility::move()
{
    simtime_t now = simTime();

    if (now == nextChange) {
        stationary = true;
        nextChange = -1;
    } else if (!stationary) {
        // Copy position of train
        lastPosition = train->getCurrentPosition();
    }
}



