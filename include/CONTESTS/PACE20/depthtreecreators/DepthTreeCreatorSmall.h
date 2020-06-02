/*
 * Copyright (c) 2020, Sylwester Swat
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
