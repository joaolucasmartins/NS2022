//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "TumTargetedMobility.h"

Define_Module(TumTargetedMobility);

TumTargetedMobility::TumTargetedMobility()
{

}

void TumTargetedMobility::computeMaxSpeed()
{
//    const BonnMotionFile::Line& vec = *lines;
//    double lastTime = vec[0];
//    Coord lastPos(vec[1], vec[2], (is3D ? vec[3] : 0));
//    unsigned int step = (is3D ? 4 : 3);
//    for (unsigned int i = step; i < vec.size(); i += step) {
//        double elapsedTime = vec[i] - lastTime;
//        Coord currPos(vec[i + 1], vec[i + 2], (is3D ? vec[i + 3] : 0));
//        double distance = currPos.distance(lastPos);
//        double speed = distance / elapsedTime;
//        if (speed > maxSpeed)
//            maxSpeed = speed;
//        lastPos.x = currPos.x;
//        lastPos.y = currPos.y;
//        lastPos.z = currPos.z;
//        lastTime = vec[i];
//    }
}

void TumTargetedMobility::initialize(int stage)
{
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        nextChange = -1;
    } else if (stage == inet::INITSTAGE_SINGLE_MOBILITY) {

    }
}

void TumTargetedMobility::setInitialPosition()
{
//    const BonnMotionFile::Line& vec = *lines;
//    if (lines->size() >= 3) {
//        lastPosition.x = vec[1];
//        lastPosition.y = vec[2];
//    }
}

void TumTargetedMobility::setTargetPosition()
{
//    const BonnMotionFile::Line& vec = *lines;
//    if (currentLine + (is3D ? 3 : 2) >= (int)vec.size()) {
//        nextChange = -1;
//        stationary = true;
//        targetPosition = lastPosition;
//        return;
//    }
//    nextChange = vec[currentLine];
//    targetPosition.x = vec[currentLine + 1];
//    targetPosition.y = vec[currentLine + 2];
//    targetPosition.z = is3D ? vec[currentLine + 3] : 0;
//    currentLine += (is3D ? 4 : 3);
}

void TumTargetedMobility::onStartCommunication() {
    target = sg->getRandomStation();
    origin = sg->getStationOutskirtsPos(&target);

    initializeOrientation();
    initializePosition();
}

void TumTargetedMobility::onEndCommunication() {

}



