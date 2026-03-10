//
// Created by sylwe on 10/03/2026.
//

#include "../../../include/contests/experiments/Exp1.h"
#include <getopt.h>
#include <bits/getopt_core.h>

#include "../../../include/contests/experiments/Config.h"


void Exp1::runPreprocessingExperiments() {
}

void Exp1::runSeparatorCreatorExperiments() {
}

void Exp1::runSeparatorMinimizerExperiments() {
}

void Exp1::runPivotExperiments() {
}

void Exp1::runSeparatorEvaluatorExperiments() {
}

void Exp1::runAllExperiments() {

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
                cnf.max_time_millis = std::stoi(optarg);
                break;
            case METADATA_FILEPATH:
                cnf.metadata_filepath = optarg;

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

    Exp1 exp_runner(cnf);

    return 0;
}
