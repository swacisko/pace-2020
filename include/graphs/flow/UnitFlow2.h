/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_UNITFLOW2_H
#define ALGORITHMSPROJECT_UNITFLOW2_H

#include "Makros.h"

/**
 * This class actually reverses edges after ang augmenting path is found. This enables us to keep in graph structure only those edges that really are in the graph,
 * unlike in UnitFlow first version where we just update cap vector to indicate that na edge is present or not
 */
class UnitFlow2{

public:
    UnitFlow2(VVI& V);


    /**
     * Function calculates flow until it reaches at least maxFlowBound. If maxFlowBound is not given or set to -1, then maximum flow is calculated
     *
     * CAUTION!
     * When this function is used multiple times, then it will not return correct data, apart from the first use.
     * We must use preprocess() BEFORE EACH USE after the first one, since edge saturation is not reset by default.
     *
     * @param maxFlowBound flow only up to that value will be checked. If maximum flow is greater, it will not be found.
     * @param reverseSourceEdges if false, then during flow finding, if an augmenting paths is found, all edges except from those that have beginning in source, are reversed.
     * @return
     */
    int calculateFlow( VI sources, VI ends, int maxFlowBound = -1 );

    /**
     * Adds source s to sources
     * @param s
     */
    void addSource(int s);

    /**
     * Removes source s from sources - removes all edges going out of s and to s. Needed by FlowCutter algorithm as a speedup
     * CAUTION! It is impossible yet to get edge saturation after this operation
     * @param s
     */
    void removeSource(int s);

    /**
     * Adds target t to targets
     * @param t
     */
    void addTarget(int t);

    /**
     * Removes target t from targets - removes all edges going out of t and to t. Needed by FlowCutter algorithm as a speedup
     * * CAUTION! It is impossible yet to get edge saturation after this operation
     * @param t
     */
    void removeTarget(int t);

    /**
     *
     * @return res[i] is a set of of elements x such that edge i->x is saturated
     */
    VVI getEdgeSaturation();

    /**
     * Augments flow from given sources to given ends.
     * It can be used e.g. to create flow, then add new source and augment the flow again.
     */
    void augmentFlow();

    /**
     *
     * @return set of nodes that are reachable from surces via non-saturated paths, WITHOUT sources (that are naturally reachable)
     */
    VI getSourceReachableNodes();

    /**
     * Finds all source-reachable nodes from source s using only nodes for which was2 is false
     * @param s
     * @param was
     * @return
     */
    VI getSourceReachableNodes( int s, VB& was2 );

    /**
     *
     * @return set of nodes from which exists a non-saturated path to some target, WITHOUT targets.
     */
    VI getTargetReachableNodes();

    /**
     * Finds all target-reachable nodes that end in target t and follow only unsaturated paths with nodes for which was2 is false
     * @param from
     * @param was2
     * @return
     */
    VI getTargetReachableNodes( int t, VB& was2 );

    /**
     * Function to debug
     */
    void writeSaturatedEdges();

    void setUseDfsAugmentation( bool b ){ useDfsAugmentation = b; }

    int flowValue(){ return maxFlow; }

    VVI& getV(){return V;}
    VVI& getRevV(){return revV;}

    static void test();

private:
    /**
     * Original graph, it may be directed
     */
    VVI origV;

    int N; // size of V
    VVI V; // undirected structure of graph origV

    VVI revV; // it is neccessary to create reversedV after each augmentation to be able to extract target-reachable nodes.
    VPII edgesToReverseInRevV;
    void updateRevV();


    VB was;
    VI inLayer;
    int maxFlow;
    unordered_set<int> sources, targets;
    VB isSource, isTarget;

    /**
     * Creates all data needed to execute algorithm
     */
    void preprocess();

    /**
     * Creates layers from sources to the first met target.
     * Returned nodes can be used to clear inLayer data for those nodes
     * @return nodes that are in given layers, in which layers those nodes are can be found in @{inLayer} array
     */
    VI createLayers();

    /**
     * Single iteration of augmenting a flow - creation of layers and maximal set of edges-disjoint paths, and augmenting the flow
     * @return
     */
    bool augmentFlowIteration();

    bool augmentBFS();
    bool augmentDFS();

    /**
     * If true, then i will look for augmenting paths using dfs instead of creating bfs-layers.
     * This might be faster, but usually find extremely long augmenting paths, whereas bfs-layer method find short - it may be significant in some cases
     */
    bool useDfsAugmentation;

};

#endif //ALGORITHMSPROJECT_UNITFLOW2_H
