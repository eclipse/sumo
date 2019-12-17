/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSOverheadWire.h
/// @author  Jakub Sevcik (RICE)
/// @author  Jan Prikryl (RICE)
/// @date    2019-11-25
///
// Overhead wires for Electric (equipped with elecHybrid device) vehicles (Overhead wire segments, overhead wire sections, traction substations)
/****************************************************************************/
#ifndef MSOverheadWire_h
#define MSOverheadWire_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <microsim/MSStoppingPlace.h>
#include <utils/common/Named.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/common/WrappingCommand.h>
#include <utils/traction_wire/Circuit.h>

// Resistivity of Cu is 1.69*10^-8 Ohm*m. A cross-section S of the overhead wire used in Pilsen is 150 mm^2. So the "resistivity/S" is 0.000113 Ohm/m. 
const double WIRE_RESISTIVITY = (double)2 * 0.000113;

// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSBusStop;
class OptionsCont;
class MSDevice_ElecHybrid;
class MSTractionSubstation;
class Named;


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class MSOverheadWire
* @brief Definition of overhead wire segment
*/

class MSOverheadWire : public MSStoppingPlace {
public:

    /// @brief constructor
    MSOverheadWire(const std::string& overheadWireSegmentID, MSLane& lane, double startPos, double endPos,
        bool voltageSource);

    /// @brief destructor
    ~MSOverheadWire();

    /// @brief Get overhead wire's voltage
    double getVoltage() const;

    /// @brief Set overhead wire's voltage
    void setVoltage(double voltage);

    /// @brief enable or disable charging vehicle
    void setChargingVehicle(bool value);

    /** @brief Check if a vehicle is inside in  the Charge Station
    * @param[in] position Position of vehicle in the LANE
    * @return true if is between StartPostion and EndPostion
    */
    bool vehicleIsInside(const double position) const;

    /// @brief Return true if in the current time step charging station is charging a vehicle
    bool isCharging() const;

    void addVehicle(SUMOVehicle& veh);

    void eraseVehicle(SUMOVehicle& veh);

    int getElecHybridCount() const {
        return (int)myChargingVehicles.size();
    }

    double getTotalCharged() const {
        return myTotalCharge;
    }

    /// @brief add charge value for output
    void addChargeValueForOutput(double WCharged, MSDevice_ElecHybrid* elecHybrid, bool ischarging = 1);

    /// @brief write charging station values
    void writeOverheadWireSegmentOutput(OutputDevice& output);

    std::string getOverheadWireSegmentName();

    MSTractionSubstation* getTractionSubstation() const {
        return myTractionSubstation;
    }

    void setTractionSubstation(MSTractionSubstation* substation) {
        myTractionSubstation = substation;
    }

    Circuit* getCircuit() const;

    void setCircuitStartNode_pos(Node* node) {
        myCircuitStartNode_pos = node;
    }

    void setCircuitEndNode_pos(Node* node) {
        myCircuitEndNode_pos = node;
    }

    void setCircuitElement_pos(Element* element) {
        myCircuitElement_pos = element;
    }

    Node* getCircuitStartNode_pos() {
        return myCircuitStartNode_pos;
    }

    Node* getCircuitEndNode_pos() {
        return myCircuitEndNode_pos;
    }

    Element* getCircuitElement_pos() {
        return myCircuitElement_pos;
    }

    bool isThereVoltageSource() {
        return myVoltageSource;
    }

    MSTractionSubstation* getTractionSubstation() {
        return myTractionSubstation;
    }

protected:

    /// @brief struct to save information for the overhead wire segment output
    struct charge {
        /// @brief constructor
        charge(SUMOTime _timeStep, std::string _vehicleID, std::string _vehicleType, std::string _status,
            double _WCharged, double _actualBatteryCapacity, double _maxBatteryCapacity, double _voltage,
            double _totalEnergyCharged) :
            timeStep(_timeStep),
            vehicleID(_vehicleID),
            vehicleType(_vehicleType),
            status(_status),
            WCharged(_WCharged),
            actualBatteryCapacity(_actualBatteryCapacity),
            maxBatteryCapacity(_maxBatteryCapacity),
            voltage(_voltage),
            totalEnergyCharged(_totalEnergyCharged) {}

        // @brief vehicle TimeStep
        SUMOTime timeStep;
        // @brief vehicle ID
        std::string vehicleID;
        // @brief vehicle Type
        std::string vehicleType;
        /// @brief status
        std::string status;
        // @brief W charged
        double WCharged;
        // @brief actual battery capacity AFTER charging
        double actualBatteryCapacity;
        // @brief battery max capacity
        double maxBatteryCapacity;
        // @brief current charging power of charging station
        double voltage;
        // @brief current efficiency of charging station
        double chargingEfficiency;
        // @brief current energy charged by charging stations AFTER charging
        double totalEnergyCharged;

         
            
    };

