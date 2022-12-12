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

#ifndef SRC_COMMON_TRAINPACKET_H_
#define SRC_COMMON_TRAINPACKET_H_

#include <inet/common/packet/chunk/FieldsChunk.h>

using namespace inet;
using namespace std;

class TrainSelfPacket : public FieldsChunk {
private:
    int trainId, trackId;
    float lat, lon;
public:
    TrainSelfPacket();
    virtual ~TrainSelfPacket();

    float getLat() const { return lat; }
    void setLat(float lat) { this->lat = lat; }

    float getLon() const { return lon; }
    void setLon(float lon) { this->lon = lon; }

    int getTrackId() const { return trackId; }
    void setTrackId(int trackId) { this->trackId = trackId; }

    int getTrainId() const { return trainId; }
    void setTrainId(int trainId) { this->trainId = trainId; }
};

#endif /* SRC_COMMON_TRAINPACKET_H_ */
