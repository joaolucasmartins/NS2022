//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __TRAINULTRAPRECISEMONITORING_TUMSTATIONARYMOBILITY_H
#define __TRAINULTRAPRECISEMONITORING_TUMSTATIONARYMOBILITY_H

#include <inet/mobility/base/StationaryMobilityBase.h>

#include "TumStationaryMobility.h"
#include "TumBaseMobility.h"
#include "../../sbahn/networkgenerator/SbahnNetworkGenerator.h"

/**
 * This mobility module does not move at all; it can be used for standalone stationary nodes.
 *
 * @ingroup mobility
 */
class TumStationaryMobility : public inet::StationaryMobilityBase, TumBaseMobility
{
  protected:
    double radius;
    SbahnNetworkGenerator *sg;

  protected:
    virtual void initialize(int stage) override;
    virtual inet::Coord getRandomPosition() override;

  public:
    virtual void onStartCommunication() override;
    virtual void onEndCommunication() override;
};


#endif

