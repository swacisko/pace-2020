//
// Created by sylwester on 8/28/19.
//

#ifndef ALGORITHMSPROJECT_KERNELIZERVC_H
#define ALGORITHMSPROJECT_KERNELIZERVC_H

#include "Makros.h"

/**
 * Class responsible for kernelization of the graph for vertex cover problem
 */
class KernelizerVC{
public:



    //******************************** BEGINNING OF KERNELIZATION SECTION
    /**
     * This function performs initial kernelization for the graph, such as crown decomposition.
     * CAUTION! It modifies the structure of the graph.
     * @param G
     * @return all vertices that certainly are in some VC of G and all edges that were removed.
     */
    pair<VI, VPII> initialKernelization(VVI &G);


    /**
     * @return all vertices that have loops and all edges that were removed
     */
    pair<VI, VPII> loopKernelization(VVI &G);

    /**
     * Function applies domination rule to all nodes that have maximum degree @{maxDegree} and its neighbors.
     * @param V
     * @param maxDegree
     * @return
     */
    pair<VI, VPII> dominationRule( VVI & V, int maxDegree );


    /**
     *
     * @param G
     * @return all nodes that should be added to VC using crown decomposition method and all edges that were removed from G.
     */
    pair<VI, VPII> crownDecomposition(VVI &G);

    /**
     * Though this has lp in name, it does no use linear programming. It uses max matching in bipartite layer graph
     * @param G
     * @return all vertices that should be added to VC using max matching and V0, V0.5 and V1 assignment for LP.
     */
    pair<VI,VPII> lpDecomposition(VVI& G);

    /**
     * Function performs that kernelization in each iteration - it takes nodes from given set.
     * @param G
     * @param nodeDegrees
     * @return nodes that should be added to VC and edges that were removed.
     */
    pair<VI, VPII> iterativeKernelization(VVI &G, set<PII> &nodeDegrees, VI &currentVC, VI &bestVC);

    /**
     * Kernelizes all nodes that have a leaf neighbor
     * @param V
     * @return
     */
    pair<VI, VPII> degree1Kernelization( VVI & V );

private:

    /**
     * Removes all nodes and corresponding edges from graph G and updates nodeDegrees
     * @param G
     * @param toRemove
     * @return all edges that were removed that way.
     */
    VPII removeNodesFromGraph(VVI &G, VI &toRemove, set<PII> &nodeDegrees);

    /**
     *
     * @param G
     * @param toRemove
     * @return all edges that were removed that way.
     */
    VPII removeNodeFromGraph( VVI & G, int toRemove );

    /**
     * Add all @edges to the graph.
     * @param G
     * @param edges
     */
    void addEdgesToGraph( VVI & G, VPII & edges );

    //******************************** END OF KERNELIZATION SECTION


    /**
     * Function updates neighborhoods of graph G and returns nodeDegrees.
     * @param G
     * @return nodeDegrees for given graph, that is set of pairs for nodes in the form (degree, Id). Only nodes with positive degree will be added
     */
    set<PII> getNodeDegreesForGraph(VVI & G);

    /**
     * Function adds proper edges to @G and pairs {degree, id} to @nodeDegrees of vertices that were previously kernelized and are given here as kernel.
     * This function also removes last kernel.first.size() elements form currentVC.
     * @param kernel
     */
    void restoreGraphAndNodeDegreesFromKernel(VVI &G, set<PII> &nodeDegrees, pair<VI, VPII> &kernel, VI &currentVC);


    /**
     * If @matching is a maximal matching of a graph 'induced' by bipartition ((a,b) belong to G iff bipartition[a] != bipartition[b]), then this function will return minimal vertex
     * cover that covers all edges.
     * @param G
     * @param matching
     * @param bipartition
     * @return minimum (with regard to size) vector of vertices of G that cover all edges in the @matching between nodes in bipartitions
     */
    VI getMinVCForBipartition(VVI &G, VI &matching, VB &bipartition);




};

#endif //ALGORITHMSPROJECT_KERNELIZERVC_H
