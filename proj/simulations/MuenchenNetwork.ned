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

import tum.tumclient.TumClient;
import tum.tumtrain.TumTrain;
import tum.tumtrain.TumTrain5G;
import tum.tumserver.TumServer;

import tum.sbahn.networkgenerator.SbahnNetworkGenerator;
import tum.sbahn.traingenerator.SbahnTrainGenerator;

import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.Upf;
import simu5g.nodes.NR.gNodeB;
import simu5g.world.radio.LteChannelControl;
import inet.networklayer.ipv4.RoutingTableRecorder;

import simu5g.nodes.mec.MECHost;
import simu5g.nodes.mec.MECOrchestrator.MecOrchestrator;
import simu5g.nodes.mec.UALCMP.UALCMP;

network MuenchenNetwork
{
    parameters:
        int nClients;
        int nTrains;
        @display("bgi=maps/muenchen,s;");

    types:
        channel C extends Eth100M
        {
        }
        channel TC extends Eth100M
        {
            @display("ls=lightgray,0.25");
        }
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



        mecHost1: MECHost {
            @display("p=500,600;is=l");
        }
        mecHost2: MECHost {
            @display("p=3839,3078;is=l");
        }
        iUpf1: Upf {
            @display("p=500,500");
        }
        iUpf2: Upf {
            @display("p=3839,3178");
        }
        gnb1: gNodeB {
            parameters:
                @display("p=0,0;is=l;r=5000,,blue");
        }
        gnb2: gNodeB {
            parameters:
                @display("p=4339,3678;is=l;r=5000,,blue");
        }



        client[nClients]: TumClient {
            parameters:
                @display("is=vs;i=device/pocketpc");
        }
        train[nTrains]: TumTrain5G;
        server: TumServer {
            parameters:
                @display("p=2200,1430;i=device/server");
        }


        upfRouter: Router {
            parameters:
                @display("p=2400,1630;i=device/smallrouter");
        }
        trainRouter: Router {
            parameters:
                @display("p=2200,1630;i=device/smallrouter");
        }

        upf: Upf {
            @display("p=2400,1830");
        }
        mecOrchestrator: MecOrchestrator {
            @display("p=2599,1430;is=l");
        }
        ualcmp: UALCMP {
            @display("p=2600,1630");
        }
    connections:
        upfRouter.pppg++ <--> Eth10G <--> upf.filterGate;
        trainRouter.ethg++ <--> C <--> server.ethg++;
        upfRouter.pppg++ <--> Eth10G <--> trainRouter.pppg++;

	    //# 5G Core Network connections
        upf.pppg++ <--> Eth10G <--> iUpf1.pppg++;
        upf.pppg++ <--> Eth10G <--> iUpf2.pppg++;
        iUpf1.pppg++ <--> Eth10G <--> gnb1.ppp;
        iUpf2.pppg++ <--> Eth10G <--> gnb2.ppp;

		//# X2 connections
        gnb1.x2++ <--> Eth10G <--> gnb2.x2++;

        //# MEC stuff
        ualcmp.ppp++ <--> Eth10G <--> upfRouter.pppg++;

        ualcmp.toMecOrchestrator --> mecOrchestrator.fromUALCMP;
        ualcmp.fromMecOrchestrator <-- mecOrchestrator.toUALCMP;

        mecHost1.ppp++ <--> Eth10G <--> iUpf1.pppg++;
        mecHost2.ppp++ <--> Eth10G <--> iUpf2.pppg++;
}
