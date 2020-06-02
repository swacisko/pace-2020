/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DTKERNELIZER_H
#define ALGORITHMSPROJECT_DTKERNELIZER_H

#include <graphs/GraphInducer.h>
#include "Separator.h"
#include "DepthTree.h"

class DTKernelizer{

public:
    DTKernelizer(VVI &V);

    /**
     *
     * @return graph after kernelization
     */
    VVI & getKernelizedGraph(int KERNEL_MODE = DANGLING_TREES);

    /**
     * Function modifies depth tree created for kernelizedV.V to make it a depth tree for V
     * @param dt
     * @return
     */
    DepthTree dekernelize(DepthTree dt);



    VVI & getKernelizedGraphSubgraphs(int KERNEL_MODE = DANGLING_TREES);
    DepthTree dekernelizeSubgraphs(DepthTree dt);



    static void test();

    static const int DANGLING_TREES = 1;
    static const int EDGE_COMPRESSION = 2;



    VVI kernelizeDegree3Nodes( VVI& G );
    void dekernelizeDegree3Nodes( VVI& G );

private:
    InducedGraph kernelizedDeg3V;
    VVPII deg3AddedEdgesInIteration;
    VVPII deg3RemovedEdgesInIteration;



    VVI* V;
    InducedGraph kernelizedV;

    /**
     * removedTreeNodes[i] is a set of vertices that induces a connected tree-subgraph in V. DepthTree will be made for that subgraph and root will be
     * attached to the unique vertex incident to any node in that tree
     */
    VVI removedTreeNodes;
    set<PII> edgesToAddToKernelizedGraph;

    /**
     * centroids[i] is a centroid decomposition of tree induced by nodes removedTreeNodes[i] (keeping pointers to parents in that tree)
     */
    vector< unordered_map<int,int> > centroids;


    /**
     * Point to whichc the tree is atached in graph V, that is
     * @param tree
     * @return
     */
    VI getAttachmentPoints(VI &treeNodes);

    void createDanglingTrees();








    VVI removedSubgraphsNodes;
    vector<DepthTree> subgraphTrees;
    VVI createDanglingSubgraphs(int recDepth=0);
};

#endif //ALGORITHMSPROJECT_DTKERNELIZER_H