    /** @brief A class for sorting vehicle on lane under the overhead wire segment */
    class vehicle_position_sorter {
    public:
        /// @brief Constructor
        explicit vehicle_position_sorter() { }

        /// @brief Sorting function; compares RODFRouteDesc::distance2Last
        int operator()(SUMOVehicle* v1, SUMOVehicle* v2) {
            return v1->getPositionOnLane() > v2->getPositionOnLane();
        }
    };

    /// @brief Overhead wire's voltage
    double myVoltage;

    /// @brief Check if in the current TimeStep overheadWireSegment is charging a vehicle
    bool myChargingVehicle;

    /// @brief total energy charged by this charging station
    double myTotalCharge;

    /// @brief vector with the charges of this charging station
    std::vector<charge> myChargeValues;

    std::vector<SUMOVehicle*> myChargingVehicles;

    /// @brief Parameter, Pointer to the electrical substation (by default is nullptr)
    MSTractionSubstation* myTractionSubstation;

    bool myVoltageSource;

    Element* myCircuitElement_pos;
    Node* myCircuitStartNode_pos;
    Node* myCircuitEndNode_pos;

private:
    /// @brief Invalidated copy constructor.
    MSOverheadWire(const MSOverheadWire&);

    /// @brief Invalidated assignment operator.
    MSOverheadWire& operator=(const MSOverheadWire&);	
};


class MSTractionSubstation : public Named {
public:

    /// @brief constructor
    MSTractionSubstation(const std::string& substationId, double voltage, double currentLimit);

    /// @brief destructor
    ~MSTractionSubstation();

    Circuit* getCircuit() const {
        return myCircuit;
    }
    void addOverheadWireSegmentToCircuit(MSOverheadWire* newOverheadWireSegment);

    void addOverheadWireClampToCircuit(const std::string id, MSOverheadWire* startSegment, MSOverheadWire* endSegment);

    void eraseOverheadWireSegmentFromCircuit(MSOverheadWire* oldWireSegment);
    void writeOut();
    std::size_t numberOfOverheadSegments() const {
        return myOverheadWireSegments.size();
    }

    /// @brief enable or disable charging vehicle
    void setChargingVehicle(bool value);

    /// @brief Return true if in the current time step the substation (overhead wire section) is charging a vehicle
    bool isCharging() const;

    void increaseElecHybridCount();

    void decreaseElecHybridCount();

    void addForbiddenLane(MSLane* lane);

    bool isForbidden(const MSLane* lane);

    void addClamp(const std::string& id, MSOverheadWire* startPos, MSOverheadWire* endPos);

    int getElecHybridCount() const {
        return myElecHybridCount;
    }

    void addVehicle(MSDevice_ElecHybrid* elecHybrid);

    void eraseVehicle(MSDevice_ElecHybrid* elecHybrid);

    double getSubstationVoltage() const {
        return mySubstationVoltage;
    }

    bool isAnySectionPreviouslyDefined();

    void addSolvingCirucitToEndOfTimestepEvents();
    SUMOTime solveCircuit(SUMOTime currentTime);

private:
    void addOverheadWireInnerSegmentToCircuit(MSOverheadWire* incomingSegment, MSOverheadWire* outgoingSegment, const MSLane* connection, const MSLane* frontConnection, const MSLane* behindConnection);

private:
    double mySubstationVoltage;
    double myCurrentLimit;

protected:
    /// @brief Check if in the current TimeStep substation (overhead wire section) is charging a vehicle
    bool myChargingVehicle;
    int myElecHybridCount;

private:
    std::vector<MSOverheadWire*> myOverheadWireSegments;
    std::vector<MSDevice_ElecHybrid*> myElecHybrid;
    Circuit* myCircuit;
    std::vector<MSLane*> myForbiddenLanes;
    static Command* myCommandForSolvingCircuit;

public:   
    //preparation of overhead wire clamp
    struct OverheadWireClamp{
        // provide default constructor for vector construction below
        OverheadWireClamp() :
            id("undefined"),
            start(nullptr),
            end(nullptr),
            usage(false) {}

        OverheadWireClamp(const std::string _id, MSOverheadWire* _start, MSOverheadWire* _end, bool _usage ):
            id(_id),
            start(_start),
            end(_end),
            usage(_usage) {}

        const std::string id;
        MSOverheadWire* start;
        MSOverheadWire* end; 
        bool usage;

        OverheadWireClamp& operator=(const OverheadWireClamp&) = delete;
    };

private:
    std::vector<OverheadWireClamp> myOverheadWireClamps;

public:
    OverheadWireClamp* findClamp(std::string id);
    //bool findClamp(std::string id);
};
#endif
