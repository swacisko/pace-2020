/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_DEPTHTREEIRRELEVANTNODESHIFTER_H
#define ALGORITHMSPROJECT_DEPTHTREEIRRELEVANTNODESHIFTER_H

#include "DepthTree.h"

namespace DepthTreeIrrelevantNodeShifter{


    /**
     * If there is a node in a stretch, that has edges in real graph only up the tree, but in the elimination tree it has sons, then it is removed, its sons
     * are attached to its parent, and the irrelevant node is attached to the lowest of its neighbors
     * @return
     */
    extern void shiftIrrelevantNodes(DepthTree &dt);


    void test();

}

#endif //ALGORITHMSPROJECT_DEPTHTREEIRRELEVANTNODESHIFTER_H
