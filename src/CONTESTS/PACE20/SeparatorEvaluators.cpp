/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <CONTESTS/PACE20/Pace20Params.h>

#include "CONTESTS/PACE20/SeparatorEvaluators.h"


bool SeparatorEvaluators::sizeTimesMaxCompSize(Separator& sep1, Separator& sep2){
    double norm1 = sep1.nodes.size();
    double norm2 = sep2.nodes.size();
    return norm1 * sep1.stats.maxCompSize < norm2 * sep2.stats.maxCompSize;
}

bool SeparatorEvaluators::sizeTimesMaxCompEdges(Separator& sep1, Separator& sep2){
    double norm1 = sep1.nodes.size();
    double norm2 = sep2.nodes.size();
    return norm1 * sep1.stats.maxCompEdges  < norm2 * sep2.stats.maxCompEdges ;
//    return norm1 * sep1.stats.maxCompEdges + norm1*norm1 < norm2 * sep2.stats.maxCompEdges + norm2*norm2;
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



double SeparatorEvaluators::estimateDepthBasedOnNodes(const Separator &sep) {
    if( sep.stats.maxCompSize <= 2 ) return sep.stats.size + sep.stats.maxCompSize;


//    double beta = (double)sep.stats.maxCompSize / sep.stats.originalGraphSize;
//    double logBetaN = (int)ceil( log( sep.stats.maxCompSize ) / log( 1 / beta ) ); // original values

//    int addFactor = 0;
//    if( sep.stats.maxCompSize < sep.stats.originalGraphSize-1 ) addFactor = 1;
    double addFactor = (1 - (double)sep.stats.maxCompSize / sep.stats.originalGraphSize) * 0.9;


    double beta = ((double)sep.stats.maxCompSize+addFactor) / sep.stats.originalGraphSize;
//    double logBetaN = (int)ceil( log( (sep.stats.maxCompSize+1) ) / log( 1 / beta ) );
    double logBetaN = log( sep.stats.originalGraphSize ) / log( 1 / beta );

    double fact = ( 1 - pow(beta,logBetaN) ) / ( 1 - beta );



    return (double)sep.stats.size * (fact+Constants::EPS);

//    double imbalance = 1 - max( (double)0, (double)sep.stats.maxCompSize / sep.stats.originalGraphSize - 0.5 );
//    return (double)sep.stats.size * (fact+Constants::EPS) * ( 1 + imbalance ); // #TEST


}


double SeparatorEvaluators::estimateDepthBasedOnEdges(const Separator &sep) {
    if( sep.stats.maxCompEdges == 0 ) return sep.stats.size + 1;
    else if( sep.stats.maxCompEdges <= 2 ) return sep.stats.size + 2;

//    double beta = (double)sep.stats.maxCompEdges / sep.stats.originalGraphEdges;
//    double logBetaN =  log( sep.stats.originalGraphEdges ) / log( 1 / beta ); //

//    int addFactor = 0;
//    if( sep.stats.maxCompEdges < sep.stats.originalGraphEdges-1 ) addFactor = 1;
    double addFactor = (1 - (double)sep.stats.maxCompEdges / sep.stats.originalGraphEdges) * 0.9;

    double beta = ((double)sep.stats.maxCompEdges+addFactor) / sep.stats.originalGraphEdges;
    double logBetaN =  log( sep.stats.originalGraphEdges ) / log( 1 / beta ); //


    double fact = ( 1 - pow(beta,logBetaN) ) / ( 1 - beta );



    return (double)sep.stats.size * (fact + Constants::EPS);


//    double imbalance = 1 - max( (double)0, (double)sep.stats.maxCompEdges/ sep.stats.originalGraphEdges - 0.5 );
//    return (double)sep.stats.size * (fact+Constants::EPS) * ( 1 + imbalance ); // #TEST

}


bool SeparatorEvaluators::estimatedDepthTreeNode(const Separator &sep1, const Separator &sep2) {
    if( Pace20Params::requireBalancedSeparators ){
        double balance = Pace20Params::balance;
        if( SeparatorEvaluators::isBalanced(sep1,balance) != SeparatorEvaluators::isBalanced(sep2,balance) ) return SeparatorEvaluators::isBalanced(sep1,balance);
    }

    double perc1 = (double)sep1.stats.size / sep1.stats.originalGraphSize;
    double perc2 = (double)sep2.stats.size / sep2.stats.originalGraphSize;

    double THR = 0.75;
    if( perc1 > THR && perc2 <= THR ) return false;
    if( perc1 <= THR && perc2 > THR ) return true;


    return estimateDepthBasedOnNodes(sep1) < estimateDepthBasedOnNodes(sep2);
}


bool SeparatorEvaluators::estimatedDepthTreeEdge(const Separator &sep1, const Separator &sep2) {
    if( Pace20Params::requireBalancedSeparators ){
        double balance = Pace20Params::balance;
        if( SeparatorEvaluators::isBalanced(sep1,balance) != SeparatorEvaluators::isBalanced(sep2,balance) ) return SeparatorEvaluators::isBalanced(sep1,balance);
    }

    double perc1 = (double)sep1.stats.size / sep1.stats.originalGraphSize; // original
    double perc2 = (double)sep2.stats.size / sep2.stats.originalGraphSize; // original


    double THR = 0.75;
    if( perc1 > THR && perc2 <= THR ) return false;
    if( perc1 <= THR && perc2 > THR ) return true;

    return estimateDepthBasedOnEdges(sep1) < estimateDepthBasedOnEdges(sep2);
}


double SeparatorEvaluators::edgeScaleFactor = 0.5;
double SeparatorEvaluators:: nodeScaleFactor = 0.5;

//double SeparatorEvaluators::edgeScaleFactor = 0.95; // #TEST
//double SeparatorEvaluators:: nodeScaleFactor = 0.05; // #TEST

bool SeparatorEvaluators::estimatedDepthTreeEdgePlusNode(const Separator &sep1, const Separator &sep2) {
    if( Pace20Params::requireBalancedSeparators ){
        double balance = Pace20Params::balance;
        if( SeparatorEvaluators::isBalanced(sep1,balance) != SeparatorEvaluators::isBalanced(sep2,balance) ) return SeparatorEvaluators::isBalanced(sep1,balance);
    }

    double perc1 = (double)sep1.stats.size / sep1.stats.originalGraphSize; // original
    double perc2 = (double)sep2.stats.size / sep2.stats.originalGraphSize; // original


    double THR = 0.75;
    if( perc1 > THR && perc2 <= THR ) return false;
    if( perc1 <= THR && perc2 > THR ) return true;



//    const double EPS = Constants::EPS;
//    const double val1 = edgeScaleFactor * estimateDepthBasedOnEdges(sep1) + nodeScaleFactor * estimateDepthBasedOnNodes(sep1);
//    const double val2 = edgeScaleFactor * estimateDepthBasedOnEdges(sep2) + nodeScaleFactor * estimateDepthBasedOnNodes(sep2);
//
//    if( abs( val1-val2 ) < EPS ){ // #TEST - if seps are identical in statistics, we sort them by nodes in separators
//        if( sep1.stats.size != sep2.stats.size ) return sep1.stats.size < sep2.stats.size;
//        else return accumulate( ALL(sep1.nodes),0 ) < accumulate( ALL(sep2.nodes),0 );
//    }else return val1 < val2;


    return
    edgeScaleFactor * estimateDepthBasedOnEdges(sep1) + nodeScaleFactor * estimateDepthBasedOnNodes(sep1)
    <
    edgeScaleFactor * estimateDepthBasedOnEdges(sep2) + nodeScaleFactor * estimateDepthBasedOnNodes(sep2)
    ;
}



//SeparatorEvaluator SeparatorEvaluators::sepEvalToUse = estimatedDepthTreeEdge;
//SeparatorEvaluator SeparatorEvaluators::sepEvalToUse = estimatedDepthTreeNode;
SeparatorEvaluator SeparatorEvaluators::sepEvalToUse = estimatedDepthTreeEdgePlusNode; // works fine for very dense graphs!!


bool SeparatorEvaluators::estimatedDepthTreeMaxNodeEdge(const Separator &sep1, const Separator &sep2) {
    if( Pace20Params::requireBalancedSeparators ){
        double balance = Pace20Params::balance;
        if( SeparatorEvaluators::isBalanced(sep1,balance) != SeparatorEvaluators::isBalanced(sep2,balance) ) return SeparatorEvaluators::isBalanced(sep1,balance);
    }

    return
    max( estimateDepthBasedOnEdges(sep1), estimateDepthBasedOnNodes(sep1) )
    <
    max( estimateDepthBasedOnEdges(sep2), estimateDepthBasedOnNodes(sep2) );
}

bool SeparatorEvaluators::estimatedDepthTreeMinNodeEdge(const Separator &sep1, const Separator &sep2) {
    if( Pace20Params::requireBalancedSeparators ){
        double balance = Pace20Params::balance;
        if( SeparatorEvaluators::isBalanced(sep1,balance) != SeparatorEvaluators::isBalanced(sep2,balance) ) return SeparatorEvaluators::isBalanced(sep1,balance);
    }

    return
    min( estimateDepthBasedOnEdges(sep1), estimateDepthBasedOnNodes(sep1) )
    <
    min( estimateDepthBasedOnEdges(sep2), estimateDepthBasedOnNodes(sep2) );
}

bool SeparatorEvaluators::isBalanced(const Separator &sep, double balance) {
//    double nodeRatio = (double) sep.stats.maxCompSize / sep.V->size();
    double nodeRatio = (double) sep.stats.maxCompSize / sep.stats.originalGraphSize;
//    double nodeRatio = (double) sep.stats.maxCompSize / sep.stats.originalGraphSize;
    if( nodeRatio <= balance ) return true;

    double edgeRatio = (double) sep.stats.maxCompEdges / sep.stats.originalGraphEdges;
    if( edgeRatio <= balance ) return true;

    return false;
}



