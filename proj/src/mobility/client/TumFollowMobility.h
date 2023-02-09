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

#ifndef __TUM_TUMFOLLOWMOBILITY_H_
#define __TUM_TUMFOLLOWMOBILITY_H_

#include <omnetpp.h>

#include "../../sbahn/manager/SbahnNetworkManager.h"
#include <inet/mobility/base/MovingMobilityBase.h>
#include <inet/mobility/base/MobilityBase.h>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class TumFollowMobility : public inet::MovingMobilityBase
{
    SbahnNetworkManager *sg;

    const inet::Coord restCoords = inet::Coord(700, 300);

    // state
    MobilityBase *train;

  protected:

    /** @brief Initializes mobility model parameters. */
    virtual void initialize(int stage) override;

    /** @brief Initializes the position according to the mobility model. */
    virtual void setInitialPosition() override;

    virtual void initializePosition() override;

    virtual void move() override;

    virtual void handleMessage(omnetpp::cMessage *message) override;

  public:
    TumFollowMobility();

};

#endif
