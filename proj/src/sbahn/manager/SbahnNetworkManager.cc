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

#include "../manager/SbahnNetworkManager.h"

#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>

#include <iostream>

Define_Module(SbahnNetworkManager);

void SbahnNetworkManager::initialize()
{
    parseFile(!par("disable") && hasGUI());
}

void SbahnNetworkManager::parseFile(bool addFigs) {
    std::string filename = par("filename");
    std::ifstream in(filename, std::ios::in);
    if (in.fail())
        throw cRuntimeError("Cannot open file '%s'", filename.c_str());

    int nNodes, nEdges;
    in >> maxX >> maxY >> nNodes;
    stations = std::vector<Station>();
    stations.reserve(nNodes);

    cDisplayString &dp = getParentModule()->getDisplayString();
    dp.setTagArg("bgb", 0, maxX);
    dp.setTagArg("bgb", 1, maxY);

    if (addFigs) {
        fGroup = new cGroupFigure("networkMap");
        getParentModule()->getCanvas()->addFigure(fGroup);
        fGroup->lowerToBottom();

        bool renderHandover = par("handoverLine");
        if (renderHandover) {
            cLineFigure *line = new cLineFigure("handover-line");
            line->setStart(cFigure::Point(maxY, 0));
            line->setEnd(cFigure::Point(0, maxX));
            line->setLineWidth(2);
            line->setEndArrowhead(cFigure::ARROW_NONE);
            fGroup->addFigure(line);
        }

        cGroupFigure *connGroup = new cGroupFigure("connections");
        fGroup->addFigure(connGroup);
    }

    for (int i=0; i<nNodes; ++i) {
        int id, degree, lat, lon;
        std::string name;

        in >> id >> degree >> lon >> lat;
        in.get(); // Consume extra space
        std::getline(in, name);

        stations.emplace_back(id, lon, lat, name);

        if (addFigs)
            addStop(id, degree, lon, lat, name);
    }

    in >> nEdges;
    for (int i=0; i<nEdges; ++i)
{
        int a, b, routes;
        in >> a >> b >> routes;

         if (addFigs)
             addConnection(a, b, routes);
    }
}


void SbahnNetworkManager::addStop(int id, int degree, int lon, int lat, const std::string &name) {
    cFigure::Point p((double) lon, (double) lat);

    auto *icon = new cIconFigure((std::to_string(id)+ "_icon").c_str());
    icon->setImageName("sbahn");
    icon->setSize(16, 16);
    icon->setPosition(p);
    fGroup->addFigure(icon);

    p.y += 18; // TODO: Calculate offset from maxsize

    cLabelFigure *label = new cLabelFigure((std::to_string(id) + "_label").c_str());
    label->setText(name.c_str());
    label->setAlignment(cFigure::ALIGN_CENTER);
    label->setPosition(p);
    label->setAnchor(cFigure::ANCHOR_N);
    label->setFont(cFigure::Font("Courier New"));
    label->setOpacity(0.5);
    fGroup->addFigure(label);
}

void SbahnNetworkManager::addConnection(int a, int b, int routes) {
    auto sa = std::find(stations.begin(), stations.end(), Station(a));
    auto sb = std::find(stations.begin(), stations.end(), Station(b));

    if (sa == stations.end() || sb == stations.end())
        cRuntimeError("[SbahnNetworkGenerator] Connection between non-existant Station(s) not found");

    cLineFigure *line = new cLineFigure("line");
    line->setStart(cFigure::Point(sa->xPos, sa->yPos));
    line->setEnd(cFigure::Point(sb->xPos, sb->yPos));
    line->setLineWidth(2);
    line->setLineOpacity(0.5);
    line->setLineColor(cFigure::Color("#008D4F"));
    line->setLineStyle(cFigure::LineStyle::LINE_DOTTED);
    fGroup->getFigure("connections")->addFigure(line);
}


inet::Coord SbahnNetworkManager::getStationOutskirtsPos(double minRadius, double maxRadius) {
    Station target = getRandomStation();
    return getStationOutskirtsPos(&target, minRadius, maxRadius);
}

Station SbahnNetworkManager::getRandomStation() {
    int idx = intuniform(0, stations.size() - 1);
    return stations.at(idx);
}

inet::Coord SbahnNetworkManager::getStationOutskirtsPos(const Station *target, double minRadius, double maxRadius) {
    double radius = uniform(minRadius, maxRadius);
    double theta = uniform(0, M_2_PI);

    double xPos = target->xPos + radius * std::cos(theta);
    double yPos = target->yPos + radius * std::sin(theta);

    xPos = xPos > 0 ? xPos : 0;
    xPos = xPos < maxX ? xPos : maxX;

    yPos = yPos > 0 ? yPos : 0;
    yPos = yPos < maxY ? yPos : maxY;

    return inet::Coord(xPos, yPos, 0);
}



SbahnNetworkManager::TrainRep SbahnNetworkManager::getRandomActiveTrain() {
    return activeTrains.at(intuniform(0, activeTrains.size() - 1));
}

void SbahnNetworkManager::registerTrainStart(cModule *train, simtime_t stopTime) {
    activeTrains.emplace_back(stopTime, train);
}

void SbahnNetworkManager::registerTrainStop(cModule *train) {
    auto lambda = [train](TrainRep i){ return i.train == train; };
    activeTrains.erase(std::find_if(
            activeTrains.begin(),
            activeTrains.end(),
            lambda
    ));
}













