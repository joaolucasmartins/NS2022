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
        @display("bgb=93253,52926;bgi=maps/muenchen,s;");
    
    types:
        channel C extends Eth100M {}
    submodules:
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s;");
        }
        client[nClients]: TumClient {
            parameters:
                @display("i=device/laptop;p=250,100,col,100;");
        }
        train[0]: TumTrain { @dynamic; }
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
            @display("p=100,200");
        	filename = "sbahn/muenchen_sbahn.txt";
        	disable = false;
        };

		trainNetworkGenerator: SbahnTrainGenerator {
			@display("p=100, 300");
			trainRouterPattern="*.trainRouter";
		}
		
    connections:
        for i=0..nClients-1 {
            client[i].ethg++ <--> C <--> clientRouter.ethg++;
        }
//        for i=0..nTrains-1 {
//            train[i].ethg++ <--> C <--> trainRouter.ethg++;
//        }
        clientRouter.ethg++ <--> C <--> server.ethg++;
        trainRouter.ethg++ <--> C <--> server.ethg++;
        
}
