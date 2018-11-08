#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <random>

#include <cstring>

#include "Simulator.h"
#include "ThreadPool.h"


using namespace std;


namespace szx {

void Simulator::initDefaultEnvironment() {
    Solver::Environment env;
    env.save(Env::DefaultEnvPath());

    Solver::Configuration cfg;
    cfg.save(Env::DefaultCfgPath());
}

void Simulator::run(const Task &task) {
    String instanceName(task.instSet + task.instId + ".json");
    String solutionName(task.instSet + task.instId + ".json");

    char argBuf[Cmd::MaxArgNum][Cmd::MaxArgLen];
    char *argv[Cmd::MaxArgNum];
    for (int i = 0; i < Cmd::MaxArgNum; ++i) { argv[i] = argBuf[i]; }
    strcpy(argv[ArgIndex::ExeName], ProgramName().c_str());

    int argc = ArgIndex::ArgStart;

    strcpy(argv[argc++], Cmd::InstancePathOption().c_str());
    strcpy(argv[argc++], (InstanceDir() + instanceName).c_str());

    System::makeSureDirExist(SolutionDir());
    strcpy(argv[argc++], Cmd::SolutionPathOption().c_str());
    strcpy(argv[argc++], (SolutionDir() + solutionName).c_str());

    if (!task.randSeed.empty()) {
        strcpy(argv[argc++], Cmd::RandSeedOption().c_str());
        strcpy(argv[argc++], task.randSeed.c_str());
    }

    if (!task.timeout.empty()) {
        strcpy(argv[argc++], Cmd::TimeoutOption().c_str());
        strcpy(argv[argc++], task.timeout.c_str());
    }

    if (!task.maxIter.empty()) {
        strcpy(argv[argc++], Cmd::MaxIterOption().c_str());
        strcpy(argv[argc++], task.maxIter.c_str());
    }

    if (!task.jobNum.empty()) {
        strcpy(argv[argc++], Cmd::JobNumOption().c_str());
        strcpy(argv[argc++], task.jobNum.c_str());
    }

    if (!task.runId.empty()) {
        strcpy(argv[argc++], Cmd::RunIdOption().c_str());
        strcpy(argv[argc++], task.runId.c_str());
    }

    if (!task.cfgPath.empty()) {
        strcpy(argv[argc++], Cmd::ConfigPathOption().c_str());
        strcpy(argv[argc++], task.cfgPath.c_str());
    }

    if (!task.logPath.empty()) {
        strcpy(argv[argc++], Cmd::LogPathOption().c_str());
        strcpy(argv[argc++], task.logPath.c_str());
    }

    Cmd::run(argc, argv);
}

void Simulator::run(const String &envPath) {
    char argBuf[Cmd::MaxArgNum][Cmd::MaxArgLen];
    char *argv[Cmd::MaxArgNum];
    for (int i = 0; i < Cmd::MaxArgNum; ++i) { argv[i] = argBuf[i]; }
    strcpy(argv[ArgIndex::ExeName], ProgramName().c_str());

    int argc = ArgIndex::ArgStart;

    strcpy(argv[argc++], Cmd::EnvironmentPathOption().c_str());
    strcpy(argv[argc++], envPath.c_str());

    Cmd::run(argc, argv);
}

void Simulator::debug() {
    Task task;
    task.instSet = "";
    task.instId = "pmed1.n100e198p5";
    task.randSeed = "1500972793";
    //task.randSeed = to_string(RandSeed::generate());
    task.timeout = "180";
    //task.maxIter = "1000000000";
    task.jobNum = "1";
    task.cfgPath = Env::DefaultCfgPath();
    task.logPath = Env::DefaultLogPath();
    task.runId = "0";

    run(task);
}

void Simulator::benchmark(int repeat) {
    Task task;
    task.instSet = "";
    //task.timeout = "180";
    //task.maxIter = "1000000000";
    task.timeout = "3600";
    //task.maxIter = "1000000000";
    task.jobNum = "1";
    task.cfgPath = Env::DefaultCfgPath();
    task.logPath = Env::DefaultLogPath();

    random_device rd;
    mt19937 rgen(rd());
    // EXTEND[szx][5]: read it from InstanceList.txt.
    vector<String> instList({
        "pmed1.n100e198p5",
        "pmed2.n100e193p10",
        "pmed3.n100e198p10",
        "pmed4.n100e196p20",
        "pmed5.n100e196p33",
        "pmed6.n200e786p5",
        "pmed7.n200e779p10",
        "pmed8.n200e792p20",
        "pmed9.n200e785p40",
        "pmed10.n200e786p67",
        "pmed11.n300e1772p5",
        "pmed12.n300e1758p10",
        "pmed13.n300e1760p30",
        "pmed14.n300e1771p60",
        "pmed15.n300e1754p100",
        "pmed16.n400e3153p5",
        "pmed17.n400e3142p10",
        "pmed18.n400e3134p40",
        "pmed19.n400e3134p80",
        "pmed20.n400e3144p133",
        "pmed21.n500e4909p5",
        "pmed22.n500e4896p10",
        "pmed23.n500e4903p50",
        "pmed24.n500e4914p100",
        "pmed25.n500e4894p167",
        "pmed26.n600e7069p5",
        "pmed27.n600e7072p10",
        "pmed28.n600e7054p60",
        "pmed29.n600e7042p120",
        "pmed30.n600e7042p200",
        "pmed31.n700e9601p5",
        "pmed32.n700e9584p10",
        "pmed33.n700e9616p70",
        "pmed34.n700e9585p140",
        "pmed35.n800e12548p5",
        "pmed36.n800e12560p10",
        "pmed37.n800e12564p80",
        "pmed38.n900e15898p5",
        "pmed39.n900e15896p10",
        "pmed40.n900e15879p90"
    });
    for (int i = 0; i < repeat; ++i) {
        //shuffle(instList.begin(), instList.end(), rgen);
        for (auto inst = instList.begin(); inst != instList.end(); ++inst) {
            task.instId = *inst;
            task.randSeed = to_string(Random::generateSeed());
            task.runId = to_string(i);
            run(task);
        }
    }
}

void Simulator::parallelBenchmark(int repeat) {
    Task task;
    task.instSet = "";
    //task.timeout = "180";
    //task.maxIter = "1000000000";
    task.timeout = "3600";
    //task.maxIter = "1000000000";
    task.jobNum = "1";
    task.cfgPath = Env::DefaultCfgPath();
    task.logPath = Env::DefaultLogPath();

    ThreadPool<> tp(4);

    random_device rd;
    mt19937 rgen(rd());
    // EXTEND[szx][5]: read it from InstanceList.txt.
    vector<String> instList({
        "pmed1.n100e198p5",
        "pmed2.n100e193p10",
        "pmed3.n100e198p10",
        "pmed4.n100e196p20",
        "pmed5.n100e196p33",
        "pmed6.n200e786p5",
        "pmed7.n200e779p10",
        "pmed8.n200e792p20",
        "pmed9.n200e785p40",
        "pmed10.n200e786p67",
        "pmed11.n300e1772p5",
        "pmed12.n300e1758p10",
        "pmed13.n300e1760p30",
        "pmed14.n300e1771p60",
        "pmed15.n300e1754p100",
        "pmed16.n400e3153p5",
        "pmed17.n400e3142p10",
        "pmed18.n400e3134p40",
        "pmed19.n400e3134p80",
        "pmed20.n400e3144p133",
        "pmed21.n500e4909p5",
        "pmed22.n500e4896p10",
        "pmed23.n500e4903p50",
        "pmed24.n500e4914p100",
        "pmed25.n500e4894p167",
        "pmed26.n600e7069p5",
        "pmed27.n600e7072p10",
        "pmed28.n600e7054p60",
        "pmed29.n600e7042p120",
        "pmed30.n600e7042p200",
        "pmed31.n700e9601p5",
        "pmed32.n700e9584p10",
        "pmed33.n700e9616p70",
        "pmed34.n700e9585p140",
        "pmed35.n800e12548p5",
        "pmed36.n800e12560p10",
        "pmed37.n800e12564p80",
        "pmed38.n900e15898p5",
        "pmed39.n900e15896p10",
        "pmed40.n900e15879p90"
    });
    for (int i = 0; i < repeat; ++i) {
        //shuffle(instList.begin(), instList.end(), rgen);
        for (auto inst = instList.begin(); inst != instList.end(); ++inst) {
            task.instId = *inst;
            task.randSeed = to_string(Random::generateSeed());
            task.runId = to_string(i);
            tp.push([=]() { run(task); });
            this_thread::sleep_for(1s);
        }
    }
}

void Simulator::generateInstance(const InstanceTrait &trait) {
    Random rand;

    Problem::Input input;

    // EXTEND[szx][5]: generate random instances.

    ostringstream path;
    path << InstanceDir() << "rand.n" << input.graph().nodenum()
        << "p" << input.centernum() << ".json";
    save(path.str(), input);
}

void Simulator::convertPmedInstance(const String &pmedPath, int index) {
    Log(Log::Info) << "converting pmed" << index << endl;

    ifstream ifs(pmedPath);

    int nodeNum, edgeNum, centerNum;
    ifs >> nodeNum >> edgeNum >> centerNum;

    Arr2D<int> edgeIndices(nodeNum, nodeNum, -1);

    Problem::Input input;
    input.set_centernum(centerNum);

    auto &graph(*input.mutable_graph());
    graph.set_nodenum(nodeNum);
    for (int e = 0; e < edgeNum; ++e) {
        int source, target, length;
        ifs >> source >> target >> length;
        --source;
        --target;

        if (source > target) { swap(source, target); }

        if (edgeIndices.at(source, target) < 0) {
            edgeIndices.at(source, target) = graph.edges().size();
            auto &edge(*graph.add_edges());
            edge.set_source(source);
            edge.set_target(target);
            edge.set_length(length);
        } else {
            Log(Log::Warning) << "duplicated edge " << source << "-" << target << ", overwrite length " << graph.edges(edgeIndices.at(source, target)).length() << "->" << length << endl;
            graph.mutable_edges(edgeIndices.at(source, target))->set_length(length);
        }
    }

    ostringstream path;
    path << InstanceDir() << "pmed" << index << ".n" << input.graph().nodenum()
        << "e" << input.graph().edges().size() << "p" << input.centernum() << ".json";
    save(path.str(), input);
}

}
