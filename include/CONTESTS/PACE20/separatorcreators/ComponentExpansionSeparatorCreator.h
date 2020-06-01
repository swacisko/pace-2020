//
// Created by sylwester on 4/5/20.
//

#ifndef ALGORITHMSPROJECT_COMPONENTEXPANSIONSEPARATORCREATOR_H
#define ALGORITHMSPROJECT_COMPONENTEXPANSIONSEPARATORCREATOR_H

#include "SeparatorCreator.h"


class ComponentExpansionSeparatorCreator : public SeparatorCreator{

    struct CompBorder{
        // compEdges[i] = total number of edges in components induced by set X_i  \ border_i,
        // where X_i is the set of first i+1 elements in order and border_i is the set of elements in border after adding first i+1 nodes.
        VI compEdges, borderSize;
        // maxInternalCompEdges[i] is maximum number of edges in an internal component. The same is for size.
        VI maxInternalCompSize;
        VI maxInternalCompEdges;
        VVI modifications;
    };
public:

//    ComponentExpansionSeparatorCreator( function< bool(const Separator&, const Separator&) > eval );
    ComponentExpansionSeparatorCreator( SeparatorEvaluator eval );

    vector<Separator> createSeparators(VVI& V, int repeats);

    /**
     *
     * @param v
     * @return vector containing some expansion orders, according to different strategies.
     */
    VVI getExpansionOrdersForNode(VVI &V, int v);

    VVI getExpansionOrdersForNodes( VVI & V, VI sources );


    /**
     *
     * @param V
     * @param sources
     * @return
     */
    VI getTightestNeighborOrder(VVI &V, VI sources);

    /**
     *
     * @param V
     * @param sources
     * @return order in which nodes appear in bfs traverse with start from given sources
     */
    VI getBFSOrder( VVI& V, VI sources );

    /**
     * For each pair of layers (i,i+1) there is found a minimal vertex cover for that pair, then, in BFS order,
     * nodes from layer[i] that are in VC(i-1,i) preceed those that are not in that VC
     * @param V
     * @param sources
     * @return
     */
    VI getBFSLayerVCOrder( VVI& V, VI sources );

    /**
     * In each turn a neighbor from border that has least neighbors in the exterior is selected. Then that node is moved from border to interior,
     * and all its neighbors from exterior are added to the border
     * @param V
     * @param sources
     * @return
     */
    VI getLeastExteriorNeighborhoodOrder( VVI& V, VI sources );





    /**
     * Function takes order of adding elements, then as soon as a separator is detected, order is rearranged,
     * so that all vertices in smaller component proceed before those in larger components.
     * @param V
     * @param order
     * @return
     */
    VI optimizeOrderByTraversingFirstSmallestComponents( VVI& V, VI order );

    Separator getBestSeparatorForExpansionOrder( VVI& V, VI order );

    /**
     * Function takes source nodes, then iteratively invokes all order craetors for results
     * @param V
     * @param sources
     * @return
     */
    Separator getIterativeSepForStartingNodes( VVI& V, VI sources );

    /**
     *
     * @param V
     * @param order
     * @return res.first is a vector vec such that vec[i] is is maximum size of a component induced by nodes order[i] + order[i+1] + ... + order.back()
     * res.second[i] contains maximum number of edges in any component induced by nodes order[i] + order[i+1] + ... + order.back()
     */
    pair<VI,VI> calculateExternalSizes(VVI &V, VI order);

    /**
     *
     * @param V
     * @param order
     * @return res.first[i] is the number of nodes on the border of component induced by nodes order[0] + ... + order[i]
     * res.second[i] is the number of edges in that component WITHOUT NODES IN BORDER
     */
    CompBorder calculateInternalSizes(VVI &V, VI order);

    static void test();

//private:
    function< bool(const Separator&, const Separator&) > sepEval;


    static const int TIGHTEST_NODE_ORDER = 1;
    static const int LEAST_NEIGHBORS_ORDER = 2;
    static const int BFS_ORDER = 4;
    static const int BFS_VC_ORDER = 8;

    void setOrdersToCreate( int orders ){ ORDERS_TO_CREATE = orders; }
    void setOrdersToOptimize( int orders ){ ORDERS_TO_OPTIMIZE = orders; }

private:

//    int ORDERS_TO_CREATE = (TIGHTEST_NODE_ORDER /*+ LEAST_NEIGHBORS_ORDER*/);
    int ORDERS_TO_CREATE = (LEAST_NEIGHBORS_ORDER /*+ LEAST_NEIGHBORS_ORDER*/);

//    int ORDERS_TO_OPTIMIZE = ( LEAST_NEIGHBORS_ORDER );
    int ORDERS_TO_OPTIMIZE = ( TIGHTEST_NODE_ORDER );

};

#endif //ALGORITHMSPROJECT_COMPONENTEXPANSIONSEPARATORCREATOR_H
