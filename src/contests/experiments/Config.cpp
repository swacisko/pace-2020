//
// Created by sylwe on 10/03/2026.
//

#include "../../../include/contests/experiments/Config.h"

vector<pair<string, string>> Config::getConfigEntries() {
    vector<pair<string, string>> entries;

    entries.emplace_back("max_time", to_string(max_time_millis / 1000));
    entries.emplace_back("metadata_filepath", metadata_filepath);
    entries.emplace_back("experiment_name", experiment_name);
    entries.emplace_back("run_until_time_limit", to_string(run_until_time_limit));
    entries.emplace_back("main_repetitions", to_string(main_repetitions));
    entries.emplace_back("node_scale_factor", to_string(node_scale_factor));
    entries.emplace_back("min_graph_size_for_kernelization", to_string(min_graph_size_for_kernelization));
    entries.emplace_back("predefined_config_id", to_string(predefined_config_id));
    entries.emplace_back("use_init_prepr", to_string(use_init_prepr));
    entries.emplace_back("max_rec_depth_for_flowcutter", to_string(max_rec_depth_for_flowcutter));
    entries.emplace_back("max_estimated_treedepth_for_flowcutter", to_string(max_estimated_treedepth_for_flowcutter));

    stringstream str;
    for (int i=0; i<30; i++) {
        if (preprocessing_to_use_mask & (1<<i) & (1<<IndSet3Prepr)) str << "indset-3 ";
        if (preprocessing_to_use_mask & (1<<i) & (1<<IndSet4Prepr)) str << "indset-4 ";
        // if (preprocessing_to_use_mask & (1<<i) & (1<<ArtPointsPrepr)) str << "art-points ";
        if (preprocessing_to_use_mask & (1<<i) & (1<<Prepr::DanglingTrees)) str << "dangling-trees ";
    }
    entries.emplace_back("preprocessing", str.str());
    str.clear(); str.str("");

    for (int i=0; i<30; i++) {
        if (sep_cr_to_use_mask & (1<<i) & (1<<ArtPointCr)) str << "art-point ";
        if (sep_cr_to_use_mask & (1<<i) & (1<<BfsCr)) str << "bfs ";
        if (sep_cr_to_use_mask & (1<<i) & (1<<CompExpCr)) str << "comp-exp ";
        // if (sep_cr_to_use_mask & (1<<i) & (1<<FlowCr)) str << "flow ";
        if (sep_cr_to_use_mask & (1<<i) & (1<<FlowCutterCr)) str << "flow-cutter ";
    }
    entries.emplace_back("creators", str.str());
    str.clear(); str.str("");

    for (int i=0; i<30; i++) {
        if (sep_minim_to_use_mask & (1<<i) & (1<<BfsMinim)) str << "bfs ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<NeighVCCMinim)) str << "neigh-vc ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<ExpansionMinim)) str << "exp ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<FlowMinim)) str << "flow ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<FlowCutterMinim)) str << "flow-cutter ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<FlowCutterDstMinim)) str << "flow-cutter-dst ";
        if (sep_minim_to_use_mask & (1<<i) & (1<<GNEMinim)) str << "gne ";
    }
    entries.emplace_back("minimizers", str.str());
    str.clear(); str.str("");

    for (int i=0; i<30; i++) {
        if (pivots_to_use_mask & (1<<i) & (1<<BlockPivots)) str << "block ";
        if (pivots_to_use_mask & (1<<i) & (1<<HallSetPivots)) str << "hall-set ";
    }
    entries.emplace_back("pivots", str.str());
    str.clear(); str.str("");

    return entries;
}

void Config::writeBasicInfo() {
    auto entries = Config::getConfigEntries();
    clog << "Config: " << endl;
    for (auto [k,v] : entries) clog << "\t" << k << ": " << v << endl;
}

void Config::setPredefinedConfig(int id) {
    if (id == 0) disableAll();

    if (id == 1) { // fastest and weakest
        sep_cr_max_sources = 5;
        pivots_to_use_mask = (1<<AllPivots)-1;
        preprocessing_to_use_mask = (1<<AllPrepr)-1;
        sep_cr_to_use_mask = (1<<FullSepCr)-1;
        sep_minim_to_use_mask = (((1<<TotalMinim)-1) ^ (1<<FlowCutterDstMinim) ^ (1<<FlowMinim) ^ (1<<FlowCutterMinim));
        main_repetitions = 5;
        max_separator_size_for_GNE_minimizer = max_separator_size_for_flow_minimizer = 500;
        max_rec_depth_for_flowcutter = 5;
        max_estimated_treedepth_for_flowcutter = 500;
        max_best_seps_for_minimizers = 5;
        max_best_seps_for_recursion = 1;
        pivot_balances = pivot_balances_large_graph;
    }

    if (id == 2) { // trade-off
        sep_cr_max_sources = 15;
        pivots_to_use_mask = (1<<AllPivots)-1;
        preprocessing_to_use_mask = (1<<AllPrepr)-1;
        sep_cr_to_use_mask = (1<<FullSepCr)-1;
        sep_minim_to_use_mask = (((1<<TotalMinim)-1) ^ (1<<FlowCutterDstMinim) ^ (1<<FlowMinim));
        main_repetitions = 10;
        max_separator_size_for_GNE_minimizer = max_separator_size_for_flow_minimizer = 1'000;
        max_rec_depth_for_flowcutter = 10;
        max_estimated_treedepth_for_flowcutter = 1'000;
        max_best_seps_for_minimizers = 5;
        max_best_seps_for_recursion = 1;
        pivot_balances = pivot_balances_large_graph;
    }

    if (id == 3) { // trade-off
        sep_cr_max_sources = 20;
        pivots_to_use_mask = (1<<AllPivots)-1;
        preprocessing_to_use_mask = (1<<AllPrepr)-1;
        sep_cr_to_use_mask = (1<<FullSepCr)-1;
        sep_minim_to_use_mask = (1<<TotalMinim)-1;
        main_repetitions = 15;
        max_separator_size_for_GNE_minimizer = max_separator_size_for_flow_minimizer = 3'000;
        max_rec_depth_for_flowcutter = 15;
        max_estimated_treedepth_for_flowcutter = 3'000;
        max_best_seps_for_minimizers = 10;
        max_best_seps_for_recursion = 1;
        pivot_balances = pivot_balances_small_graph;
    }

    if (id == 4) { // slowest, but most robust
        sep_cr_max_sources = 30;
        pivots_to_use_mask = (1<<AllPivots)-1;
        preprocessing_to_use_mask = (1<<AllPrepr)-1;
        sep_cr_to_use_mask = (1<<FullSepCr)-1;
        sep_minim_to_use_mask = (1<<TotalMinim)-1;
        main_repetitions = 30;
        max_separator_size_for_GNE_minimizer = max_separator_size_for_flow_minimizer = 10'000;
        max_rec_depth_for_flowcutter = 1e9;
        max_estimated_treedepth_for_flowcutter = 10'000;
        max_best_seps_for_minimizers = sep_cr_max_sources;
        max_best_seps_for_recursion = 1;
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