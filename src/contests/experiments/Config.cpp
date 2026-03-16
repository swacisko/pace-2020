//
// Created by sylwe on 10/03/2026.
//

#include "../../../include/contests/experiments/Config.h"

void Config::writeBasicInfo() {
    clog << "Config:" << endl
        << "\t max_time: " << max_time_millis / 1000 << endl
        << "\t metadata_filepath: " << metadata_filepath << endl
        << "\t experiment_name: " << experiment_name << endl
        << "\t run_until_time_limit: " << run_until_time_limit << endl
        << "\t main_repetitions: " << main_repetitions << endl
        << "\t node_scale_factor: " << node_scale_factor << endl
        << "\t min_graph_size_for_kernelization: " << min_graph_size_for_kernelization << endl
        << "\t main_repetitions: " << main_repetitions << endl
        << "\t main_repetitions: " << main_repetitions << endl;

    clog << "\t preprocessing: ";
    for (int i=0; i<30; i++) {
        if (preprocessing_to_use_mask & (1<<i) & (1<<IndSet3Prepr)) clog << "indset-3 ";
        if (preprocessing_to_use_mask & (1<<i) & (1<<IndSet4Prepr)) clog << "indset-4 ";
        if (preprocessing_to_use_mask & (1<<i) & (1<<ArtPointsPrepr)) clog << "art-points ";
        if (preprocessing_to_use_mask & (1<<i) & (1<<Prepr::DanglingTrees)) clog << "dangling-trees ";
    }
    clog << endl;

    clog << "\t creators: ";
    for (int i=0; i<30; i++) {
        if (sep_cr_to_use_mask & (1<<i) & (1<<ArtPointCr)) clog << "art-point ";
        if (sep_cr_to_use_mask & (1<<i) & (1<<BfsCr)) clog << "bfs ";
        if (sep_cr_to_use_mask & (1<<i) & (1<<CompExpCr)) clog << "comp-exp ";
        if (sep_cr_to_use_mask & (1<<i) & (1<<FlowCr)) clog << "flow ";
        if (sep_cr_to_use_mask & (1<<i) & (1<<FlowCutterCr)) clog << "flow-cutter ";
    }
    clog << endl;

    clog << "\t minimizers: ";
    for (int i=0; i<30; i++) {
        if (sep_minim_to_use_mask & (1<<i) & (1<<BfsMinim)) clog << "bfs ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<NeighVCCMinim)) clog << "neigh-vc ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<ExpansionMinim)) clog << "exp ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<FlowMinim)) clog << "flow ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<FlowCutterMinim)) clog << "flow-cutter ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<FlowCutterDstMinim)) clog << "flow-cutter-dst ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<GNEMinim)) clog << "gne ";
    }
    clog << endl;

}

void Config::setPredefinedConfig(int id) {
    if (id == 1) { // fastest and weakest



    }

    if (id == 2) { // trade-off



    }

    if (id == 3) { // slowest, but most robust



    }
}

void Config::disableAll() {
    preprocessing_to_use_mask = Prepr::NoPrepr;
    sep_cr_to_use_mask = NoCr;
    pivots_to_use_mask = NoPivots;
    sep_minim_to_use_mask = NoMinim;
}

double Config::sep_balance = 0.97;
bool Config::require_balanced_separators = true;