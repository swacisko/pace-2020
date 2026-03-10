/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_SEPARATOREVALUATORS_H
#define ALGORITHMSPROJECT_SEPARATOREVALUATORS_H

#include "Separator.h"


namespace SeparatorEvaluators{

    extern bool isBalanced(const Separator& sep, double balance);


    extern bool sizeTimesMaxCompSize( Separator&, Separator& );

    extern bool sizeTimesMaxCompEdges( Separator&, Separator& );


    extern double estimateDepthBasedOnNodes(const Separator& sep);
    extern double estimateDepthBasedOnEdges(const Separator& sep);

    extern bool estimatedDepthTreeNode(const Separator &sep1, const Separator &sep2);
    extern bool estimatedDepthTreeEdge(const Separator &sep1, const Separator &sep2);

    extern double edgeScaleFactor;
    extern double nodeScaleFactor;
    extern bool estimatedDepthTreeEdgePlusNode(const Separator &sep1, const Separator &sep2);

    extern double getAlphaParamNode(const Separator& sep);
    extern double getAlphaParamEdge(const Separator& sep);
    extern double getBetaParamNode(const Separator& sep);
    extern double getBetaParamEdge(const Separator& sep);
    extern double estimateNodeHeightOfSepBasedOnBest( const Separator& sep, const Separator& bestSep );
    extern double estimateEdgeHeightOfSepBasedOnBest( const Separator& sep, const Separator& bestSep );
    extern pair<double,double> estimateHeightsOfSepBasedOnBest( const Separator& sep, const Separator& bestSep );

    extern SeparatorEvaluator sepEvalToUse;

    extern bool estimatedDepthTreeMaxNodeEdge(const Separator &sep1, const Separator &sep2);
    extern bool estimatedDepthTreeMinNodeEdge(const Separator &sep1, const Separator &sep2);

}

#endif //ALGORITHMSPROJECT_SEPARATOREVALUATORS_H
