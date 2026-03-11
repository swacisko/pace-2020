//
// Created by sylwe on 10/03/2026.
//

#include "../../../include/contests/experiments/Exp1.h"
#include <getopt.h>
#include <bits/getopt_core.h>

#include "GraphReader.h"
#include "../../../include/contests/experiments/Config.h"


void Exp1::runPreprocessingExperiments() {
    clog << "Exp1 -> running preprocessing experiments" << endl;


}

void Exp1::runSeparatorCreatorExperiments() {
    clog << "Exp1 -> running separator creator experiments" << endl;


}

void Exp1::runSeparatorMinimizerExperiments() {
    clog << "Exp1 -> running separator minimizer experiments" << endl;


}

void Exp1::runPivotExperiments() {
    clog << "Exp1 -> running pivot experiments" << endl;


}

void Exp1::runSeparatorEvaluatorExperiments() {
    clog << "Exp1 -> running separator evaluator experiments" << endl;


}

void Exp1::runAllExperiments() {
    clog << "Exp1 -> running all experiments" << endl;


}






Config parse_arguments(int argc, char* argv[]) {
    Config cnf;

    enum {
        OPT_TIME = 1,
        METADATA_FILEPATH = 2,
    };


    const struct option long_options[] = {
        {"time", required_argument, nullptr, OPT_TIME},
        {"mtd", required_argument, nullptr,METADATA_FILEPATH},
        {nullptr, 0, nullptr, 0}
    };

    int option_index = 0;
    int opt;

    while ((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {

        switch (opt) {

            case OPT_TIME:
                cnf.max_time_millis = 1000 * std::stoi(optarg);
                break;
            case METADATA_FILEPATH:
                cnf.metadata_filepath = optarg;
                break;

            case '?':
            default:
                std::cerr << "Usage: " << argv[0] << " [--time milliseconds]\n";
                exit(EXIT_FAILURE);
        }
    }

    return cnf;
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout << fixed;

    Config cnf = parse_arguments(argc, argv);


    clog << "Hello, running experiments!" << endl;
    // auto V = GraphReader::readGraphStandardEdges(cin);
    auto V = GraphReader::readGraphDIMACSWunweighed(cin);


    // running experiments
    Exp1 exp_runner(V,cnf);
    exp_runner.runAllExperiments();


    // writing data to the cnf.metadata_filepath file
    auto data = exp_runner.data;
    ofstream f(cnf.metadata_filepath);
    data.writeData(f);
    f.close();

    return 0;
}
