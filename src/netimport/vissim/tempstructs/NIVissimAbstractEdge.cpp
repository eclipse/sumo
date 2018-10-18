/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimAbstractEdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <map>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBNetBuilder.h>
#include "NIVissimAbstractEdge.h"


NIVissimAbstractEdge::DictType NIVissimAbstractEdge::myDict;

NIVissimAbstractEdge::NIVissimAbstractEdge(int id,
        const PositionVector& geom)
    : myID(id), myNode(-1) {
    // convert/publicate geometry
    for (auto p : geom) {
        if (!NBNetBuilder::transformCoordinate(p)) {
            WRITE_WARNING("Unable to project coordinates for edge '" + toString(id) + "'.");
        }
        myGeom.push_back_noDoublePos(p);
    }
    //
    dictionary(id, this);
}


NIVissimAbstractEdge::~NIVissimAbstractEdge() {}


bool
NIVissimAbstractEdge::dictionary(int id, NIVissimAbstractEdge* e) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        myDict[id] = e;
        return true;
    }
    return false;
}


NIVissimAbstractEdge*
NIVissimAbstractEdge::dictionary(int id) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        return nullptr;
    }
    return (*i).second;
}



Position
NIVissimAbstractEdge::getGeomPosition(double pos) const {
    if (myGeom.length() > pos) {
        return myGeom.positionAtOffset(pos);
    } else if (myGeom.length() == pos) {
        return myGeom[-1];
    } else {
        PositionVector g(myGeom);
        const double amount = pos - myGeom.length();
        g.extrapolate(amount * 2);
        return g.positionAtOffset(pos + amount * 2);
    }
}


void
NIVissimAbstractEdge::splitAndAssignToNodes() {
    for (auto & i : myDict) {
        NIVissimAbstractEdge* e = i.second;
        e->splitAssigning();
    }
}

void
NIVissimAbstractEdge::splitAssigning() {}





bool
NIVissimAbstractEdge::crossesEdge(NIVissimAbstractEdge* c) const {
    return myGeom.intersects(c->myGeom);
}


Position
NIVissimAbstractEdge::crossesEdgeAtPoint(NIVissimAbstractEdge* c) const {
    return myGeom.intersectionPosition2D(c->myGeom);
}


std::vector<int>
NIVissimAbstractEdge::getWithin(const AbstractPoly& p, double offset) {
    std::vector<int> ret;
    for (auto & i : myDict) {
        NIVissimAbstractEdge* e = i.second;
        if (e->overlapsWith(p, offset)) {
            ret.push_back(e->myID);
        }
    }
    return ret;
}


bool
NIVissimAbstractEdge::overlapsWith(const AbstractPoly& p, double offset) const {
    return myGeom.overlapsWith(p, offset);
}


bool
NIVissimAbstractEdge::hasNodeCluster() const {
    return myNode != -1;
}


int
NIVissimAbstractEdge::getID() const {
    return myID;
}

void
NIVissimAbstractEdge::clearDict() {
    for (auto & i : myDict) {
        delete i.second;
    }
    myDict.clear();
}


const PositionVector&
NIVissimAbstractEdge::getGeometry() const {
    return myGeom;
}


void
NIVissimAbstractEdge::addDisturbance(int disturbance) {
    myDisturbances.push_back(disturbance);
}


const std::vector<int>&
NIVissimAbstractEdge::getDisturbances() const {
    return myDisturbances;
}



/****************************************************************************/

