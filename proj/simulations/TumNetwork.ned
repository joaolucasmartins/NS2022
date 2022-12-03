//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//
//

package tum.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;
import inet.node.inet.Router;

import tum.tumserver.TrainManager;


module TumClient extends StandardHost {
}

module TumTrain extends StandardHost {
}

module TumServer extends StandardHost {
    submodules:
        trainManager: TrainManager;
}

network TumNetwork
{
    parameters:
        int nClients;
        int nTrains;
    types:
        channel C extends DatarateChannel
        {
            datarate = 100Mbps;
            delay = 0.1us;
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s");
        }
        client[nClients]: TumClient {
            parameters:
                @display("i=device/laptop;p=250,100,col,100");
        }
        train[nTrains]: TumTrain {
            parameters:
                @display("i=trains/s2;p=550,100,col,100");
        }
        server: TumServer {
        	parameters:
        	    @display("p=400,250;i=device/pc2");
        }
        clientRouter: Router {
        	parameters:
        	    @display("p=375,150");
        }
        trainRouter: Router {
        	parameters:
        	    @display("p=425,150");
        }
    connections:
        for i=0..nClients-1 {
            client[i].pppg++ <--> C <--> clientRouter.pppg++;
        }
        for i=0..nTrains-1 {
            train[i].pppg++ <--> C <--> trainRouter.pppg++;
        }
        clientRouter.pppg++ <--> C <--> server.pppg++;
        trainRouter.pppg++ <--> C <--> server.pppg++;
}

