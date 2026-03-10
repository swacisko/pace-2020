/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_GREEDYNODEMINIMIZER_H
#define ALGORITHMSPROJECT_GREEDYNODEMINIMIZER_H

//#include "../Separator.h"
#include "SeparatorMinimizer.h"

class GreedyNodeEdgeMinimizer : public SeparatorMinimizer{

public:

    GreedyNodeEdgeMinimizer( int minimizationType = GreedyNodeEdgeMinimizer::MINIMIZE_EDGES ) : minimizationType(minimizationType){
        sepEval = nullptr;
    }


    /**
     * Function minimizes separator, but first it creates sepGraph, nodeWeights and edgeWeight for that graph. That is done in O(E) time
     * @param sep
     */
    Separator minimizeSeparator(Separator sep);


    /**
     * Function removes nodes from given separator, until any further removal results in obtaining non-balanced separator.
     * Order of removal is given by some heuristic (by default it is greedily selecting node that minimizes total size of neighboring connected components).
     * @param sep
     * @param sepGraph first sep.nodes.size() nodes in sepGraph should be separator nodes. Other nodes represent connected components of G \ S.
     * @param nodeWeights should denote number of nodes in given component (1 if node is in separator)
     * @param edgeWeights should denote edges in given component (or 0 if node is in separator)
     * @return minimized separator
     *
     */
    Separator minimizeSeparator( Separator sep, VVPII & sepGraph, VI nodeWeights, VI edgeWeights );


    /**
     * Converts separator graph to simple graph gr and weights in grW. This enables to quickly update data
     */
    void convertSepGraphToGr();


    static void test();



    Separator* sep;

//    function<bool(Separator&, Separator&)>* sepEval; // function to evaluate separator

    int n,N;

    const int minimizationType;
    static const int MINIMIZE_NODES = 1;
    static const int MINIMIZE_EDGES = 2;

};

#endif //ALGORITHMSPROJECT_GREEDYNODEMINIMIZER_H
