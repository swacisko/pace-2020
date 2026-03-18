/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DEPTHTREECREATORLARGE_H
#define ALGORITHMSPROJECT_DEPTHTREECREATORLARGE_H

#include "DepthTreeCreator.h"

class DepthTreeCreatorLarge : public DepthTreeCreator{
public:
    DepthTreeCreatorLarge(VVI & V, int recurrenceDepth, Config c);

    DepthTree getDepthTree();

    vector<pair<SeparatorStats,SeparatorStats>> sep_data;

    static void test();

private:

    void testBridgesAndArtPoints();

    void testVCorARTPOINTKernelization();

    Separator testMatchingEdgesContraction();

};


#endif //ALGORITHMSPROJECT_DEPTHTREECREATORLARGE_H
