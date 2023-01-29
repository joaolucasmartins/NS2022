//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "TumStationaryMobility.h"

Define_Module(TumStationaryMobility);

void TumStationaryMobility::initialize(int stage) {
    StationaryMobilityBase::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        sg = static_cast<SbahnNetworkGenerator*>(getModuleByPath("sbahnNetworkGenerator"));
    }
}


inet::Coord TumStationaryMobility::getRandomPosition() {
    return sg->getStationOutskirtsPos();
}

