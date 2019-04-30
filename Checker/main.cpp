#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits>

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

    static constexpr double GeometricalGraphObjScale = 100;

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

    if (output.centers().size() <= 0) { return ~CheckerFlag::FormatError; }

    int nodeNum = input.graph().nodenum();

    // check solution.
    int error = 0;
    // check constraints.
    if (output.centers().size() > input.centernum()) { error |= CheckerFlag::TooManyCentersError; }
    // check objective.
    int coverRadius = 0;
    double objScale = 1;
    if (input.graph().nodes().empty()) { // topological graph.
        int maxWeight = 0;
        using Dij = Dijkstra<int, int>; // OPTIMIZE[szx][0]: use floyd on dense graph.
        Dij::AdjList adjList(nodeNum);
        for (auto e = input.graph().edges().begin(); e != input.graph().edges().end(); ++e) {
            adjList[e->source()].push_back(Dij::AdjNode(e->target(), e->length()));
            adjList[e->target()].push_back(Dij::AdjNode(e->source(), e->length()));
            maxWeight += e->length(); // OPTIMIZE[szx][1]: find better upperbound estimation for the longest path.
        }

        vector<bool> isCenter(nodeNum, false);
        for (auto c = output.centers().begin(); c != output.centers().end(); ++c) { isCenter[*c] = true; }

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
    } else { // geometrical graph.
        double minX = numeric_limits<double>::max();
        double maxX = numeric_limits<double>::min();
        double minY = numeric_limits<double>::max();
        double maxY = numeric_limits<double>::min();

        objScale = GeometricalGraphObjScale;
        for (int n = 0; n < nodeNum; ++n) {
            double nx = input.graph().nodes(n).x();
            double ny = input.graph().nodes(n).y();
            int shortestDist = numeric_limits<int>::max();
            for (auto c = output.centers().begin(); c != output.centers().end(); ++c) { 
                int dist = lround(objScale * hypot(
                    nx - input.graph().nodes(*c).x(), ny - input.graph().nodes(*c).y()));
                if (dist < shortestDist) { shortestDist = dist; }
            }
            if (coverRadius < shortestDist) { coverRadius = shortestDist; }

            minX = min(minX, nx); maxX = max(maxX, nx); minY = min(minY, ny); maxY = max(maxY, ny);
        }

        // visualization.
        auto pos = outputPath.find_last_of('/');
        string outputName = (pos == string::npos) ? outputPath : outputPath.substr(pos + 1);
        constexpr double Margin = 10;
        double nodeSize = max(1.0, (maxX - minX) / 640);
        Drawer draw;
        draw.begin("Visualization/" + outputName + ".html", minX - Margin, minY - Margin, maxX - minX + 2 * Margin, maxY - minY + 2 * Margin);
        for (int n = 0; n < nodeNum; ++n) {
            draw.circle(input.graph().nodes(n).x(), input.graph().nodes(n).y(), nodeSize, "000");
        }
        double r = coverRadius / objScale;
        for (auto c = output.centers().begin(); c != output.centers().end(); ++c) {
            draw.circle(input.graph().nodes(*c).x(), input.graph().nodes(*c).y(), 2 * nodeSize, "F00");
            draw.circle(input.graph().nodes(*c).x(), input.graph().nodes(*c).y(), r);
        }
        draw.end();
    }

    int returnCode = (error == 0) ? coverRadius : ~error;
    cout << ((error == 0) ? (coverRadius / objScale) : returnCode) << endl;
    return returnCode;
}
