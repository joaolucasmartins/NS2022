//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "TumTargetedMobility.h"


#include "../../tumclient/TumClientApp.h"

#include <iostream>

using namespace omnetpp;

Define_Module(TumTargetedMobility);

TumTargetedMobility::TumTargetedMobility(): MovingMobilityBase()
{
    targetPosition = restCoords;
    lastPosition = restCoords;
    stationary = true;
}

void TumTargetedMobility::initialize(int stage)
{
    MovingMobilityBase::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        sg = static_cast<SbahnNetworkManager*>(getModuleByPath("sbahnNetworkGenerator"));
        speed = par("speed");
        WATCH(stationary);
        WATCH(targetPosition);
        WATCH(lastVelocity);
        getParentModule()->getDisplayString().setTagArg("i", 0, "vehicles/bicycle");
    }
}

void TumTargetedMobility::setInitialPosition()
{
    if (stationary) {
        targetPosition = restCoords;
        lastPosition = restCoords;
        nextChange = -1;
        if (hasGUI())
            getParentModule()->getDisplayString().setTagArg("t", 0, "");
    } else {
        target = sg->getRandomStation();
        lastPosition = sg->getStationOutskirtsPos(&target, 400, 600);
        targetPosition = target.getCoord();
        if (hasGUI())
            getParentModule()->getDisplayString().setTagArg("t", 0, target.name.c_str());
    }
}

void TumTargetedMobility::initializePosition()
{
    MovingMobilityBase::initializePosition(); // Internally calls setInitialPosition()
    if (!stationary) {
        lastVelocity = (targetPosition - lastPosition).normalize() * speed;

        EV_INFO << "new trajectory from position = " << targetPosition << " to station " << target.id << endl;
    } else  {
        lastVelocity = inet::Coord::ZERO;
    }
    lastUpdate = simTime();
    scheduleUpdate();
}

void TumTargetedMobility::handleMessage(cMessage *message)
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
            initializePosition();
        } else
            throw cRuntimeError("Invalid msg: name='%s' kind=%d", message->getName(), message->getKind());

        delete message;
    }
}


void TumTargetedMobility::move()
{
    simtime_t now = simTime();
    if (!stationary) {
        double elapsedTime = (now - lastUpdate).dbl();
        lastPosition += lastVelocity * elapsedTime;

//        if (lastPosition.distance(targetPosition) <= 10) {
//            stationary = true;
//        }
    }
}



