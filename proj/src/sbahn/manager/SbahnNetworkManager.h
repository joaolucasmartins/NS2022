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

#ifndef __TUM_SBAHNNETWORKGENERATOR_H_
#define __TUM_SBAHNNETWORKGENERATOR_H_

#include <omnetpp.h>

#include <string>
#include <vector>

#include <inet/common/geometry/common/Coord.h>
#include "../Station.h"


using namespace omnetpp;

/**
 * TODO - Generated class
 */
class SbahnNetworkManager : public cSimpleModule
{
public:
    inet::Coord getStationOutskirtsPos(const Station *s, double minRadius=0, double maxRadius=200);
    inet::Coord getStationOutskirtsPos(double minRadius=0, double maxRadius=200);
    Station getRandomStation();

    class TrainRep {
      public:
        simtime_t stopTime;
        cModule *train;
        TrainRep(simtime_t stopTime, cModule *train): stopTime(stopTime), train(train) {};
    };
    TrainRep getRandomActiveTrain();
    void registerTrainStart(cModule *train, simtime_t stopTime);
    void registerTrainStop(cModule *train);
private:
    cGroupFigure *fGroup;

    std::vector<Station> stations;
    double maxX, maxY;

    std::vector<TrainRep> activeTrains;


    void parseFile(bool addFigs);
    void addStop(int id, int degree, int lat, int lon, const std::string &name);
    void addConnection(int a, int b, int routes);

  protected:
    virtual void initialize() override;
};

#endif
