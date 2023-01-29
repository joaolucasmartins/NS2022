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
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;

import tum.tumclient.TumClient5G;
import tum.tumtrain.TumTrain5G;
import tum.tumserver.TumServerClients;
import tum.tumserver.TumServerTrains;

import tum.sbahn.networkgenerator.SbahnNetworkGenerator;
import tum.sbahn.traingenerator.SbahnTrainGenerator;

import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.Upf;
import simu5g.nodes.NR.gNodeB;
import simu5g.world.radio.LteChannelControl;
import inet.networklayer.ipv4.RoutingTableRecorder;

network MuenchenNetwork
{
    parameters:
        int nClients;
        int nTrains;
        @display("bgi=maps/muenchen,s;");
    
    types:
        channel C extends Eth100M {}
        channel GrayC extends Eth100M { @display("ls=lightgray,0.25"); }
    submodules:
        routingRecorder: RoutingTableRecorder {
            @display("p=100,600;is=s");
        }
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,700;is=s;");
        }
        sbahnNetworkGenerator: SbahnNetworkGenerator {
            @display("p=100,800;is=s;i=block/wheelbarrow");
            filename = "sbahn/muenchen_sbahn.txt";
            disable = false;
        }
		carrierAggregation: CarrierAggregation {
            @display("p=100,900;is=s;");
        }
        binder: Binder {
            @display("p=100,1000;is=s;");
        }
        channelControl: LteChannelControl {
            @display("p=100,1100;is=s;");
        }


        upf: Upf {
            @display("p=200,150");
        }
        iUpf: Upf {
            @display("p=150,150");
        }
        gnb1: gNodeB {
            parameters:
                @display("p=500,500;is=l");
        }
        gnb2: gNodeB {
            parameters:
                @display("p=3839,3178;is=l");
        }


        client[nClients]: TumClient5G {
            parameters:
                @display("is=s;i=device/pocketpc");
        }
        train[nTrains]: TumTrain5G;

        serverTrains: TumServerTrains {
        	parameters:
        	    @display("p=50,20;i=device/server");
        }
        serverClients: TumServerClients {
        	parameters:
        	    @display("p=70,20;i=device/server");
        }
        clientRouter: Router {
        	parameters:
        	    @display("p=55,30;i=device/smallrouter");
        }
        trainRouter: Router {
        	parameters:
        	    @display("p=45,30;i=device/smallrouter");
        }
        externalRouter: Router {
            @display("p=45,30;i=device/smallrouter");
        }
            
    connections:        
        //# Data Network connections
        externalRouter.pppg++ <--> C <--> upf.filterGate;

		externalRouter.ethg++ <--> C <--> clientRouter.ethg++;
		externalRouter.ethg++ <--> C <--> trainRouter.ethg++;

        clientRouter.ethg++ <--> C <--> serverClients.ethg++;
        trainRouter.ethg++ <--> C <--> serverTrains.ethg++;

	    //# 5G Core Network connections
        upf.pppg++ <--> C <--> iUpf.pppg++;
        iUpf.pppg++ <--> C <--> gnb1.ppp;
        iUpf.pppg++ <--> C <--> gnb2.ppp;

		//# X2 connections
        gnb1.x2++ <--> GrayC <--> gnb2.x2++;
}
