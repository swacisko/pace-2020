//
// Created by sylwester on 3/27/20.
//

#ifndef ALGORITHMSPROJECT_DEPTHTREEPIVOTMAKER_H
#define ALGORITHMSPROJECT_DEPTHTREEPIVOTMAKER_H

#include "DepthTree.h"


namespace DepthTreePivotMaker{

    /**
     * Function checks whether there can be made any pivots in the depth tree concerning the root stretch of the tree
     * @param dt
     * @return
     */
    extern DepthTree makePivot( DepthTree & dt );


    /**
     * It traverses the tree from topmost stretch each time taking the highest subtree. When the height of traversed tree is greater than balance * dt.height, then
     * we take all vertices in traversed stretches, make it a separator, take all unattached subtrees and merge them using ComponentTreeMerger
     * @param dt
     * @param balance
     * @return
     */
    extern DepthTree makePivotMultipleStretch( DepthTree& dt, double balance );


    /**
     * Makes pivots by taking set P of nodes in some beginning stretches of depth tree, finding subset P' of P with |N(P') \cap T0| < |P'| and
     * @param dt
     * @param balance
     * @return
     */
    extern DepthTree makeHallSetPivots( DepthTree& dt, double balance, bool useDeepestPath = false );



    //*********************** Hall-Set single pass

    struct SubtreeData : public DepthTree{
        SubtreeData(VVI& V) : DepthTree(V) {}
        unordered_set<int> neighInP;
    };

    extern DepthTree makeHallSetPivotsSinglePass( DepthTree& dt, bool useDeepestPath = false );



    /**
     * Makes a series of pivots and returns best tree found
     * @param dt
     * @return
     */
    extern DepthTree makeAllPivots( DepthTree& dt );

    extern void test();

}

#endif //ALGORITHMSPROJECT_DEPTHTREEPIVOTMAKER_H
