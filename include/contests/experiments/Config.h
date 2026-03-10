//
// Created by sylwe on 10/03/2026.
//

#ifndef EXTREEM_CONFIG_H
#define EXTREEM_CONFIG_H

#include "Makros.h"


enum SepCr {
    ArtPointCr,
    ComponentExpansionCr,
    FlowCr,
    NodeAddOrderCr,
    FullCr
};

enum SepMinim {
    BfsMinim,
    ExpansionMinim,
    FlowCutterMinim,
    FlowMinim,
    GNEMinim,
    NeighVCCMinim,
    TotalMinim
};

enum Pivots {
    BlockPivots,
    HallSetPivots,
    AllPivots,
};

class Config {
public:

    string metadata_filepath = "";
    int max_time_millis = 1e9;


    //************************** Separator evaluation config





    //************************** Separator creators config

    SepCr sep_cr_to_use = SepCr::FullCr;
    VI sep_cr_iters = {7,5}; // default of 10 iterations for each separator creator



    //************************** Separator Minimizers config

    SepMinim sep_minim_to_use = SepMinim::TotalMinim;



    //************************** Preprocessing config





    //************************** Pivots config

    Pivots pivots_to_use = Pivots::AllPivots;



};



#endif //EXTREEM_CONFIG_H