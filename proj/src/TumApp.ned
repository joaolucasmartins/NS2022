//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//
//

package src;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import ned.DatarateChannel;


module TumClient extends StandardHost {
}
module TumTrain extends StandardHost {
}

module TumServer {
    submodules:
        udpServer: StandardHost {
        }
        tcpServer: StandardHost {
        }
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
                @display("i=device/laptop;p=550,100,col,100");
        }
        server: TumServer {
        	parameters:
        	    @display("p=400,150;i=device/pc2");
        }
        clientServer: StandardHost {
            parameters:
                @display("p=375,150;i=device/pc2");
        }
        trainServer: StandardHost {
            parameters:
                @display("p=425,150;i=device/pc2");
        }
    connections:
        for i=0..nClients-1 {
            client[i].pppg++ <--> C <--> clientServer.pppg++;
        }
        for i=0..nTrains-1 {
            train[i].pppg++ <--> C <--> trainServer.pppg++;
        }
}

