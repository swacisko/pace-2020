/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_ARTPOINTSEPARATORCREATOR_H
#define ALGORITHMSPROJECT_ARTPOINTSEPARATORCREATOR_H

#include <datastructures/FAU.h>
#include "Makros.h"
#include "SeparatorCreator.h"

class ArtPointSeparatorCreator : public SeparatorCreator{

public:


    vector<Separator> createSeparators(VVI& V, int repeats);

    /**
     * Selects greedily most balanced nodes of the tree until those nodes form a balanced separator.
     * @param V
     * @return
     */
    Separator getFirstBalancedArtPointSeparator( VVI& V);

    /**
     * Creates articulation point tree and associated data (edgeWeights and nodeWeights)
     */
    void createArtPointGraph();

    /**
     * For given set was of 'forbidden' nodes, function updates nodeWeights and edgeWeights for given tree within bounds of those 'forbidden 'nodes.
     * This is just to quickly find next balanced articulation points.
     * @param num
     * @param par
     * @param was
     * @return res.first is id of the best art point to add to separator, res.second is the maximum number of edges in newly created
     * components resulted after removing res.first from corresponding graph. This second value is required to update priority queue.
     */
    PII getMostBalancedArtInSubgraph(int num, VB &was, VB &forbidden);


    int getMostBalancedArtPoint(int num, int par, VB& was);


    static void test();

private:
    VVI V;
    VI arts;
    VB isArtPoint; // isArtPoint[i] is true if i-th node represents articulation point, otherwise it represent connected component of the graph
    int n,N; // n is number of articulation points - represented by nodes [0, n-1] and N is number of all nodes (as in sepGraph in Minimizers)

    VVI apT; // articulation points tree
    VVI apW; // weights of edges in the tree apT, that is number of edges between articulation point and connected component in the graph (or other articulation point)
    VI nodeWeights, edgeWeights; // nodeWeights and edgeWeight denote number of edges in component represented by a vertex in the tree
    VI nodeWeightsSum, edgeWeightsSum;
    VI nodeWeightsSumInSubgraph, edgeWeightsSumInSubgraph;
    VI maxNodesInSubgraph, maxEdgesInSubgraph;
    VI preOrder;
    VI low;

};

#endif //ALGORITHMSPROJECT_ARTPOINTSEPARATORCREATOR_H
