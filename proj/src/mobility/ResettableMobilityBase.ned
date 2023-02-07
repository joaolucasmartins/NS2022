package tum.mobility;

import inet.mobility.base.MobilityBase;

//
// Abstract base module for mobility models.
//
simple ResettableMobilityBase extends MobilityBase
{
    parameters:
        double updateInterval @unit(s) = default(0.1s); // the simulation time interval used to regularly signal mobility state changes and update the display
        bool faceForward = default(true);
}
