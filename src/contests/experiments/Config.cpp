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
        << "\t predefined_config: " << predefined_config_id << endl;

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

    clog << endl;
}

void Config::setPredefinedConfig(int id) {
    if (id == 0) disableAll();

    if (id == 1) { // fastest and weakest
        sep_cr_max_sources = 5;
        pivots_to_use_mask = AllPivots;
        preprocessing_to_use_mask = AllPrepr;
        sep_cr_to_use_mask = FullSepCr;
        sep_minim_to_use_mask = (TotalMinim ^ (1<<FlowCutterDstMinim) ^ (1<<FlowMinim) ^ (1<<FlowCutterMinim));
        main_repetitions = 10;
        max_separator_size_for_GNE_minimizer = max_separator_size_for_flow_minimizer = 500;
        max_rec_depth_for_flowcutter = 5;
        max_estimated_treedepth_for_flowcutter = 500;
        max_best_seps_for_minimizers = 5;
        max_best_seps_for_recursion = 1;
        pivot_balances = pivot_balances_large_graph;
    }

    if (id == 2) { // trade-off
        sep_cr_max_sources = 15;
        pivots_to_use_mask = AllPivots;
        preprocessing_to_use_mask = AllPrepr;
        sep_cr_to_use_mask = FullSepCr;
        sep_minim_to_use_mask = (TotalMinim ^ (1<<FlowCutterDstMinim) ^ (1<<FlowMinim));
        main_repetitions = 15;
        max_separator_size_for_GNE_minimizer = max_separator_size_for_flow_minimizer = 1'000;
        max_rec_depth_for_flowcutter = 10;
        max_estimated_treedepth_for_flowcutter = 1'000;
        max_best_seps_for_minimizers = 5;
        max_best_seps_for_recursion = 1;
        pivot_balances = pivot_balances_large_graph;
    }

    if (id == 3) { // trade-off
        sep_cr_max_sources = 20;
        pivots_to_use_mask = AllPivots;
        preprocessing_to_use_mask = AllPrepr;
        sep_cr_to_use_mask = FullSepCr;
        sep_minim_to_use_mask = TotalMinim;
        main_repetitions = 20;
        max_separator_size_for_GNE_minimizer = max_separator_size_for_flow_minimizer = 3'000;
        max_rec_depth_for_flowcutter = 15;
        max_estimated_treedepth_for_flowcutter = 3'000;
        max_best_seps_for_minimizers = 10;
        max_best_seps_for_recursion = 1;
        pivot_balances = pivot_balances_small_graph;
    }

    if (id == 4) { // slowest, but most robust
        sep_cr_max_sources = 30;
        pivots_to_use_mask = AllPivots;
        preprocessing_to_use_mask = AllPrepr;
        sep_cr_to_use_mask = FullSepCr;
        sep_minim_to_use_mask = TotalMinim;
        main_repetitions = 30;
        max_separator_size_for_GNE_minimizer = max_separator_size_for_flow_minimizer = 10'000;
        max_rec_depth_for_flowcutter = 1e9;
        max_estimated_treedepth_for_flowcutter = 10'000;
        max_best_seps_for_minimizers = sep_cr_max_sources;
        max_best_seps_for_recursion = 4;
        pivot_balances = pivot_balances_full;
    }

    // max_time_millis = 5 * 3600'000; // 1 hour limit
}

void Config::disableAll() {
    // preprocessing_to_use_mask = NoPrepr;
    // sep_cr_to_use_mask = NoCr;
    // pivots_to_use_mask = NoPivots;
    // sep_minim_to_use_mask = NoMinim;
    preprocessing_to_use_mask = 0;
    sep_cr_to_use_mask = 0;
    pivots_to_use_mask = 0;
    sep_minim_to_use_mask = 0;
}

double Config::sep_balance = 0.97;
bool Config::require_balanced_separators = true;