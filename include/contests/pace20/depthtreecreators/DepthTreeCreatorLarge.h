/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DEPTHTREECREATORLARGE_H
#define ALGORITHMSPROJECT_DEPTHTREECREATORLARGE_H

#include "DepthTreeCreator.h"

class DepthTreeCreatorLarge : public DepthTreeCreator{
public:
    DepthTreeCreatorLarge(VVI & V, int recurrenceDepth, bool randomSepEval = false);

    DepthTree getDepthTree();

    bool randomSepEval;



    static const int RAND = 0;
    static const int ART_POINTS_CREATOR = 1;
    static const int BFS_CREATOR = 2;
    static const int COMP_EXP_CREATOR = 4;
    static const int TREEWIDTH_CREATOR = 32;
    static const int FLOWCUTTER_CREATOR = 64;

    static const int FLOW_CREATOR = 8;
    static const int CLIQUE_CREATOR = 16;

//    int SEPARATOR_CREATORS_MODE = COMP_EXP_CREATOR/* + ART_POINTS_CREATOR */      + TREEWIDTH_CREATOR      ;
//    int SEPARATOR_CREATORS_MODE = COMP_EXP_CREATOR    /*+ ART_POINTS_CREATOR*/    + FLOWCUTTER_CREATOR      + BFS_CREATOR  /*+ CLIQUE_CREATOR */  /* + TREEWIDTH_CREATOR */     ;
    int SEPARATOR_CREATORS_MODE = COMP_EXP_CREATOR  + ART_POINTS_CREATOR    + FLOWCUTTER_CREATOR      /*+ TREEWIDTH_CREATOR*/      ;
//    int SEPARATOR_CREATORS_MODE = FLOWCUTTER_CREATOR; // for testing and profiling


    bool USE_KERNELIZATION = true;
    bool USE_DEG3_KERNELIZATION = true;
    bool USE_DEG4_KERNELIZATION = false;

    bool PARTITION_SEPARATORS = false;
    bool MINIMIZE_SEPARATORS = true; // if true, then separators will be minimized, otherwise they won't be minimized. It can be used to create quick and qeak decompositions

    void setSeparatorCreatorsMode( int MODE ){ SEPARATOR_CREATORS_MODE = MODE; }

    static void test();

private:
    void testBridgesAndArtPoints();

    void testVCorARTPOINTKernelization();

    Separator testMatchingEdgesContraction();

};


#endif //ALGORITHMSPROJECT_DEPTHTREECREATORLARGE_H
