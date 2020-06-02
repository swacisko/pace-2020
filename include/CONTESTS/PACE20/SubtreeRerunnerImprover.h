/*
 * Copyright (c) 2020, Sylwester Swat
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
