//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __TRAINULTRAPRECISEMONITORING_TUMTARGETEDMOBILITY_H
#define __TRAINULTRAPRECISEMONITORING_TUMTARGETEDMOBILITY_H

#include <inet/mobility/base/LineSegmentsMobilityBase.h>

#include "TumBaseMobility.h"
#include "../../sbahn/networkgenerator/SbahnNetworkGenerator.h"

/**
 * This mobility module does not move at all; it can be used for standalone stationary nodes.
 *
 * @ingroup mobility
 */
class TumTargetedMobility : public inet::LineSegmentsMobilityBase, TumBaseMobility
{
protected:
  SbahnNetworkGenerator *sg;

  // state
  SbahnNetworkGenerator::Station target;
  inet::Coord origin;

protected:

  /** @brief Initializes mobility model parameters. */
  virtual void initialize(int stage) override;

  /** @brief Initializes the position according to the mobility model. */
  virtual void setInitialPosition() override;

  /** @brief Overridden from LineSegmentsMobilityBase. */
  virtual void setTargetPosition() override;

  virtual void computeMaxSpeed();

public:
  TumTargetedMobility();

  virtual void onStartCommunication() override;
  virtual void onEndCommunication() override;
};


#endif

