/*
 * ClientResponsePacket.h
 *
 *  Created on: Nov 23, 2022
 *      Author: jm
 */

#ifndef SRC_COMMON_CLIENTRESPONSEPACKET_H_
#define SRC_COMMON_CLIENTRESPONSEPACKET_H_
#include <map>
#include <vector>
#include <inet/common/packet/chunk/FieldsChunk.h>
#include "TrainInfo.h"

using namespace inet;
using namespace std;

class ClientResponsePacket : public FieldsChunk {
private:
  map<int, vector<TrainInfo>> trackInfo;
public:
  ClientResponsePacket(map<int, vector<TrainInfo>> trackInfo) : trackInfo(trackInfo) {}

  map<int, vector<TrainInfo> > getTrackInfo() const {
    return trackInfo;
  }
};

#endif /* SRC_COMMON_CLIENTRESPONSEPACKET_H_ */
