#ifndef SRC_COMMON_TRAININFO_H_
#define SRC_COMMON_TRAININFO_H_

#include  <omnetpp/simtime.h>

#include "TrainPacket.h"

using namespace omnetpp;

class TrainInfo {
private:
  int train_id, track_id;
  float lat, lon;
  simtime_t time;
public:
  TrainInfo() {}

  TrainInfo(const TrainPacket *packet) : train_id(packet->getTrainId()), track_id(packet->getTrackId()),
    lat(packet->getLat()), lon(packet->getLon()) {}

  simtime_t getTime() const {
    return time;
  }

  void setTime(simtime_t time) {
    this->time = time;
  }

  int getTrackId() const {
    return track_id;
  }

  void setTrackId(int trackId) {
    track_id = trackId;
  }

  int getTrainId() const {
    return train_id;
  }

  void setTrainId(int trainId) {
    train_id = trainId;
  }

  float getLat() const {
    return lat;
  }

  void setLat(float lat) {
    this->lat = lat;
  }

  float getLon() const {
    return lon;
  }

  void setLon(float lon) {
    this->lon = lon;
  }

  friend ostream& operator<<(ostream& os, const TrainInfo& info) {
      os << '[' << info.train_id << "](" << info.lat << ',' << info.lon << ')';
      return os;
  }
};

#endif /* SRC_COMMON_TRAININFO_H_ */
