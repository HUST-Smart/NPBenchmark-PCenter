#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

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
        FormatError = 0x1,
        TooManyCentersError = 0x2
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

    if (output.centers().size() <= 0) { return ~CheckerFlag::FormatError; }
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
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    Dij dij(adjList, 0, maxWeight);
    cerr << "dijkstra init takes " << (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() / 1000.0) << " seconds" << endl;
    for (int n = 0; n < nodeNum; ++n) {
        if (isCenter[n]) { continue; }
        dij.reset(n);
        int closestCenter = dij.next([&](int node) { return isCenter[node]; });
        if (coverRadius < dij.getDist(closestCenter)) { coverRadius = dij.getDist(closestCenter); }
    }
    cerr << "dijkstra takes " << (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() / 1000.0) << " seconds" << endl;

    int returnCode = (error == 0) ? coverRadius : ~error;
    cout << returnCode << endl;
    return returnCode;
}
