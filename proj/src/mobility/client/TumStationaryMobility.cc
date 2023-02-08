//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "TumStationaryMobility.h"

#include "../../tumclient/TumClientApp.h"

Define_Module(TumStationaryMobility);

void TumStationaryMobility::initialize(int stage) {
    StationaryMobilityBase::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        sg = static_cast<SbahnNetworkManager*>(getModuleByPath("sbahnNetworkGenerator"));
        getParentModule()->getDisplayString().setTagArg("i", 0, "misc/town");
    }
}


inet::Coord TumStationaryMobility::getRandomPosition() {
    return sg->getStationOutskirtsPos();
}


void TumStationaryMobility::handleMessage(cMessage *message)
{
    if (message->isSelfMessage())
        handleSelfMessage(message);
}
