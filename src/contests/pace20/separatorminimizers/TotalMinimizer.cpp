/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <contests/pace20/separatorminimizers/TotalMinimizer.h>
#include <contests/pace20/Pace20Params.h>
#include <contests/pace20/separatorminimizers/SnapToNonpathNodesMinimizer.h>
#include <contests/pace20/separatorminimizers/FlowMinimizer.h>
#include <contests/pace20/separatorminimizers/LargestComponentsVCMinimizer.h>
#include <contests/pace20/separatorminimizers/BFSMinimizer.h>
#include <contests/pace20/separatorminimizers/NeighborhoodVCMinimizer.h>
#include <contests/pace20/separatorminimizers/ExpansionMinimizer.h>
#include <contests/pace20/SeparatorEvaluators.h>
#include <contests/pace20/separatorminimizers/GreedyNodeEdgeMinimizer.h>
#include <contests/pace20/separatorminimizers/FlowCutterMinimizer.h>
#include <graphs/components/ConnectedComponents.h>
#include <utils/StandardUtils.h>

TotalMinimizer::TotalMinimizer(SeparatorEvaluator *eval,  Config c) : SeparatorMinimizer(c) {
    sepEval = eval;
}


Separator TotalMinimizer::minimizeSeparator(Separator bestSep) {
    bool debug = false;

    VVI* V = bestSep.V;
    if( cnf.sw.tle("main") ) return bestSep;

    int foundBetterIndex = -1;
    int index = -1;


    while(true){
        index = -1;

        if( cnf.sw.tle("main") ) return bestSep;
        if( debug ) DEBUG(index);
        index++; if( foundBetterIndex == index ) break;
        {
            NeighborhoodVCMinimizer vcmin(cnf);
            auto vcSep = vcmin.minimizeSeparator(bestSep);
            vcSep.updatePointers(*V);

            while ((*sepEval)(vcSep, bestSep) && !(*sepEval)(bestSep, vcSep)) {
                if(debug){
                    cerr << "vcMinimizer makes beter solution!" << endl;
                    DEBUG(bestSep);
                    DEBUG(vcSep);
                    ENDL(1);
                }
                bestSep = vcSep;
                vcSep = vcmin.minimizeSeparator(bestSep);
                vcSep.updatePointers(*V);
                foundBetterIndex = index;
                if( cnf.sw.tle("main") ) return bestSep;
            }
        }



        if( cnf.sw.tle("main") ) return bestSep;
        if( debug ) DEBUG(index);
        index++; if( foundBetterIndex == index ) break;
        {
            LargestComponentsVCMinimizer minim(cnf);
            auto lcSep = minim.minimizeSeparator(bestSep);
            lcSep.updatePointers(*V);

            while (  (*sepEval)(lcSep, bestSep) && !(*sepEval)(bestSep, lcSep)  ) {
                if(debug){
                    cerr << "lcMinimizer makes beter solution!" << endl;
                    DEBUG(bestSep);
                    DEBUG(lcSep);
                    ENDL(1);
                }
                bestSep = lcSep;
                lcSep = minim.minimizeSeparator(bestSep);
                lcSep.updatePointers(*V);
                foundBetterIndex = index;
                if( cnf.sw.tle("main") ) return bestSep;
            }

        }


        //  BFS minimizer should be uncommmented
        /*if( Pace20Params::tle ) return bestSep;
        if( debug ) DEBUG(index);
        index++; if( foundBetterIndex == index ) break;
        {
            BFSMinimizer bfsMin;
            auto bfsSep = bfsMin.minimizeSeparator(bestSep);
            bfsSep.updatePointers(*V);

            while ((*sepEval)(bfsSep, bestSep) && !(*sepEval)(bestSep, bfsSep) ) {
                if(debug){
                    cerr << "bfsMinimizer makes beter solution!" << endl;
                    DEBUG(bestSep);
                    DEBUG(bfsSep);
                    ENDL(1);
                }

                auto estDepth = SeparatorEvaluators::estimateDepthBasedOnEdges;
                if( estDepth( bestSep ) - estDepth(bfsSep) < 1 ){
                    bestSep = bfsSep;
                    break;
                }

                bestSep = bfsSep;
                bfsSep = bfsMin.minimizeSeparator(bestSep);
                bfsSep.updatePointers(*V);
//                foundBetter = true;
                foundBetterIndex = index;
                if (Pace20Params::tle) return bestSep;
            }
        }*/

        if( cnf.sw.tle("main") ) return bestSep;
        if( debug ) DEBUG(index);
        index++; if( foundBetterIndex == index ) break;
        if (bestSep.stats.size <= Pace20Params::maxSeparatorSizeForFlowMinimizer) {
            FlowMinimizer fmin(cnf);
            auto fSep = fmin.minimizeSeparator(bestSep);
            fSep.updatePointers(*V);

            while ( (*sepEval)(fSep, bestSep) && !(*sepEval)(bestSep, fSep)) {
                if(debug){
                    cerr << "fMinimizer makes beter solution!" << endl;
                    DEBUG(bestSep);
                    DEBUG(fSep);
                    ENDL(1);
                }
                bestSep = fSep;
                fSep = fmin.minimizeSeparator(bestSep);
                fSep.updatePointers(*V);
                foundBetterIndex = index;
                if ( cnf.sw.tle("main") ) return bestSep;
            }
        }


        /*if( Pace20Params::tle ) return bestSep; // disabling flow-cutter distance minimizer - we use furthest-point minimization at the end
        if( debug ) DEBUG(index);
        index++; if( foundBetterIndex == index ) break;
        if( Pace20Params::inputGraphEdges <= 100'000
            && SeparatorEvaluators::estimateDepthBasedOnEdges(bestSep) + SeparatorEvaluators::estimateDepthBasedOnNodes( bestSep ) < 1'000 ) {
            if(debug) cerr << "FC minimizer" << endl;

            FlowCutterMinimizer fcMin(*sepEval);
            auto fcSep = fcMin.minimizeSeparator(bestSep);
            fcSep.updatePointers(*V);

            while (  (*sepEval)(fcSep, bestSep) && !(*sepEval)(bestSep, fcSep)  ) {
                if(debug){
                    cerr << "fcMinimizer makes beter solution!" << endl;
                    DEBUG(bestSep);
                    DEBUG(fcSep);
                    ENDL(1);
                }


                bestSep = fcSep;
                fcSep = fcMin.minimizeSeparator(bestSep);
                fcSep.updatePointers(*V);
//                foundBetter = true;
                foundBetterIndex = index;
                if (Pace20Params::tle) return bestSep;
            }

            if(debug) cerr << "Leaving FC minimizer" << endl;
        }*/




        if( cnf.sw.tle("main") ) return bestSep;
        if( debug ) DEBUG(index);
        index++; if( foundBetterIndex == index ) break;
        {
            ExpansionMinimizer exMin(cnf);
            auto exSep = exMin.minimizeSeparator(bestSep);
            exSep.updatePointers(*V);

            while (  (*sepEval)(exSep, bestSep) && ! (*sepEval)(bestSep, exSep)  ) {
                if(debug){
                    cerr << "exMinimizer makes beter solution!" << endl;
                    DEBUG(bestSep);
                    DEBUG(exSep);
                    ENDL(1);
                }

                auto estDepth = SeparatorEvaluators::estimateDepthBasedOnEdges;
                if( V->size() > 1'000 && estDepth( bestSep ) - estDepth(exSep) < 1 ){
                    bestSep = exSep;
                    break;
                }

                bestSep = exSep;
                exSep = exMin.minimizeSeparator(bestSep);
                exSep.updatePointers(*V);
                foundBetterIndex = index;
                if ( cnf.sw.tle("main") ) return bestSep;
            }
        }

        if( debug ) DEBUG(foundBetterIndex);
        if( foundBetterIndex == -1 ) break;

        if( oneRoundMinimization ){
            cerr << "oneRoundMinimization" << endl;
            DEBUG(foundBetterIndex);
            break;
        }
    }


    if( cnf.sw.tle("main") ) return bestSep;
    if (bestSep.stats.size <= Pace20Params::maxSeparatorSizeForGNEMinimizer) {
        GreedyNodeEdgeMinimizer gneMin( cnf, Pace20Params::minimizeNodesIteration ? GreedyNodeEdgeMinimizer::MINIMIZE_NODES : GreedyNodeEdgeMinimizer::MINIMIZE_EDGES );
        gneMin.sepEval = sepEval;

        auto gneSep = gneMin.minimizeSeparator(bestSep);
        gneSep.updatePointers(*V);

        while ((*sepEval)(gneSep, bestSep)) {
            if(debug){
                cerr << "gneMinimizer makes beter solution!" << endl;
                DEBUG(bestSep);
                DEBUG(gneSep);
                ENDL(1);
            }
            bestSep = gneSep;
            gneSep = gneMin.minimizeSeparator(bestSep);
            gneSep.updatePointers(*V);
            foundBetterIndex = index;
            if ( cnf.sw.tle("main") ) return bestSep;
        }
    }



    if( V->size() > 5 && Pace20Params::inputGraphEdges < 10'000 ){ // only for small input graphs - it probably is not very effective, just shifts single vertices
        bool improved = true;

        while(improved) {
            improved = false;

            VI nodes = bestSep.nodes;
            VVI comps = ConnectedComponents::getConnectedComponents(*V, nodes);
            sort(ALL(comps), [](auto &v, auto &w) { return v.size() > w.size(); });
            VB inLargest = StandardUtils::toVB(V->size(), comps[0]);

            int cnt = 0;
            VI toChange;
            for (int p : nodes) {
                for (int d : (*V)[p]) {
                    if (inLargest[d]) cnt++;
                }

                if (cnt == 1) toChange.push_back(p);
            }

            if (!toChange.empty()) {
                VI diff;
                sort(ALL(nodes));
                sort(ALL(toChange));
                set_difference(ALL(nodes), ALL(toChange), back_inserter(diff));
                for (int p : toChange) for (int d : (*V)[p]) if (inLargest[d]) diff.push_back(d);

                auto newSep = Separator(*V, diff);
                newSep.createSeparatorStats();

                if ((*sepEval)(newSep, bestSep) && !(*sepEval)(bestSep, newSep)) {
                    bestSep = newSep;
                    bestSep.updatePointers(*V);
                    improved = true;
                }
            }
        }
    }


    if( bestSep.V->size() < 10'000 && bestSep.V->size() > 100 ){
        FlowCutterMinimizer fcMin(sepEval,cnf);
        fcMin.MINIMIZATION_MODE = FlowCutterMinimizer::FURTHEST_POINT_MINIMIZATION;
        auto fcSep = fcMin.minimizeSeparator(bestSep);
        fcSep.updatePointers(*V);

        while (  (*sepEval)(fcSep, bestSep) && !(*sepEval)(bestSep, fcSep)  ) {
            if(debug){
                cerr << "fcMinimizer with FPM mode makes beter solution after whole minimization!" << endl;
                DEBUG(bestSep);
                DEBUG(fcSep);
                ENDL(1);
            }

            bestSep = fcSep; bestSep.updatePointers(*V);
            fcSep = fcMin.minimizeSeparator(bestSep);
            fcSep.updatePointers(*V);
            if ( cnf.sw.tle("main") ) return bestSep;
        }
    }

    if(debug){
        ENDL(1);
    }

    return bestSep;
}

void TotalMinimizer::test() {


    exit(1);
}


