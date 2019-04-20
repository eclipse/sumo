/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSStoppingPlace.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A lane area vehicles can halt at
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <map>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSNet.h>
#include "MSLane.h"
#include "MSTransportable.h"
#include "MSStoppingPlace.h"

// ===========================================================================
// method definitions
// ===========================================================================
MSStoppingPlace::MSStoppingPlace(const std::string& id,
                                 const std::vector<std::string>& lines,
                                 MSLane& lane,
                                 double begPos, double endPos, const std::string name,
                                 int capacity) :
    Named(id), myLines(lines), myLane(lane),
    myBegPos(begPos), myEndPos(endPos), myLastFreePos(endPos),
    myName(name),
    myTransportableCapacity(capacity) {
    computeLastFreePos();
    for (int i = 0; i < capacity; i++) {
        myWaitingSpots.insert(i);
    }
}


MSStoppingPlace::~MSStoppingPlace() {}


const MSLane&
MSStoppingPlace::getLane() const {
    return myLane;
}


double
MSStoppingPlace::getBeginLanePosition() const {
    return myBegPos;
}


double
MSStoppingPlace::getEndLanePosition() const {
    return myEndPos;
}


void
MSStoppingPlace::enter(SUMOVehicle* what, double beg, double end) {
    myEndPositions[what] = std::pair<double, double>(beg, end);
    computeLastFreePos();
}


double
MSStoppingPlace::getLastFreePos(const SUMOVehicle& forVehicle) const {
    if (myLastFreePos != myEndPos) {
        const double vehGap = forVehicle.getVehicleType().getMinGap();
        double pos = myLastFreePos - vehGap;
        if (!fits(pos, forVehicle)) {
            // try to find a place ahead of the waiting vehicles
            const double vehLength = forVehicle.getVehicleType().getLength();
            std::vector<std::pair<double, std::pair<double, const SUMOVehicle*> > > spaces;
            for (auto it : myEndPositions) {
                spaces.push_back(std::make_pair(it.second.first, std::make_pair(it.second.second, it.first)));
            }
            // sorted from myEndPos towars myBegPos
            std::sort(spaces.begin(), spaces.end());
            std::reverse(spaces.begin(), spaces.end());
            double prev = myEndPos;
            for (auto it : spaces) {
                //if (forVehicle.isSelected()) {
                //    std::cout << SIMTIME << " fitPosFor " << forVehicle.getID() << " l=" << vehLength << " prev=" << prev << " vehBeg=" << it.first << " vehEnd=" << it.second.first << " found=" << (prev - it.first >= vehLength) << "\n";
                //}
                if (prev - it.first + NUMERICAL_EPS >= vehLength && (
                            it.second.second->isParking()
                            || it.second.second->remainingStopDuration() > TIME2STEPS(10))) {
                    return prev;
                }
                prev = it.second.first - vehGap;
            }
        }
        return pos;
    }
    return myLastFreePos;
}

bool
MSStoppingPlace::fits(double pos, const SUMOVehicle& veh) const {
    // always fit at the default position or if at least half the vehicle length
    // is within the stop range (debatable)
    return pos + POSITION_EPS >= myEndPos || (pos - myBegPos >= veh.getVehicleType().getLength() / 2);
}

double
MSStoppingPlace::getWaitingPositionOnLane(MSTransportable* t) const {
    auto it = myWaitingTransportables.find(t);
    if (it != myWaitingTransportables.end() && it->second >= 0) {
        return myEndPos - (0.5 + (it->second) % getPersonsAbreast()) * SUMO_const_waitingPersonWidth;
    } else {
        return (myEndPos + myBegPos) / 2;
    }
}


int
MSStoppingPlace::getPersonsAbreast(double length) {
    return MAX2(1, (int)floor(length / SUMO_const_waitingPersonWidth));
}

int
MSStoppingPlace::getPersonsAbreast() const {
    return getPersonsAbreast(myEndPos - myBegPos);
}

