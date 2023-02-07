//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __TRAINULTRAPRECISEMONITORING_TUMTARGETEDMOBILITY_H
#define __TRAINULTRAPRECISEMONITORING_TUMTARGETEDMOBILITY_H

#include "inet/mobility/base/MovingMobilityBase.h"
#include "../ResettableMobilityBase.h"
#include "../../sbahn/networkgenerator/SbahnNetworkGenerator.h"
#include "../../sbahn/Station.h"

/**
 * This mobility module does not move at all; it can be used for standalone stationary nodes.
 *
 * @ingroup mobility
 */
class TumTargetedMobility : public inet::MovingMobilityBase
{
protected:
  SbahnNetworkGenerator *sg;

  const inet::Coord restCoords = inet::Coord(500, 100);
  double speed;

  // state
  Station target;
  inet::Coord targetPosition;

protected:

  /** @brief Initializes mobility model parameters. */
  virtual void initialize(int stage) override;

  /** @brief Initializes the position according to the mobility model. */
  virtual void setInitialPosition() override;

  virtual void initializePosition() override;

  virtual void move() override;

  virtual void handleMessage(omnetpp::cMessage *message) override;

public:
  TumTargetedMobility();

};


#endif

