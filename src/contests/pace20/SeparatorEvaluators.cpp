/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <contests/pace20/SeparatorEvaluators.h>
#include <contests/pace20/Pace20Params.h>

#include "contests/pace20/SeparatorEvaluators.h"


bool SeparatorEvaluators::sizeTimesMaxCompSize(Separator& sep1, Separator& sep2){
    double norm1 = sep1.nodes.size();
    double norm2 = sep2.nodes.size();
    return norm1 * sep1.stats.maxCompSize < norm2 * sep2.stats.maxCompSize;
}

bool SeparatorEvaluators::sizeTimesMaxCompEdges(Separator& sep1, Separator& sep2){
    double norm1 = sep1.nodes.size();
    double norm2 = sep2.nodes.size();
    return norm1 * sep1.stats.maxCompEdges  < norm2 * sep2.stats.maxCompEdges ;
}



double SeparatorEvaluators::getAlphaParamNode(const Separator &sep) {
    return (double)sep.stats.size / sep.stats.originalGraphSize;
}

double SeparatorEvaluators::getAlphaParamEdge(const Separator &sep) {
    return (double)sep.stats.size / sep.stats.originalGraphEdges;
}

double SeparatorEvaluators::getBetaParamNode(const Separator &sep) {
    double addFactor = (1 - (double)sep.stats.maxCompSize / sep.stats.originalGraphSize) * 0.9;
    double beta = ((double)sep.stats.maxCompSize+addFactor) / sep.stats.originalGraphSize;
    return beta;
}

double SeparatorEvaluators::getBetaParamEdge(const Separator &sep) {
    double addFactor = (1 - (double)sep.stats.maxCompEdges / sep.stats.originalGraphEdges) * 0.9;
    double beta = ((double)sep.stats.maxCompEdges+addFactor) / sep.stats.originalGraphEdges;
    return beta;
}

double SeparatorEvaluators::estimateNodeHeightOfSepBasedOnBest(const Separator &sep, const Separator &bestSep) {
    double alpha = getAlphaParamNode(bestSep);
    double beta = getBetaParamNode(bestSep);
    return sep.stats.size + alpha * sep.stats.maxCompSize / ( 1 - beta );
}

double SeparatorEvaluators::estimateEdgeHeightOfSepBasedOnBest(const Separator &sep, const Separator &bestSep) {
    double alpha = getAlphaParamEdge(bestSep);
    double beta = getBetaParamEdge(bestSep);
    return sep.stats.size + alpha * sep.stats.maxCompEdges / ( 1 - beta );
}

pair<double, double>
SeparatorEvaluators::estimateHeightsOfSepBasedOnBest(const Separator &sep, const Separator &bestSep) {
    return { estimateNodeHeightOfSepBasedOnBest(sep,bestSep), estimateEdgeHeightOfSepBasedOnBest(sep,bestSep) };
}



double SeparatorEvaluators::estimateDepthBasedOnNodes(const SeparatorStats &sep_stats) {
    if( sep_stats.maxCompSize <= 2 ) return sep_stats.size + sep_stats.maxCompSize;
    double addFactor = (1 - (double)sep_stats.maxCompSize / sep_stats.originalGraphSize) * 0.9;
    double beta = ((double)sep_stats.maxCompSize+addFactor) / sep_stats.originalGraphSize;
    double logBetaN = log( sep_stats.originalGraphSize ) / log( 1 / beta );
    double fact = ( 1 - pow(beta,logBetaN) ) / ( 1 - beta );
    return (double)sep_stats.size * (fact+Constants::EPS);
}


double SeparatorEvaluators::estimateDepthBasedOnEdges(const SeparatorStats &sep_stats) {
    if( sep_stats.maxCompEdges == 0 ) return sep_stats.size + 1;
    else if( sep_stats.maxCompEdges <= 2 ) return sep_stats.size + 2;

    double addFactor = (1 - (double)sep_stats.maxCompEdges / sep_stats.originalGraphEdges) * 0.9;
    double beta = ((double)sep_stats.maxCompEdges+addFactor) / sep_stats.originalGraphEdges;
    double logBetaN =  log( sep_stats.originalGraphEdges ) / log( 1 / beta ); //
    double fact = ( 1 - pow(beta,logBetaN) ) / ( 1 - beta );
    return (double)sep_stats.size * (fact + Constants::EPS);
}


