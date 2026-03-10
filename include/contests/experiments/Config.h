//
// Created by sylwe on 10/03/2026.
//

#ifndef EXTREEM_CONFIG_H
#define EXTREEM_CONFIG_H

#include "Makros.h"


enum SepCr {
    ArtPointCr = 1,
    ComponentExpansionCr,
    FlowCr,
    NodeAddOrderCr,
    FullCr
};

enum SepMinim {
    BfsMinim = 1,
    ExpansionMinim,
    FlowCutterMinim,
    FlowMinim,
    GNEMinim,
    NeighVCCMinim,
    TotalMinim
};

enum Prepr {
    AllPrepr = 1,
    ArtPoints,
    IndSet3,
    IndSet4,
    PathCompression,
    CactusMerging,
};

enum Pivots {
    BlockPivots = 1,
    HallSetPivots,
    AllPivots,
};

class Config {
public:
    static constexpr int inf = 1e9+1;

    string metadata_filepath = "";
    int max_time_millis = inf;



    int min_graph_size_for_kernelization = 1'000;
    bool minimize_nodes_iteration = false;


    double ct_merger_small_size = 4;


    int max_best_seps_for_recursion = 1;
    int max_rec_depth_for_best_seps = 0;


    //************************** General config

    bool quick_and_weak_tree_creation = false;
    bool require_balanced_separators = true;




    //************************** Separator evaluation config

    double sep_eval_balance = 0.97;




    //************************** Separator creators config

    int sep_cr_to_use_mask = SepCr::FullCr;
    VI sep_cr_iters = {7,5}; // default of 10 iterations for each separator creator
    int sep_cr_max_sources = 7;


    //************************** Separator Minimizers config

    int sep_minim_to_use_mask = SepMinim::TotalMinim;
    int max_separator_size_for_GNE_minimizer = inf;
    int max_separator_size_for_flow_minimizer = inf;
    int max_best_seps_for_minimizers = 5;


    //************************** Preprocessing config

    bool preprocessing_to_use_mask = Prepr::AllPrepr;
    bool use_preprocessing = (preprocessing_to_use_mask != 0);


    //************************** Pivots config

    int pivots_to_use_mask = Pivots::AllPivots;



};



#endif //EXTREEM_CONFIG_H