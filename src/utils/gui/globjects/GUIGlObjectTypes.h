/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIGlObjectTypes.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A list of object types which may be displayed within the gui
// each type has an associated string which will be prefefixed to an object id
// when constructing the full name
/****************************************************************************/
#ifndef GUIGlObjectTypes_h
#define GUIGlObjectTypes_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// definitions
// ===========================================================================
/**
 * ChooseableArtifact
 * Enumeration to differ to show the list of which artefact
 * The order is important during the selection of items for displaying their
 * tooltips; the item with the hightest type value will be chosen.
 */
enum GUIGlObjectType {
    /// @brief The network - empty
    GLO_NETWORK = 0,

    /// @brief reserved GLO type to pack all netElements
    GLO_NETELEMENT = 1,
    /// @brief an edge
    GLO_EDGE = 2,
    /// @brief a lane
    GLO_LANE = 3,
    /// @brief a junction
    GLO_JUNCTION = 4,
    /// @brief a tl-logic
    GLO_CROSSING = 5,
    /// @brief a connection
    GLO_CONNECTION = 6,
    /// @brief a tl-logic
    GLO_TLLOGIC = 8,

    /// @brief reserved GLO type to pack all additionals
    GLO_ADDITIONAL = 100,
    /// @brief a busStop
    GLO_BUS_STOP = 101,
    /// @brief a containerStop
    GLO_CONTAINER_STOP = 102,
    /// @brief a chargingStation
    GLO_CHARGING_STATION = 103,
    /// @brief a ParkingArea
    GLO_PARKING_AREA = 104,
    /// @brief a ParkingSpace
    GLO_PARKING_SPACE = 105,
    /// @brief a E1 detector
    GLO_E1DETECTOR = 106,
    /// @brief a E1 detector
    GLO_E1DETECTOR_ME = 107,
    /// @brief a E1 detector
    GLO_E1DETECTOR_INSTANT = 108,
    /// @brief a E2 detector
    GLO_E2DETECTOR = 109,
    /// @brief a E3 detector
    GLO_E3DETECTOR = 110,
    /// @brief a DetEntry detector
    GLO_DET_ENTRY = 111,
    /// @brief a DetExit detector
    GLO_DET_EXIT = 112,
    /// @brief a Rerouter
    GLO_REROUTER = 113,
    /// @brief a Rerouter
    GLO_REROUTER_EDGE = 114,
    /// @brief a Variable Speed Sign
    GLO_VSS = 115,
    /// @brief a Calibrator
    GLO_CALIBRATOR = 116,
    /// @brief a RouteProbe
    GLO_ROUTEPROBE = 117,
    /// @brief a Vaporizer
    GLO_VAPORIZER = 118,
    /// @brief a Acces
    GLO_ACCESS = 119,
    /// @brief a TAZ
    GLO_TAZ = 120,
    /// @brief a segment of an overhead line 
    GLO_OVERHEAD_WIRE_SEGMENT = 121,

    /// @brief reserved GLO type to pack shapes
    GLO_SHAPE = 200,
    /// @brief a polygon
    GLO_POLYGON = 201,
    /// @brief a poi
    GLO_POI = 202,

    /// @brief reserved GLO type to pack all RouteElements (note: In this case the sorting of GLO_<element> is important!)
    GLO_ROUTEELEMENT = 300,
    GLO_VTYPE = 301,

    /// @name routes
    /// @{
    /// @brief a route
    GLO_ROUTE = 310,
    /// @brief a embedded route
    GLO_EMBEDDEDROUTE = 311,
    /// @}

    /// @name Person plans
    /// @{
    /// @brief a ride
    GLO_RIDE = 320,
    /// @brief a walk
    GLO_WALK = 321,
    /// @brief a person trip
    GLO_PERSONTRIP = 322,
    /// @}

    /// @name stops
    /// @{
    /// @brief a stop
    GLO_STOP = 330,
    /// @brief a person stop
    GLO_PERSONSTOP = 331,
    /// @}

    /// @name vehicles
    /// @{
    /// @brief a vehicle
    GLO_VEHICLE = 340,
    /// @brief a trip
    GLO_TRIP = 341,
    /// @brief a flow
    GLO_FLOW = 342,
    /// @brief a routeFlow
    GLO_ROUTEFLOW = 343,
    /// @}

    /// @name containers (carried by vehicles)
    /// @{
    /// @brief a container
    GLO_CONTAINER = 350,
    /// @}

    /// @name persons
    /// @{
    /// @brief a person
    GLO_PERSON = 360,
    /// @brief a person flow
    GLO_PERSONFLOW = 361,
    /// @}

    /// @brief empty max
    GLO_MAX = 2048
};

#endif

/****************************************************************************/
