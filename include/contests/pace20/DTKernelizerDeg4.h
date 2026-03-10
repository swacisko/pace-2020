/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DTKERNELIZERDEG4_H
#define ALGORITHMSPROJECT_DTKERNELIZERDEG4_H

#include "Separator.h"
#include "DepthTree.h"
#include <graphs/GraphInducer.h>

class DTKernelizerDeg4{
public:
    DTKernelizerDeg4( VVI& V ){ this->V = &V; }

    VVI kernelize();
    DepthTree dekernelize( DepthTree dt );

    static void test();

private:
    VVI* V;

    InducedGraph kernelizedV;
    VPII deg4AddedEdges;
    VI deg4RemovedNodes;


};

#endif //ALGORITHMSPROJECT_DTKERNELIZERDEG4_H
