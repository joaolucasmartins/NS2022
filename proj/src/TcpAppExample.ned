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

network TcpAppExample
{
    parameters:
        int n;
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
        client[n]: StandardHost {
            parameters:
                @display("i=device/laptop;p=250,100,col,100");
        }
        server: StandardHost {
            parameters:
                @display("p=400,150;i=device/pc2");
        }
    connections:
        for i=0..n-1 {
            client[i].pppg++ <--> C <--> server.pppg++;
        }
}

