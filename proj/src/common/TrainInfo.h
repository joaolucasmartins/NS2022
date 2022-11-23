#ifndef SRC_COMMON_TRAININFO_H_
#define SRC_COMMON_TRAININFO_H_

#include  <omnetpp/simtime.h>

using namespace omnetpp;

class TrainInfo {
private:
  int train_id, track_id;
  simtime_t time;
public:
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
};

#endif /* SRC_COMMON_TRAININFO_H_ */
