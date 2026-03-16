//
// Created by sylwe on 10/03/2026.
//

#ifndef EXTREEM_CONFIG_H
#define EXTREEM_CONFIG_H

#include "Makros.h"
#include "utils/Stopwatch.h"


enum SepCr {
    NoCr = 0,
    ArtPointCr,
    BfsCr,
    CompExpCr,
    FlowCr,
    FlowCutterCr,
    FullSepCr
};

enum SepMinim {
    NoMinim = 0,
    BfsMinim,
    ExpansionMinim,
    FlowCutterMinim,
    FlowMinim,
    GNEMinim,
    NeighVCCMinim,
    TotalMinim
};

enum Prepr {
    NoPrepr = 0,
    AllPrepr,
    ArtPointsPrepr,
    IndSet3Prepr,
    IndSet4Prepr,
    PathCompressionPrepr,
    CactusMergingPrepr,
};

enum Pivots {
    NoPivots = 0,
    BlockPivots,
    HallSetPivots,
    AllPivots,
};

class Config {
public:
    void writeBasicInfo();

    static constexpr int inf = 1e9+1;

    string metadata_filepath = "";
    int max_time_millis = inf;
    Stopwatch sw;
    void startMain(){ sw.setLimit("main", max_time_millis); sw.start("main"); }



    int min_graph_size_for_kernelization = 1'000;
    bool minimize_nodes_iteration = false; // minimize nodes vs minimize edges


    double ct_merger_small_size = 4;


    int max_best_seps_for_recursion = 1;
    int max_rec_depth_for_best_seps = 0;


    //************************** General config

    // bool quick_and_weak_tree_creation = false;
    static bool require_balanced_separators;




    //************************** Separator evaluation config

    static double sep_balance;




    //************************** Separator creators config

    int sep_cr_to_use_mask = SepCr::FullSepCr;
    VI sep_cr_iters = {7,5}; // default of 10 iterations for each separator creator
    int sep_cr_max_sources = 15;
    int max_rec_depth_for_flowcutter = 1e9;
    int max_estimated_treedepth_for_flowcutter = 1e9;


    //************************** Separator Minimizers config

    int sep_minim_to_use_mask = SepMinim::TotalMinim;
    int max_separator_size_for_GNE_minimizer = 10'000;
    int max_separator_size_for_flow_minimizer = 10'000;
    int max_best_seps_for_minimizers = 5;


    //************************** Preprocessing config

    int preprocessing_to_use_mask = Prepr::AllPrepr;


    //************************** Pivots config

    int pivots_to_use_mask = Pivots::AllPivots;
    vector<double> pivot_balances_large_graph = { 0.60, 0.45, 0.30, 0.15 };
    vector<double> pivot_balances_small_graph = { 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1 };
    vector<double> pivot_balances = pivot_balances_small_graph;
    bool use_hall_set_pivots_single_pass = false; // use only for small graphs




    void enableOptions(int & mask, int options){ mask |= options; }
    void disableOptions(int & mask, int options){ mask &= ~options; }

};



#endif //EXTREEM_CONFIG_H