Position
MSStoppingPlace::getWaitPosition(MSTransportable* t) const {
    double lanePos = getWaitingPositionOnLane(t);
    int row = 0;
    auto it = myWaitingTransportables.find(t);
    if (it != myWaitingTransportables.end()) {
        if (it->second >= 0) {
            row = int(it->second / getPersonsAbreast());
        } else {
            // invalid position, draw outside bounds
            row = 1 + myTransportableCapacity / getPersonsAbreast();
        }
    }
    return myLane.getShape().positionAtOffset(myLane.interpolateLanePosToGeometryPos(lanePos),
            myLane.getWidth() / 2 + row * SUMO_const_waitingPersonDepth);
}


double
MSStoppingPlace::getStoppingPosition(const SUMOVehicle* veh) const {
    std::map<const SUMOVehicle*, std::pair<double, double> >::const_iterator i = myEndPositions.find(veh);
    if (i != myEndPositions.end()) {
        return i->second.second;
    } else {
        return getLastFreePos(*veh);
    }
}

bool
MSStoppingPlace::hasSpaceForTransportable() const {
    return myWaitingSpots.size() > 0;
}

bool
MSStoppingPlace::addTransportable(MSTransportable* p) {
    int spot = -1;
    if (!hasSpaceForTransportable()) {
        return false;
    }
    spot = *myWaitingSpots.begin();
    myWaitingSpots.erase(myWaitingSpots.begin());
    myWaitingTransportables[p] = spot;
    return true;
}


void
MSStoppingPlace::removeTransportable(MSTransportable* p) {
    auto i = myWaitingTransportables.find(p);
    if (i != myWaitingTransportables.end()) {
        if (i->second >= 0) {
            myWaitingSpots.insert(i->second);
        }
        myWaitingTransportables.erase(i);
    }
}


void
MSStoppingPlace::leaveFrom(SUMOVehicle* what) {
    assert(myEndPositions.find(what) != myEndPositions.end());
    myEndPositions.erase(myEndPositions.find(what));
    computeLastFreePos();
}


void
MSStoppingPlace::computeLastFreePos() {
    myLastFreePos = myEndPos;
    std::map<const SUMOVehicle*, std::pair<double, double> >::iterator i;
    for (i = myEndPositions.begin(); i != myEndPositions.end(); i++) {
        if (myLastFreePos > (*i).second.second) {
            myLastFreePos = (*i).second.second;
        }
    }
}


double
MSStoppingPlace::getAccessPos(const MSEdge* edge) const {
    if (edge == &myLane.getEdge()) {
        return (myBegPos + myEndPos) / 2.;
    }
    for (const auto& access : myAccessPos) {
        if (edge == &std::get<0>(access)->getEdge()) {
            return std::get<1>(access);
        }
    }
    return -1.;
}


double
MSStoppingPlace::getAccessDistance(const MSEdge* edge) const {
    if (edge == &myLane.getEdge()) {
        return 0.;
    }
    for (const auto& access : myAccessPos) {
        const MSLane* const accLane = std::get<0>(access);
        if (edge == &accLane->getEdge()) {
            const double length = std::get<2>(access);
            if (length >= 0.) {
                return length;
            }
            const Position accPos = accLane->geometryPositionAtOffset(std::get<1>(access));
            const Position stopPos = myLane.geometryPositionAtOffset((myBegPos + myEndPos) / 2.);
            return accPos.distanceTo(stopPos);
        }
    }
    return -1.;
}


const std::string&
MSStoppingPlace::getMyName() const {
    return myName;
}


bool
MSStoppingPlace::addAccess(MSLane* lane, const double pos, const double length) {
    // prevent multiple accesss on the same lane
    for (const auto& access : myAccessPos) {
        if (lane == std::get<0>(access)) {
            return false;
        }
    }
    myAccessPos.push_back(std::make_tuple(lane, pos, length));
    return true;
}


/****************************************************************************/
