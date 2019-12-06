/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSEmissionExport.cpp
/// @author  Jakub Sevcik (RICE)
/// @author  Jan Prikryl (RICE)
/// @date    2019-11-25
/// @version $Id$
///
// Realises dumping Electric hybrid vehicle data
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/devices/MSDevice_ElecHybrid.h>
#include "MSElecHybridExport.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
MSElecHybridExport::writeAggregated(OutputDevice& of, SUMOTime timestep, int precision) {
    of.openTag(SUMO_TAG_TIMESTEP).writeAttr(SUMO_ATTR_TIME, time2string(timestep));
    of.setPrecision(precision);

    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {

        const SUMOVehicle* veh = it->second;
        const MSVehicle* microVeh = dynamic_cast<const MSVehicle*>(veh);

        if (!veh->isOnRoad()) {
            continue;
        }

        std::string fclass = veh->getVehicleType().getID();
        fclass = fclass.substr(0, fclass.find_first_of("@"));

        if (static_cast<MSDevice_ElecHybrid*>(veh->getDevice(typeid(MSDevice_ElecHybrid))) != 0) {
            MSDevice_ElecHybrid* elecHybridToExport = dynamic_cast<MSDevice_ElecHybrid*>(veh->getDevice(typeid(MSDevice_ElecHybrid)));
            if (elecHybridToExport->getMaximumBatteryCapacity() > 0) {
                // Open Row
                of.openTag(SUMO_TAG_VEHICLE);
                // Write ID
                of.writeAttr(SUMO_ATTR_ID, veh->getID());

                // Write Maximum battery capacity
                of.writeAttr(SUMO_ATTR_MAXIMUMBATTERYCAPACITY, elecHybridToExport->getMaximumBatteryCapacity());
                // Write Actual battery capacity
                of.writeAttr(SUMO_ATTR_ACTUALBATTERYCAPACITY, elecHybridToExport->getActualBatteryCapacity());

                // Write consumed energy [Wh] (computed by HelpersEnergy::compute)
                of.writeAttr(SUMO_ATTR_ENERGYCONSUMED, elecHybridToExport->getConsum());
                // Write Energy charged in the Battery [Wh] (drawn energy from overhead wire minus consumed energy)
                of.writeAttr(SUMO_ATTR_ENERGYCHARGED, elecHybridToExport->getEnergyCharged());
                // Write Power demand (requsted from overhed wire) [W]
                of.writeAttr(SUMO_ATTR_CHARGINGPOWER, elecHybridToExport->getPowerWanted());

                // Write OverheadWire Segment ID
                of.writeAttr(SUMO_ATTR_OVERHEADWIREID, elecHybridToExport->getOverheadWireSegmentID());
                // Write Traction Substation ID
                of.writeAttr(SUMO_ATTR_TRACTIONSUBSTATIONID, elecHybridToExport->getTractionSubstationID());

                // Write current from overheadwire
                of.writeAttr(SUMO_ATTR_CURRENTFROMOVERHEADWIRE, elecHybridToExport->getCurrentFromOverheadWire());
                // Write voltage of overheadwire
                of.writeAttr(SUMO_ATTR_VOLTAGEOFOVERHEADWIRE, elecHybridToExport->getVoltageOfOverheadWire());
                // Write circuit alpha best (1 if the traction substation is not overloaded, number from interval [0,1) if the traction substation is overloaded, NAN if it is not applicable)
                of.writeAttr(SUMO_ATTR_ALPHACIRCUITSOLVER, elecHybridToExport->getCircuitAlpha());

                // Write Speed
                of.writeAttr(SUMO_ATTR_SPEED, veh->getSpeed());
                // Write Acceleration
                of.writeAttr(SUMO_ATTR_ACCELERATION, veh->getAcceleration());
                // Write Distance
                double distance;
                if (veh->getLane()->isInternal()) {
                    // route edge still points to the edge before the intersection
                    const double normalEnd = (*veh->getCurrentRouteEdge())->getLength();
                    distance = (veh->getRoute().getDistanceBetween(veh->getDepartPos(), normalEnd,
                        veh->getRoute().begin(), veh->getCurrentRouteEdge())
                        + veh->getRoute().getDistanceBetween(normalEnd, veh->getPositionOnLane(),
                        *veh->getCurrentRouteEdge(), &veh->getLane()->getEdge()));
                }
                else {
                    distance = veh->getRoute().getDistanceBetween(veh->getDepartPos(), veh->getPositionOnLane(),
                        veh->getRoute().begin(), veh->getCurrentRouteEdge());
                }
                of.writeAttr(SUMO_ATTR_DISTANCE, distance);
                // Write pos x
                of.writeAttr(SUMO_ATTR_X, veh->getPosition().x());
                // Write pos y
                of.writeAttr(SUMO_ATTR_Y, veh->getPosition().y());
                // Write pos z
                of.writeAttr(SUMO_ATTR_Z, veh->getPosition().z());
                // Write slope
                of.writeAttr(SUMO_ATTR_SLOPE, veh->getSlope());
                if (microVeh != 0) {
                    // Write Lane ID
                    of.writeAttr(SUMO_ATTR_LANE, microVeh->getLane()->getID());
                }
                // Write vehicle position in the lane
                of.writeAttr(SUMO_ATTR_POSONLANE, veh->getPositionOnLane());
                // Close Row
                of.closeTag();
            }
        }
    }
    of.closeTag();
}


