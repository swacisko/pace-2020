/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_SUBTREERERUNNERIMPROVER_H
#define ALGORITHMSPROJECT_SUBTREERERUNNERIMPROVER_H

#include "DepthTree.h"


class SubtreeRerunnerImprover{
public:

    /**
     * Selects heighest node on the deepest path such that subtree containing that node contains at most balance * V.size() nodes.
     * Then takes hte graph induced by nodes in that subtree and reruns
     * @param dt
     * @return
     */
    DepthTree improve(DepthTree& dt, double balance = 0.2);

    static void test();
private:
};

#endif //ALGORITHMSPROJECT_SUBTREERERUNNERIMPROVER_H
