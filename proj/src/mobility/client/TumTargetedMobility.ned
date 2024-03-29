//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package tum.mobility.client;

import inet.mobility.base.MovingMobilityBase;

//
// This mobility module does nothing; it can be used for stationary nodes.
//
simple TumTargetedMobility extends MovingMobilityBase
{
    parameters:
        double initialX @unit(m) = default(uniform(this.constraintAreaMinX, this.constraintAreaMaxX));
        double initialY @unit(m) = default(uniform(this.constraintAreaMinY, this.constraintAreaMaxY));
        double initialZ @unit(m) = default(nanToZero(uniform(this.constraintAreaMinZ, this.constraintAreaMaxZ)));
        double initialLatitude @unit(deg) = default(nan deg);
        double initialLongitude @unit(deg) = default(nan deg);
        double initialAltitude @unit(m) = default(0m);
        double initialHeading @unit(deg) = default(0deg);
        double initialElevation @unit(deg) = default(0deg);
        double initialBank @unit(deg) = default(0deg);
        bool initFromDisplayString = default(true); // enables one time initialization from the subject module's display string
        bool updateFromDisplayString = default(true); // enables continuous update from the subject module's display string for dragging and rotating it
        
        double speed @unit(mps) = default(5mps); // speed of the host
        @class(TumTargetedMobility);
	gates:
	    input direct @directIn;
}

