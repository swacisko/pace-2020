//
// Created by sylwester on 5/4/20.
//

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
