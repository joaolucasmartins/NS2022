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

#include "../../tumtrain/TumTrainApp.h"

#include <iostream>

Define_Module(TrainBonnMotionMobility);

// COPY STUFF FROM BONN MOTION

TrainBonnMotionMobility::TrainBonnMotionMobility() :
        startTimer(nullptr)
{
}

TrainBonnMotionMobility::~TrainBonnMotionMobility()
{
    cancelAndDelete(startTimer);
}

void TrainBonnMotionMobility::initialize(int stage)
{
    BonnMotionMobility::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        stationary = true;
        startTimer = new cMessage("start");
    } else if (stage == inet::INITSTAGE_LAST) {
        TumTrainApp *app = static_cast<TumTrainApp*>(getParentModule()->getSubmodule("app", 0));
        scheduleAt(app->par("startTime"), startTimer);
    }
}

void TrainBonnMotionMobility::handleSelfMessage(cMessage *message)
{
    if (!strcmp(message->getName(), "start")) {
        stationary = false;
    } else {
        moveAndUpdate();
        scheduleUpdate();
    }
}



