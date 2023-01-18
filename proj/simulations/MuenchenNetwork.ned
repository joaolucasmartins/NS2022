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
import tum.tumclient.TumClient5G;
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
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s;");
        }
        sbahnNetworkGenerator: SbahnNetworkGenerator {
            @display("p=100,200;is=s;i=block/wheelbarrow");
            filename = "sbahn/muenchen_sbahn.txt";
            disable = false;
        }

        channelControl: LteChannelControl {
            @display("p=100,300;is=s;");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=100,400;is=s;");
        }
        binder: Binder {
            @display("p=100,500;is=s;");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=100,600;is=s");
        }
        upf: Upf {
            @display("p=300,300");
        }
        upf2: Upf {
            @display("p=300,1500");
        }
        iUpf: Upf {
            @display("p=1500,300");
        }
        gnb1: gNodeB {
            parameters:
                @display("is=l;r=5000");
        }
        gnb2: gNodeB {
            parameters:
                @display("is=l;r=5000");
        }

        client[nClients]: TumClient5G {
            parameters:
                @display("is=vs;i=device/pocketpc");
        }
        train[nTrains]: TumTrain5G;
        server: TumServer {
            parameters:
                @display("p=50,20;i=device/server");
        }
//        clientRouter: Router {
//            parameters:
//                @display("p=55,30;i=device/smallrouter");
//        }
        trainRouter: Router {
            parameters:
                @display("p=45,30;i=device/smallrouter");
        }
        mecHost: MECHost {
            @display("p=352.92624,145.62;is=l");
        }

        mecOrchestrator: MecOrchestrator {
            @display("p=544.0525,63.70875;is=l");
        }

        ualcmp: UALCMP {
            @display("p=545.0638,158.76625;i=device/smallrouter");
        }
    connections:
        //# Data Network connections
        //        for i=0..nClients-1 {
        //            client[i].ethg++ <--> C <--> clientRouter.ethg++;
        //        }
        trainRouter.pppg++ <--> Eth10G <--> upf.filterGate;

//        clientRouter.ethg++ <--> C <--> server.ethg++;
        trainRouter.ethg++ <--> C <--> server.ethg++;

	    //# 5G Core Network connections
        upf.pppg++ <--> Eth10G <--> iUpf.pppg++;
        upf2.pppg++ <--> Eth10G <--> iUpf.pppg++;
        iUpf.pppg++ <--> Eth10G <--> gnb1.ppp;
        iUpf.pppg++ <--> Eth10G <--> gnb2.ppp;

		//# X2 connections
        gnb1.x2++ <--> Eth10G <--> gnb2.x2++;
        
        //# MEC stuff
        ualcmp.ppp++ <--> Eth10G <--> upf2.filterGate;

        ualcmp.toMecOrchestrator --> mecOrchestrator.fromUALCMP;
        ualcmp.fromMecOrchestrator <-- mecOrchestrator.toUALCMP;

        mecHost.ppp++ <--> Eth10G <--> iUpf.pppg++;
}
