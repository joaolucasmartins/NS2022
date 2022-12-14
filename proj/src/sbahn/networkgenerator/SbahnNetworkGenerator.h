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
#include <map>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class SbahnNetworkGenerator : public cSimpleModule
{
private:
    std::map<int, double> stopsX;
    std::map<int, double> stopsY;
    cGroupFigure *fGroup;

    void parseFile(bool addFigs);
    void addStop(int id, int degree, int lat, int lon, const std::string &name);
    void addConnection(int a, int b, int routes);
  protected:
    virtual void initialize() override;
};

#endif
