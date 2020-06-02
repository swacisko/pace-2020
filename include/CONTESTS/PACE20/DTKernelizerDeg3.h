/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DTKERNELIZERDEG3_H
#define ALGORITHMSPROJECT_DTKERNELIZERDEG3_H

#include "Separator.h"
#include "DepthTree.h"
#include <graphs/GraphInducer.h>

class DTKernelizerDeg3{
public:
    DTKernelizerDeg3( VVI& V ){ this->V = &V; }

    VVI kernelize();
    DepthTree dekernelize( DepthTree dt );

    static void test();

private:
    VVI* V;

    InducedGraph kernelizedV;
    VPII deg3AddedEdges;
    VI deg3RemovedNodes;


};

#endif //ALGORITHMSPROJECT_DTKERNELIZERDEG3_H
