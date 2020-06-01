//
// Created by sylwester on 4/17/20.
//

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
