/*
 * Station.h
 *
 *  Created on: Feb 6, 2023
 *      Author: tiago
 */

#ifndef SRC_SBAHN_STATION_H_
#define SRC_SBAHN_STATION_H_

#include <inet/common/geometry/common/Coord.h>
#include <string>

class Station {
public:
    int id;
    double xPos, yPos;
    std::string name;

    Station(int id, double xPos, double yPos, std::string name): id(id), xPos(xPos), yPos(yPos), name(name) {};
    Station(): id(-1), xPos(0), yPos(0), name("") {};
    inet::Coord getCoord() { return inet::Coord(xPos, yPos, 0); }
};

#endif /* SRC_SBAHN_STATION_H_ */