void
MSElecHybridExport::write(OutputDevice& of, const SUMOVehicle* veh, SUMOTime timestep, int precision) {
    of.openTag(SUMO_TAG_TIMESTEP).writeAttr(SUMO_ATTR_TIME, time2string(timestep));
    of.setPrecision(precision);

    if (!veh->isOnRoad()) {
        return;
    }

    const MSVehicle* microVeh = dynamic_cast<const MSVehicle*>(veh);

    if (static_cast<MSDevice_ElecHybrid*>(veh->getDevice(typeid(MSDevice_ElecHybrid))) != nullptr) {
        MSDevice_ElecHybrid* elecHybridToExport = dynamic_cast<MSDevice_ElecHybrid*>(veh->getDevice(typeid(MSDevice_ElecHybrid)));
        // Write Actual battery capacity
        of.writeAttr(SUMO_ATTR_ACTUALBATTERYCAPACITY, elecHybridToExport->getActualBatteryCapacity());

        // Write consumed energy [Wh] (computed by HelpersEnergy::compute)
        of.writeAttr(SUMO_ATTR_ENERGYCONSUMED, elecHybridToExport->getConsum());
        // Write Energy charged in the Battery [Wh] (drawn energy from overhead wire minus consumed energy)
        of.writeAttr(SUMO_ATTR_ENERGYCHARGED, elecHybridToExport->getEnergyCharged());
        // Write Power demand (requsted from overhed wire) [W]
        of.writeAttr(SUMO_ATTR_CHARGINGPOWER, elecHybridToExport->getPowerWanted());

        // Write OverheadWire Segment ID
        of.writeAttr(SUMO_ATTR_OVERHEADWIREID, elecHybridToExport->getOverheadWireSegmentID());
        // Write Traction Substation ID
        of.writeAttr(SUMO_ATTR_TRACTIONSUBSTATIONID, elecHybridToExport->getTractionSubstationID());
        
        // Write current from overheadwire
        of.writeAttr(SUMO_ATTR_CURRENTFROMOVERHEADWIRE, elecHybridToExport->getCurrentFromOverheadWire());
        // Write voltage of overheadwire
        of.writeAttr(SUMO_ATTR_VOLTAGEOFOVERHEADWIRE, elecHybridToExport->getVoltageOfOverheadWire());
        // Write circuit alpha best (1 if the traction substation is not overloaded, number from interval [0,1) if the traction substation is overloaded, NAN if it is not applicable)
        of.writeAttr(SUMO_ATTR_ALPHACIRCUITSOLVER, elecHybridToExport->getCircuitAlpha());

        // Write Speed
        of.writeAttr(SUMO_ATTR_SPEED, veh->getSpeed());
        // Write Acceleration
        of.writeAttr(SUMO_ATTR_ACCELERATION, veh->getAcceleration());
        // Write Distance
        double distance;
        if (veh->getLane()->isInternal()) {
            // route edge still points to the edge before the intersection
            const double normalEnd = (*veh->getCurrentRouteEdge())->getLength();
            distance = (veh->getRoute().getDistanceBetween(veh->getDepartPos(), normalEnd,
                veh->getRoute().begin(), veh->getCurrentRouteEdge())
                + veh->getRoute().getDistanceBetween(normalEnd, veh->getPositionOnLane(),
                *veh->getCurrentRouteEdge(), &veh->getLane()->getEdge()));
        }
        else {
            distance = veh->getRoute().getDistanceBetween(veh->getDepartPos(), veh->getPositionOnLane(),
                veh->getRoute().begin(), veh->getCurrentRouteEdge());
        }
        of.writeAttr(SUMO_ATTR_DISTANCE, distance);
        // Write pos x
        of.writeAttr(SUMO_ATTR_X, veh->getPosition().x());
        // Write pos y
        of.writeAttr(SUMO_ATTR_Y, veh->getPosition().y());
        // Write pos z
        of.writeAttr(SUMO_ATTR_Z, veh->getPosition().z());
        // Write slope
        of.writeAttr(SUMO_ATTR_SLOPE, veh->getSlope());
        // Write Lane ID
        if (microVeh != 0) {
            of.writeAttr(SUMO_ATTR_LANE, microVeh->getLane()->getID());
        }
        // Write vehicle position in the lane
        of.writeAttr(SUMO_ATTR_POSONLANE, veh->getPositionOnLane());
    }
    of.closeTag();
}
