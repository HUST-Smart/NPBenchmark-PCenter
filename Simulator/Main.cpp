#include "Simulator.h"


using namespace std;
using namespace szx;


int main() {
    //Simulator::initDefaultEnvironment();

    Simulator sim;
    //sim.debug();
    sim.benchmark(1);
    //sim.parallelBenchmark(1);
    //sim.generateInstance();
    //for (int i = 1; i <= 40; ++i) { sim.convertPmedInstance("Instance/pmed/pmed" + to_string(i) + ".txt", i); }

    return 0;
}
