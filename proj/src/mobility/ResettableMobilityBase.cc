/*
 * ResettableMobility.cc
 *
 *  Created on: Feb 7, 2023
 *      Author: tiago
 */

#include "ResettableMobilityBase.h"

using namespace inet;
using namespace omnetpp;

ResettableMobilityBase::ResettableMobilityBase() :
    moveTimer(nullptr),
    updateInterval(0),
    stationary(false),
    lastVelocity(Coord::ZERO),
    lastUpdate(0),
    nextChange(-1),
    faceForward(false),
    reset(false),
    pauseFixedUpdates(false)
{
}

ResettableMobilityBase::~ResettableMobilityBase()
{
    cancelAndDelete(moveTimer);
}

void ResettableMobilityBase::initialize(int stage)
{
    MobilityBase::initialize(stage);
    EV_TRACE << "initializing ResettableMobilityBase stage " << stage << endl;
    if (stage == INITSTAGE_LOCAL) {
        moveTimer = new cMessage("move");
        updateInterval = par("updateInterval");
        faceForward = par("faceForward");
    }
}

void ResettableMobilityBase::initializePosition()
{
    MobilityBase::initializePosition();
    lastUpdate = simTime();
    scheduleUpdate();
}

void ResettableMobilityBase::moveAndUpdate()
{
    simtime_t now = simTime();
    if (nextChange == now || lastUpdate != now) {
        move();
        orient();
        lastUpdate = simTime();
        emitMobilityStateChangedSignal();
    }
}

void ResettableMobilityBase::orient()
{
    if (faceForward) {
        // determine orientation based on direction
        if (lastVelocity != Coord::ZERO) {
            Coord direction = lastVelocity;
            direction.normalize();
            auto alpha = rad(atan2(direction.y, direction.x));
            auto beta = rad(-asin(direction.z));
            auto gamma = rad(0.0);
            lastOrientation = Quaternion(EulerAngles(alpha, beta, gamma));
        }
    }
}

void ResettableMobilityBase::handleSelfMessage(cMessage *message)
{
    moveAndUpdate();
    scheduleUpdate();
}

void ResettableMobilityBase::scheduleUpdate()
{
    cancelEvent(moveTimer);
    // TODO: Add support for a "finalized" that completely stop the moveTimer
    if (!pauseFixedUpdates && updateInterval != 0) {
        // periodic update is needed
        simtime_t nextUpdate = simTime() + updateInterval;
        if (nextChange != -1 && nextChange < nextUpdate)
            // next change happens earlier than next update
            scheduleAt(nextChange, moveTimer);
        else
            // next update happens earlier than next change or there is no change at all
            scheduleAt(nextUpdate, moveTimer);
    } else if (pauseFixedUpdates) {
        if (nextChange != -1)
            // no periodic update is needed
            scheduleAt(nextChange, moveTimer);
        else
            throw cRuntimeError("ResettableMobilityBase: HOPE THIS DOESN'T TRIGGER");
    }
}

const Coord& ResettableMobilityBase::getCurrentPosition()
{
    moveAndUpdate();
    return lastPosition;
}

const Coord& ResettableMobilityBase::getCurrentVelocity()
{
    moveAndUpdate();
    return lastVelocity;
}

const Quaternion& ResettableMobilityBase::getCurrentAngularPosition()
{
    moveAndUpdate();
    return lastOrientation;
}

const Quaternion& ResettableMobilityBase::getCurrentAngularVelocity()
{
    moveAndUpdate();
    return lastAngularVelocity;
}


