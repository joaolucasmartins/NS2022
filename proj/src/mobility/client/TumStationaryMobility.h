//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __TRAINULTRAPRECISEMONITORING_TUMSTATIONARYMOBILITY_H
#define __TRAINULTRAPRECISEMONITORING_TUMSTATIONARYMOBILITY_H

#include <inet/mobility/base/StationaryMobilityBase.h>

#include "TumStationaryMobility.h"

#include "../../sbahn/manager/SbahnNetworkManager.h"

/**
 * This mobility module does not move at all; it can be used for standalone stationary nodes.
 *
 * @ingroup mobility
 */
class TumStationaryMobility : public inet::StationaryMobilityBase
{
  protected:
    double radius;
    SbahnNetworkManager *sg;

  protected:
    virtual void initialize(int stage) override;
    virtual inet::Coord getRandomPosition() override;

    virtual void handleMessage(omnetpp::cMessage *message) override;
};


#endif

