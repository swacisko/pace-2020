/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DEPTHTREECREATORSMALL_H
#define ALGORITHMSPROJECT_DEPTHTREECREATORSMALL_H

#include "Makros.h"
#include "CONTESTS/PACE20/DepthTree.h"
#include "DepthTreeCreator.h"

class DepthTreeCreatorSmall : public DepthTreeCreator{
public:
    DepthTreeCreatorSmall(VVI & V,int depth);

    DepthTree getDepthTree();

};

#endif //ALGORITHMSPROJECT_DEPTHTREECREATORSMALL_H
