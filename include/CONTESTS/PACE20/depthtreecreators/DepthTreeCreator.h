/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_DEPTHTREECREATOR_H
#define ALGORITHMSPROJECT_DEPTHTREECREATOR_H

#include <combinatorics/CombinatoricUtils.h>
#include <CONTESTS/PACE20/DepthTree.h>
#include <CONTESTS/PACE20/Separator.h>
#include "Makros.h"

class DepthTreeCreator{
public:
    DepthTreeCreator(VVI & V, int recurrenceDepth){
        this->V = &V;
        recDepth = recurrenceDepth;
    }

    /**
     * Equivalent to CombinatoricUtils::getRandomSequence(V->size(),s)
     * @param s
     * @return set of random s vertices of V
     */
    VI getRandomSources(int s){ return CombinatoricUtils::getRandomSubset( V->size()-1,s); }

    /**
     *
     * @param quantities
     * @return vector containing sets of sources. There will be quantities[i] sets with i elements.
     */
    VVI getRandomSources( VI quantities );



    virtual DepthTree getDepthTree() = 0;

    VVI * V;

    int recDepth;
};

#endif //ALGORITHMSPROJECT_DEPTHTREECREATOR_H
