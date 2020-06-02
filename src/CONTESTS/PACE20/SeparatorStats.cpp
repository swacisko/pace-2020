/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include "CONTESTS/PACE20/SeparatorStats.h"
#include "CONTESTS/PACE20/SeparatorEvaluators.h"


ostream& operator<<(ostream& str, SeparatorStats& stats){
    str << "size: " << stats.size
        << " | #comps: " << stats.numberOfComponents
        << " | maxSize: " << stats.maxCompSize   << " (" << stats.originalGraphSize  << ")"
        << " | maxEdges: " << stats.maxCompEdges << " (" << stats.originalGraphEdges << ")"
//        << "  |  maxCompAvgDeg: " << stats.maxCompAvgDeg
//        << "  |  compsSizeSquares: " << stats.compsSizeSquares
//        << "  |  maxCompDensity: " << stats.maxCompDensity
//        << "  |  origEdges: " << stats.originalGraphEdges
//        << "  |  approxHeightNode: " << ( stats.approximateMaxTreeHeightNode != -1 ? stats.approximateMaxTreeHeightNode : SeparatorEvaluators::estimateDepthBasedOnNodes(*stats.sep) )
        << " | ahNode: " <<  SeparatorEvaluators::estimateDepthBasedOnNodes(*stats.sep)
//        << "  |  approxHeightEdge: " << ( stats.approximateMaxTreeHeightEdge != -1 ? stats.approximateMaxTreeHeightEdge : SeparatorEvaluators::estimateDepthBasedOnEdges(*stats.sep) );
        << " | ahEdge: " <<SeparatorEvaluators::estimateDepthBasedOnEdges(*stats.sep);

    return str;
}