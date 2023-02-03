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

#include "SbahnNetworkGenerator.h"

#include <fstream>
#include <sstream>
#include <random>
#include <math.h>

#include <iostream>

Define_Module(SbahnNetworkGenerator);

void SbahnNetworkGenerator::initialize()
{
    parseFile(!par("disable") && hasGUI());
}

void SbahnNetworkGenerator::parseFile(bool addFigs) {
    std::string filename = par("filename");
    std::ifstream in(filename, std::ios::in);
    if (in.fail())
        throw cRuntimeError("Cannot open file '%s'", filename.c_str());

    int nNodes, nEdges;
    in >> maxY >> maxX >> nNodes;
    stations.reserve(nNodes);

    cDisplayString &dp = getParentModule()->getDisplayString();
    dp.setTagArg("bgb", 0, maxY);
    dp.setTagArg("bgb", 1, maxX);

    if (addFigs) {
        fGroup = new cGroupFigure("networkMap");
        getParentModule()->getCanvas()->addFigure(fGroup);
        fGroup->lowerToBottom();

        bool renderHandover = par("handoverLine");
        if (renderHandover) {
            cLineFigure *line = new cLineFigure("handover-line");
            line->setStart(cFigure::Point(maxX,0));
            line->setEnd(cFigure::Point(0,maxY));
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
        std::getline(in, name);

        stations.push_back(SbahnNetworkGenerator::Station(id, lon, lat, name));

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

    stopsX.clear();
    stopsY.clear();
}


void SbahnNetworkGenerator::addStop(int id, int degree, int lon, int lat, const std::string &name) {
    cFigure::Point p((double) lon, (double) lat);

    auto *icon = new cIconFigure((std::to_string(id)+ "_icon").c_str());
    icon->setImageName("sbahn");
    icon->setSize(16, 16);
    icon->setPosition(p);
    fGroup->addFigure(icon);

    stopsX[id] = p.x;
    stopsY[id] = p.y;

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

void SbahnNetworkGenerator::addConnection(int a, int b, int routes) {
    cLineFigure *line = new cLineFigure("line");
    line->setStart(cFigure::Point(stopsX[a], stopsY[a]));
    line->setEnd(cFigure::Point(stopsX[b], stopsY[b]));
    line->setLineWidth(2);
    line->setLineOpacity(0.5);
    line->setLineColor(cFigure::Color("#008D4F"));
    line->setLineStyle(cFigure::LineStyle::LINE_DOTTED);
    fGroup->getFigure("connections")->addFigure(line);
}


inet::Coord SbahnNetworkGenerator::getStationOutskirtsPos(double minRadius, double maxRadius) {
    Station target = getRandomStation();
    return getStationOutskirtsPos(&target, minRadius, maxRadius);
}

SbahnNetworkGenerator::Station SbahnNetworkGenerator::getRandomStation() {
    int idx = std::rand() % stations.size();
    return stations.at(idx);
}

inet::Coord SbahnNetworkGenerator::getStationOutskirtsPos(const Station *target, double minRadius, double maxRadius) {
    double radius = uniform(minRadius, maxRadius);
    double theta = uniform(0, M_2_PI);

    double xPos = target->xPos + radius * std::cos(theta);
    double yPos = target->yPos + radius * std::sin(theta);

    xPos = xPos > 0 ? xPos : 0;
    xPos = xPos < maxX ? xPos : maxX;

    yPos = yPos > 0 ? yPos : 0;
    yPos = yPos < maxY ? yPos : maxY;

//    std::cout << "Center x: " << target->xPos << " y: " << target->yPos << "name: " << target->name << std::endl;
//    std::cout << "Radius: " << radius << " " << " x: " << xPos << " y: " << yPos << std::endl << std::endl;

    return inet::Coord(xPos, yPos, 0);
}

