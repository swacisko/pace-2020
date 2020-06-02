/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DEPTHTREEMERGER_H
#define ALGORITHMSPROJECT_DEPTHTREEMERGER_H

#include "DepthTree.h"
#include "Separator.h"

/**
 * Given separator S and components of graph G \ S and their DepthTrees we want to find order of nodes in S in which to add them to DepthTree and merge all trees into one tree.
 *
 */
class ComponentTreeMerger{
public:

    ComponentTreeMerger(VVI & V, Separator& sep, VVI & comps, vector<DepthTree>& subtrees);

    /**
     *  CAUTION!! Function modifies subtrees passed in constructor!!
     *
     * @param sep seperator of graph G
     * @param comps connected components of graph G \ S
     * @param subtrees subtrees[i] is a depth tree for connected component comps[i]. It must be given AFTER REMAPPING from induced subgraph to the original graph.
     * @param MERGE_TYPE is type of merge used. If ALL_ORDERS, than all permutations of comps will be considered. If largest first, comps will be added in orderd of nonincreasing DepthTree height.
     * @return DepthTree for given
     */
    DepthTree mergeComponents( int MERGE_TYPE = AUTOMATIC, int reps = 24 );

    /**
     *
     * @param mergeOrder
     * @return height of the tree for given order of adding subtrees to the tree
     */
    int getDTHeightForOrder( VI mergeOrder );

    /**
     *
     * @return order of subtrees to add
     */
    VI getMergeOrder();

    /**
     * Creates sepGraph and mapper to enable 'quickly' checking different merge orders.
     */
    void createSepGraph();

    /**
     *
     * @param mergeOrder
     * @return DepthTree that is a result of adding subtrees in given order
     */
    DepthTree mergeForOrder( VI mergeOrder );


    static void test();


    VVI* V;
    Separator* sep;
    VVI *comps;
    vector<DepthTree> *subtrees;



    VVI sepGraph;
    VI mapper; // mapper[i] is indej j such that vertex j in sepGraph represents component containing j

    int MERGE_TYPE;
    int RANDOM_REPS;
    static const int LARGEST_FIRST = 0;
    static const int ALL_ORDERS = 1;
    static const int RANDOM_PERMUTATIONS = 2;
    static const int AUTOMATIC = 3;

};

#endif //ALGORITHMSPROJECT_DEPTHTREEMERGER_H
