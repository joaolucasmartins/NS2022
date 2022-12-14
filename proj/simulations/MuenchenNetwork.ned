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
// //
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//
//

package tum.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import inet.node.ethernet.Eth100M;
import inet.node.inet.Router;

import tum.tumclient.TumClient;
import tum.tumtrain.TumTrain;
import tum.tumserver.TumServer;

import tum.sbahn.networkgenerator.SbahnNetworkGenerator;
import tum.sbahn.traingenerator.SbahnTrainGenerator;

network MuenchenNetwork
{
    parameters:
        int nClients;
        int nTrains;
        @display("bgi=maps/muenchen,s;");
    
    types:
        channel C extends Eth100M {}
        channel TC extends Eth100M { @display("ls=lightgray,0.25"); }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=500,500;is=s;");
        }
        sbahnNetworkGenerator: SbahnNetworkGenerator {
            @display("p=500,1000;is=s;i=block/wheelbarrow");
        	filename = "sbahn/muenchen_sbahn.txt";
        	disable = false;
        };
        
        client[nClients]: TumClient {
            parameters:
                @display("is=vs;i=device/pocketpc;p=6000,1000,m,10,250,250;");
        }
        train[nTrains]: TumTrain;

        server: TumServer {
        	parameters:
        	    @display("p=3000,2000;i=device/server");
        }
        clientRouter: Router {
        	parameters:
        	    @display("p=3500,3000;i=device/smallrouter");
        }
        trainRouter: Router {
        	parameters:
        	    @display("p=2500,3000;i=device/smallrouter");
        }
        

    connections:
        for i=0..nClients-1 {
            client[i].ethg++ <--> C <--> clientRouter.ethg++;
        }
        for i=0..nTrains-1 {
            train[i].ethg++ <--> TC <--> trainRouter.ethg++;
        }

        clientRouter.ethg++ <--> C <--> server.ethg++;
        trainRouter.ethg++ <--> C <--> server.ethg++;
        
}
