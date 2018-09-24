#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../Solver/PbReader.h"
#include "../Solver/GateAssignment.pb.h"


using namespace std;
using namespace pb;

int main(int argc, char *argv[]) {
    enum CheckerFlag {
        IoError = 0x0,
        FormatError = 0x1,
        FlightNotAssignedError = 0x2,
        IncompatibleAssignmentError = 0x4,
        FlightOverlapError = 0x8
    };

    string inputPath;
    string outputPath;

    if (argc > 1) {
        inputPath = argv[1];
    } else {
        cout << "input path: " << flush;
        cin >> inputPath;
    }

    if (argc > 2) {
        outputPath = argv[2];
    } else {
        cout << "output path: " << flush;
        cin >> outputPath;
    }

    pb::GateAssignment::Input input;
    if (!load(inputPath, input)) { return ~CheckerFlag::IoError; }

    pb::GateAssignment::Output output;
    ifstream ifs(outputPath);
    if (!ifs.is_open()) { return ~CheckerFlag::IoError; }
    string submission;
    getline(ifs, submission); // skip the first line.
    ostringstream oss;
    oss << ifs.rdbuf();
    jsonToProtobuf(oss.str(), output);

    int flightNumOnBridge = 0;

    if (output.assignments().size() != input.flights().size()) { return ~CheckerFlag::FormatError; }
    int f = 0;
    for (auto gate = output.assignments().begin(); gate != output.assignments().end(); ++gate, ++f) {
        // check constraints.
        if ((*gate < 0) || (*gate >= input.airport().gates().size())) { return ~CheckerFlag::FlightNotAssignedError; }
        for (auto ig = input.flights(f).incompatiblegates().begin(); ig != input.flights(f).incompatiblegates().end(); ++ig) {
            if (*gate == *ig) { return ~CheckerFlag::IncompatibleAssignmentError; }
        }
        const auto &flight(input.flights(f));
        for (auto flight1 = input.flights().begin(); flight1 != input.flights().end(); ++flight1) {
            if (*gate != output.assignments(flight1->id())) { continue; }
            int gap = max(flight.turnaround().begin() - flight1->turnaround().end(),
                flight1->turnaround().begin() - flight.turnaround().begin());
            if (gap < input.airport().gates(*gate).mingap()) { return ~CheckerFlag::FlightOverlapError; }
        }

        // check objective.
        if (*gate < input.airport().bridgenum()) { ++flightNumOnBridge; }
    }

    return flightNumOnBridge;
}
