/*
 * ResettableMobility.h
 *
 *  Created on: Feb 7, 2023
 *      Author: tiago
 */

#ifndef SRC_MOBILITY_RESETTABLEMOBILITYBASE_H_
#define SRC_MOBILITY_RESETTABLEMOBILITYBASE_H_

#include <inet/mobility/base/MobilityBase.h>

/**
 * @brief Base class for moving mobility modules. Periodically emits a signal with the current mobility state.
 *
 * @ingroup mobility
 */
class ResettableMobilityBase : public inet::MobilityBase
{
  protected:
    /** @brief The message used for mobility state changes. */
    omnetpp::cMessage *moveTimer;

    /** @brief The simulation time interval used to regularly signal mobility state changes.
     *
     * The 0 value turns off the signal. */
    omnetpp::simtime_t updateInterval;

    /** @brief A mobility model may decide to become stationary at any time.
     *
     * The true value disables sending self messages. */
    bool stationary;

    /** @brief The last velocity that was reported at lastUpdate. */
    inet::Coord lastVelocity;

    /** @brief The last angular velocity that was reported at lastUpdate. */
    inet::Quaternion lastAngularVelocity;

    /** @brief The simulation time when the mobility state was last updated. */
    omnetpp::simtime_t lastUpdate;

    /** @brief The next simulation time when the mobility module needs to update its internal state.
     *
     * The -1 value turns off sending a self message for the next mobility state change. */
    omnetpp::simtime_t nextChange;

    bool faceForward;

    bool reset, pauseFixedUpdates;

  protected:
    ResettableMobilityBase();

    virtual ~ResettableMobilityBase();

    virtual void initialize(int stage) override;

    virtual void initializePosition() override;

    virtual void handleSelfMessage(omnetpp::cMessage *message) override;

    /** @brief Schedules the move timer that will update the mobility state. */
    void scheduleUpdate();

    /** @brief Moves and notifies listeners. */
    void moveAndUpdate();

    /** @brief Moves according to the mobility model to the current simulation time.
     *
     * Subclasses must override and update lastPosition, lastVelocity, lastUpdate, nextChange
     * and other state according to the mobility model.
     */
    virtual void move() = 0;

    virtual void orient();

  public:
    virtual const inet::Coord& getCurrentPosition() override;
    virtual const inet::Coord& getCurrentVelocity() override;
    virtual const inet::Coord& getCurrentAcceleration() override { throw omnetpp::cRuntimeError("Invalid operation"); }

    virtual const inet::Quaternion& getCurrentAngularPosition() override;
    virtual const inet::Quaternion& getCurrentAngularVelocity() override;
    virtual const inet::Quaternion& getCurrentAngularAcceleration() override { throw omnetpp::cRuntimeError("Invalid operation"); }
};

#endif /* SRC_MOBILITY_CLIENT_RESETTABLEMOBILITY_H_ */

