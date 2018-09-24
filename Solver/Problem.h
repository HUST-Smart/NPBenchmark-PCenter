////////////////////////////////
/// usage : 1.	data that identifies a guillotine cut problem and its solution.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SMART_SZX_GATE_ASSIGNMENT_PROBLEM_H
#define SMART_SZX_GATE_ASSIGNMENT_PROBLEM_H


#include "Config.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Common.h"
#include "PbReader.h"
#include "GateAssignment.pb.h"


namespace szx {

class Problem {
    #pragma region Type
public:
    using Input = pb::GateAssignment::Input;

    struct Output : public pb::GateAssignment::Output {
    public:
        bool save(const String &path) const { return saveOutput(path, *this); }

        ID flightNumOnBridge = 0;
    };
    #pragma endregion Type

    #pragma region Constant
public:
    enum {
        MaxGateNum = 100,
        MaxBridgeNum = 30,
        MaxFlightNum = 400,

        InvalidId = -1,
    };
    #pragma endregion Constant

    #pragma region Constructor
public:
    #pragma endregion Constructor

    #pragma region Method
public:
    static bool loadInput(const String &path, Input &input) { return load(path, input); }
    static bool saveOutput(const String &path, const Output &output) { return save(path, output); }
    #pragma endregion Method

    #pragma region Field
public:
    #pragma endregion Field
}; // Problem

}


#endif // SMART_SZX_GATE_ASSIGNMENT_PROBLEM_H
