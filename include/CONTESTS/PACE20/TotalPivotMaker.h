/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_TOTALPIVOTMAKER_H
#define ALGORITHMSPROJECT_TOTALPIVOTMAKER_H

#include <graphs/GraphInducer.h>
#include "DepthTree.h"

class TotalPivotMaker{
public:
    TotalPivotMaker( DepthTree& dt, int recDepth );

    /**
     *
     * @return besttree after all pivots
     */
    DepthTree makePivots();

    static void test();

private:
    int recDepth;
    void logSpacing(){ for(int i=0; i<recDepth; i++) cerr << "    "; }

    int MAX_DEPTH = 3;
    int N;
    VVI* V;
    DepthTree dt;
    VB inCmp; // auxiliary vector for quick checks


    struct InducedSubtreeData{
        InducedSubtreeData( DepthTree & t, InducedGraph& g ) : tree( t ){
            this->g = g;
        }
        DepthTree tree;
        InducedGraph g;
    };

    /**
     *
     * @return vector containing induced graphs and their corresponding induced depth trees for a subgraph induced by all nodes in subtree of @{tree} with given @{root}
     */
    vector<InducedSubtreeData> getInducedSubgraphs( VVI & tree, vector<DepthTree::StretchStructureNode> & data, int root );
};

#endif //ALGORITHMSPROJECT_TOTALPIVOTMAKER_H
