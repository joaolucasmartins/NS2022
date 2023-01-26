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
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
package tum.tumclient;


import inet.applications.contract.IApp;

simple TumClientApp5G like IApp {
    parameters:
        string tracksToRequest; // tracks, separated by whitespaces, to request info on
        volatile double thinkTime @unit(s); // time gap between requests
        volatile int numRequests = default(1);  // number of requests sent
        volatile double idleInterval @unit(s); // time gap between sessions
        

        //connection infos
        int deviceLocalPort = default(4000);
        int deviceAppPort = default(4000); // ipAddress of the DeviceApp
        string deviceAppAddress; // port of the DeviceApp
                
        int packetSize = default(10);
     
        //autoscheduling infos
        double startTime @unit("s") = default(0s);
        
        string mecAppName = default("TumClientServerApp"); // TODO Change this
		
        bool logger = default(false);
        string interfaceTableModule;        
        
        // TCP
		int timeToLive = default(-1); // if not -1, set the TTL (IPv4) or Hop Limit (IPv6) field of sent packets to this value
        int dscp = default(-1); // if not -1, set the DSCP (IPv4/IPv6) field of sent packets to this value
        int tos = default(-1); // if not -1, set the Type Of Service (IPv4) / Traffic Class (IPv6) field of sent packets to this value

    gates:
        output socketOut;
        input socketIn;
}

