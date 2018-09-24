#include <iostream>
#include <string>

#include "../Solver/PbReader.h"
#include "../Solver/GateAssignment.pb.h"


using namespace std;
using namespace szx;

int main(int argc, char *argv[]) {
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
    load(inputPath, input);

    pb::GateAssignment::Output output;
    load(outputPath, output);


    // TODO[szx][0]: finish the checker.


    return -1;
}
