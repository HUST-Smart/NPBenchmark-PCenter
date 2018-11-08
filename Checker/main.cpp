#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Visualizer.h"

#include "ShortestPath.h"
#include "../Solver/PbReader.h"
#include "../Solver/PCenter.pb.h"


using namespace std;
using namespace szx;
using namespace pb;

int main(int argc, char *argv[]) {
    enum CheckerFlag {
        IoError = 0x0,
        TooManyCentersError = 0x1
    };

    string inputPath;
    string outputPath;

    if (argc > 1) {
        inputPath = argv[1];
    } else {
        cerr << "input path: " << flush;
        cin >> inputPath;
    }

    if (argc > 2) {
        outputPath = argv[2];
    } else {
        cerr << "output path: " << flush;
        cin >> outputPath;
    }

    pb::PCenter::Input input;
    if (!load(inputPath, input)) { return ~CheckerFlag::IoError; }

    pb::PCenter::Output output;
    ifstream ifs(outputPath);
    if (!ifs.is_open()) { return ~CheckerFlag::IoError; }
    string submission;
    getline(ifs, submission); // skip the first line.
    ostringstream oss;
    oss << ifs.rdbuf();
    jsonToProtobuf(oss.str(), output);

    int nodeNum = input.graph().nodenum();
    int maxWeight = 0;

    vector<bool> isCenter(nodeNum, false);
    for (int c = 0; c < output.centers().size(); ++c) { isCenter[c] = true; }

    // OPTIMIZE[szx][0]: use floyd on dense graph.
    // OPTIMIZE[szx][0]: skip shortest path finding on geometric graph.
    using Dij = Dijkstra<int, int>;
    Dij::AdjList adjList(nodeNum);
    for (auto e = input.graph().edges().begin(); e != input.graph().edges().end(); ++e) {
        adjList[e->source()].push_back(Dij::AdjNode(e->target(), e->length()));
        adjList[e->target()].push_back(Dij::AdjNode(e->source(), e->length()));
        maxWeight += e->length();
    }

    // check solution.
    int error = 0;
    // check constraints.
    if (output.centers().size() > input.centernum()) { error |= CheckerFlag::TooManyCentersError; }
    // check objective.
    int coverRadius = 0;
    Dij dij(adjList, 0, maxWeight);
    for (int n = 0; n < nodeNum; ++n) {
        dij.reset(n);
        int closestCenter = dij.next([&](int node) { return isCenter[node]; });
        if (coverRadius < dij.getDist(closestCenter)) { coverRadius = dij.getDist(closestCenter); }
    }

    int returnCode = (error == 0) ? coverRadius : ~error;
    cout << returnCode << endl;
    return returnCode;
}
