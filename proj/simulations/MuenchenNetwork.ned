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
import ned.DatarateChannel;
import inet.node.inet.Router;

import tum.tumclient.TumClient;
import tum.tumtrain.TumTrain;
import tum.tumserver.TumServer;

import tum.sbahn.SbahnNetworkGenerator;
import tum.sbahn.SbahnStop;

network MuenchenNetwork
{
    parameters:
        int nClients;
        int nTrains;
        @display("bgb=93253,52926;bgi=maps/muenchen,s;");
    
    types:
        channel C extends DatarateChannel
        {
            datarate = 100Mbps;
            delay = 0.1us;
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s;");
        }
        client[nClients]: TumClient {
            parameters:
                @display("i=device/laptop;p=250,100,col,100;");
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
        
        sbahnNetworkGenerator: SbahnNetworkGenerator {
            @display("p=100,100";)
        	filename = "sbahn/muenchen_sbahn.txt";
        	disable = false;
        };

		
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