bool SeparatorEvaluators::estimatedDepthTreeNode(const Separator &sep1, const Separator &sep2) {
    if( Config::require_balanced_separators ){
        double balance = Config::sep_balance;
        if( isBalanced(sep1,balance) != isBalanced(sep2,balance) ) return isBalanced(sep1,balance);
    }

    double perc1 = (double)sep1.stats.size / sep1.stats.originalGraphSize;
    double perc2 = (double)sep2.stats.size / sep2.stats.originalGraphSize;

    double THR = 0.75;
    if( perc1 > THR && perc2 <= THR ) return false;
    if( perc1 <= THR && perc2 > THR ) return true;

    return estimateDepthBasedOnNodes(sep1.stats) < estimateDepthBasedOnNodes(sep2.stats);
}


bool SeparatorEvaluators::estimatedDepthTreeEdge(const Separator &sep1, const Separator &sep2) {
    if( Config::require_balanced_separators ){
        double balance = Config::sep_balance;
        if( isBalanced(sep1,balance) != isBalanced(sep2,balance) ) return isBalanced(sep1,balance);
    }

    double perc1 = (double)sep1.stats.size / sep1.stats.originalGraphSize; // original
    double perc2 = (double)sep2.stats.size / sep2.stats.originalGraphSize; // original

    double THR = 0.75;
    if( perc1 > THR && perc2 <= THR ) return false;
    if( perc1 <= THR && perc2 > THR ) return true;

    return estimateDepthBasedOnEdges(sep1.stats) < estimateDepthBasedOnEdges(sep2.stats);
}


double SeparatorEvaluators::edgeScaleFactor = 0.5;
double SeparatorEvaluators:: nodeScaleFactor = 0.5;

bool SeparatorEvaluators::estimatedDepthTreeEdgePlusNode(const Separator &sep1, const Separator &sep2) {
    if( Config::require_balanced_separators ){
        double balance = Config::sep_balance;
        if( isBalanced(sep1,balance) != isBalanced(sep2,balance) ) return isBalanced(sep1,balance);
    }

    double perc1 = (double)sep1.stats.size / sep1.stats.originalGraphSize; // original
    double perc2 = (double)sep2.stats.size / sep2.stats.originalGraphSize; // original

    double THR = 0.75;
    if( perc1 > THR && perc2 <= THR ) return false;
    if( perc1 <= THR && perc2 > THR ) return true;

    return
    edgeScaleFactor * estimateDepthBasedOnEdges(sep1.stats) + nodeScaleFactor * estimateDepthBasedOnNodes(sep1.stats)
    <
    edgeScaleFactor * estimateDepthBasedOnEdges(sep2.stats) + nodeScaleFactor * estimateDepthBasedOnNodes(sep2.stats);
}



SeparatorEvaluator SeparatorEvaluators::sepEvalToUse = estimatedDepthTreeEdgePlusNode; // works fine for very dense graphs!!


bool SeparatorEvaluators::estimatedDepthTreeMaxNodeEdge(const Separator &sep1, const Separator &sep2) {
    if( Config::require_balanced_separators ){
        double balance = Config::sep_balance;
        if( isBalanced(sep1,balance) != isBalanced(sep2,balance) ) return isBalanced(sep1,balance);
    }

    return
    max( estimateDepthBasedOnEdges(sep1.stats), estimateDepthBasedOnNodes(sep1.stats) )
    <
    max( estimateDepthBasedOnEdges(sep2.stats), estimateDepthBasedOnNodes(sep2.stats) );
}

bool SeparatorEvaluators::estimatedDepthTreeMinNodeEdge(const Separator &sep1, const Separator &sep2) {
    if( Config::require_balanced_separators ){
        double balance = Config::sep_balance;
        if( isBalanced(sep1,balance) != isBalanced(sep2,balance) ) return isBalanced(sep1,balance);
    }

    return
    min( estimateDepthBasedOnEdges(sep1.stats), estimateDepthBasedOnNodes(sep1.stats) )
    <
    min( estimateDepthBasedOnEdges(sep2.stats), estimateDepthBasedOnNodes(sep2.stats) );
}

bool SeparatorEvaluators::isBalanced(const Separator &sep, double balance) {
    double nodeRatio = (double) sep.stats.maxCompSize / sep.stats.originalGraphSize;
    if( nodeRatio <= balance ) return true;

    double edgeRatio = (double) sep.stats.maxCompEdges / sep.stats.originalGraphEdges;
    if( edgeRatio <= balance ) return true;

    return false;
}



