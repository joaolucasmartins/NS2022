//
// Copyright (C) 2006 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __TRAINULTRAPRECISEMONITORING_TUMBASEMOBILITY_H
#define __TRAINULTRAPRECISEMONITORING_TUMBASEMOBILITY_H


/**
 * This mobility module does not move at all; it can be used for standalone stationary nodes.
 *
 * @ingroup mobility
 */
class TumBaseMobility {
public:
    virtual void onStartCommunication() = 0;
    virtual void onEndCommunication() = 0;
};


#endif

