/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <CONTESTS/PACE20/Pace20Params.h>
#include <CONTESTS/PACE20/Pace20.h>
#include <mutex>
#include <utils/TimeMeasurer.h>

#include "CONTESTS/PACE20/Pace20Params.h"

namespace Pace20Params{

    int temp = 0;

    int inputGraphSize;
    int inputGraphEdges;

    bool quickAndWeakTreeCreation = true;

    bool useOnlyArtPoints = false;
    bool useKernelization = true;
    int minGraphSizeForKernelization = 1'000;
//    int minGraphSizeForDeg3Kernelization = 1'000;

    bool minimizeNodesIteration = false;

    /**
     * balance means that separators S with property that max_comp_size( G \ S ) <= balance * |G| is balanced.
     */
    double balance = 0.97;

    bool requireBalancedSeparators = true;

    /**
     * ComponentTreeMergerSmallSize
     * When merging trees we will consider for given separator all orderd of connected components to add if there are no more components than this value.
     */
    double CTMergerSmallSize = 4;


    // for small dataset choose approx.

    int maxSeparatorSizeForGNEMinimizer = 3'000;

    int maxSeparatorSizeForFlowMinimizer = 5'000;

    int maxSources = 7;

    int maxBestSepsForMinimizers = 5;

    int maxBestSepsForRecursion = 1;
    int maxRecDepthForBestSeps = 0;

    volatile sig_atomic_t tle = 0;
    mutex outputWriterLock;

    void terminate(int signum) {
        tle = 1;
        if( Pace20::globalBestTree != nullptr ){
            outputWriterLock.lock();
            Pace20::globalBestTree->write();

            DEBUG( Pace20::globalBestTree->height );
            TimeMeasurer::stopMeasurement("PACE20");
            TimeMeasurer::writeAllMeasurements();
//            outputWriterLock.unlock(); // if we wrote an answer we do not release the lock
            exit(0);
        }
    }

    void setExtensiveRecursionParameters() {
        balance = 0.9;
        CTMergerSmallSize = 4;
        maxSeparatorSizeForGNEMinimizer = 350;
//        maxSeparatorSizeForGNEMinimizer = 500;
        maxSources = 5;
        maxBestSepsForRecursion = 15;
        maxRecDepthForBestSeps = 2;
    }

    void setNonextensiveRecursionParameters(){
        balance = 0.9;
        CTMergerSmallSize = 4;
        maxSeparatorSizeForGNEMinimizer = 500;
//        maxSources = 10;
        maxSources = 5;
        maxBestSepsForRecursion = 1;
        maxRecDepthForBestSeps = 0;
    }


    void setTerminationParameters(){
        maxSources = 1;
        maxSeparatorSizeForGNEMinimizer = 0;
        maxBestSepsForRecursion = 1;
        maxRecDepthForBestSeps = 0;
        CTMergerSmallSize = 0;
    }

}


/**
For exact version choose:
 balance = 0.9
 CTMergerSmallSize = 4;
 maxSep = 350
 maxSources=5
 maxBestSeps = 10
 maxRecDepth = 2







 */