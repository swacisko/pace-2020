/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/
#ifndef ALGORITHMSPROJECT_PACE20PARAMS_H
#define ALGORITHMSPROJECT_PACE20PARAMS_H

#include <csignal>
#include <mutex>

namespace Pace20Params{

    extern bool quickAndWeakTreeCreation; // if true, the we use only bfs creator with 1 source and absolutely no minimization

    extern int inputGraphSize;
    extern int inputGraphEdges;

    extern int temp;

    extern const bool useExactTrack;

    extern bool useOnlyArtPoints;
    extern bool useKernelization;
    extern int minGraphSizeForKernelization;
//    extern int minGraphSizeForDeg3Kernelization;

    extern bool minimizeNodesIteration;

    /**
     * balance means that separators S with property that max_comp_size( G \ S ) <= balance * |G| is balanced.
     */
    extern double balance;

    extern bool requireBalancedSeparators;

    /**
     * ComponentTreeMergerSmallSize
     * When merging trees we will consider for given separator all orderd of connected components to add if there are no more components than this value.
     */
    extern double CTMergerSmallSize;


    extern int maxSeparatorSizeForGNEMinimizer;

    extern int maxSeparatorSizeForFlowMinimizer;

    extern int maxSources;

    extern std::mutex outputWriterLock;

    extern volatile sig_atomic_t tle;

    extern void terminate(int signum);

    extern int maxBestSepsForMinimizers;

    extern int maxBestSepsForRecursion;
    extern int maxRecDepthForBestSeps;


    extern void setExtensiveRecursionParameters();
    extern void setNonextensiveRecursionParameters();

    extern void setTerminationParameters();
}

#endif //ALGORITHMSPROJECT_PACE20PARAMS_H
