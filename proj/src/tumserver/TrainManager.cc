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

#include  <omnetpp/simtime.h>

#include "TrainManager.h"

Define_Module(TrainManager);

void TrainManager::initialize()
{
    trains = map<int, map<int, TrainInfo>>();
}

void TrainManager::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void TrainManager::updateTrainInfo(TrainInfo &trainInfo) {
    const int trackId = trainInfo.getTrackId();
    if (trains.find(trackId) == trains.end())
        trains.insert({trackId, map<int, TrainInfo>()});

    trainInfo.setTime(simTime());
    map<int, TrainInfo> track = trains.at(trackId);
    const int trainId = trainInfo.getTrainId();
    if (track.find(trainId) != track.end()) {
        track.erase(trainId);
    }
    track.insert({trainId, trainInfo});
}

bool TrainManager::hasTrackInfo(int trackId) const {
    return trains.find(trackId) != trains.end();
}

const vector<TrainInfo> TrainManager::getTrackInfo(int trackId) {
    vector<TrainInfo> v;
    if (!hasTrackInfo(trackId))
        return v;

    map<int, TrainInfo> track = trains.at(trackId);
    for(auto it = track.begin(); it != track.end(); ++it ) {
        v.push_back( it->second );
    }

    return v;
}

