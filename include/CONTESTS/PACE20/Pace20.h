/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

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
