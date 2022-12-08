//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//
//

package tum.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;
import inet.node.inet.Router;

import tum.tumclient.TumClient;
import tum.tumtrain.TumTrain;
import tum.tumserver.TumServer;


network TumNetwork
{
    parameters:
        int nClients;
        int nTrains;
    types:
        channel C extends Eth100M {}
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
            client[i].ethg++ <--> C <--> clientRouter.ethg++;
        }
        for i=0..nTrains-1 {
            train[i].ethg++ <--> C <--> trainRouter.ethg++;
        }
        clientRouter.ethg++ <--> C <--> server.ethg++;
        trainRouter.ethg++ <--> C <--> server.ethg++;
}

