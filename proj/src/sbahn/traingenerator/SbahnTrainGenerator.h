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

#ifndef __TUM_SBAHNTRAINGENERATOR_H_
#define __TUM_SBAHNTRAINGENERATOR_H_

#include <omnetpp.h>

#include <string>
#include <fstream>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class SbahnTrainGenerator : public cSimpleModule
{
private:
   std::ifstream trainIn;
   std::string connectionType, trainRouterPattern, portName;

   int nTrains, curTrainIndex;

   const int NEW_TRAIN = 1, DELETE_TRAIN = 2;
   const char* VEC_NAME = "train";

   void createNextTrains();
   void addTrain(cModule *parent, int bonnIndex, int route);
//   void deleteTrain();
protected:
    virtual void initialize(int stages) override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
