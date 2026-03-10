/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
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
