//
// Created by sylwe on 10/03/2026.
//

#include "../../../include/contests/experiments/Config.h"

void Config::writeBasicInfo() {
    clog << "Config:" << endl
        << "\t max_time: " << max_time_millis / 1000 << endl
        << "\t metadata_filepath: " << metadata_filepath << endl;
}

double Config::sep_balance = 0.97;
bool Config::require_balanced_separators = true;