//
// Created by sylwester on 3/16/20.
//

#ifndef ALGORITHMSPROJECT_PACE20_H
#define ALGORITHMSPROJECT_PACE20_H


#include <graphs/GraphReader.h>
#include <csignal>
#include "CONTESTS/PACE20/depthtreecreators/DepthTreeCreatorSmall.h"
#include "CONTESTS/PACE20/depthtreecreators/DepthTreeCreatorLarge.h"
#include "graphs/GraphUtils.h"
#include "Pace20Params.h"

#include <sys/resource.h>
//#include <graphs/flow/DisjointPaths.h>
//#include <graphs/flow/UnitFlow.h>
#include "CONTESTS/PACE20/separatorcreators/RemovalOrderSeparatorCreator.h"
#include "SeparatorEvaluators.h"

namespace Pace20{

    extern DepthTree *volatile globalBestTree;

    /**
     * Installs SIGTERM signal listener
     */
    extern void addSigtermCheck();

    /**
     * Increases stack size (due to hard limit on my machine it is easier to increase in each program execution
     */
    extern void increaseStack();




    /**
     * Runs the main algorithm
     * @param argc
     * @param argv
     */
    extern void run( int argc, char **argv  );


}



#endif //ALGORITHMSPROJECT_PACE20_H
