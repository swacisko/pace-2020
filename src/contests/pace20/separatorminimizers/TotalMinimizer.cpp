/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <contests/pace20/separatorminimizers/TotalMinimizer.h>
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
        if (cnf.sep_minim_to_use_mask & (1<<SepMinim::NeighVCCMinim)) {
            if ( cnf.write_logs && cnf.cur_rec_depth == 0 ) clog << "\t running NeighVCCMinim" << endl;
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
            if ( cnf.write_logs && cnf.cur_rec_depth == 0 ) clog << "\t running LargestComponentsVCMinimizer" << endl;
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
        }


        //  BFS minimizer
        if( cnf.sw.tle("main") ) return bestSep;
        if (cnf.sep_minim_to_use_mask & (1<<SepMinim::BfsMinim)) {
            if ( cnf.write_logs && cnf.cur_rec_depth == 0 ) clog << "\t running BfsMinim" << endl;
            if( debug ) DEBUG(index);
            index++; if( foundBetterIndex == index ) break;
            {
                BFSMinimizer bfsMin(cnf);
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
                    if( estDepth( bestSep ) - estDepth(bfsSep) < 1 ){ bestSep = bfsSep; break; }

                    bestSep = bfsSep;
                    bfsSep = bfsMin.minimizeSeparator(bestSep);
                    bfsSep.updatePointers(*V);
                    foundBetterIndex = index;
                    if( cnf.sw.tle("main") ) return bestSep;
                }
            }
        }

        if( cnf.sw.tle("main") ) return bestSep;
        if (cnf.sep_minim_to_use_mask & (1<<SepMinim::FlowMinim)) {
            if ( cnf.write_logs && cnf.cur_rec_depth == 0 ) clog << "\t running FlowMinim" << endl;
            if( debug ) DEBUG(index);
            index++; if( foundBetterIndex == index ) break;
            if (bestSep.stats.size <= cnf.max_separator_size_for_flow_minimizer) {
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
        }



        if( cnf.sw.tle("main") ) return bestSep; // disabling flow-cutter distance minimizer - we use furthest-point minimization at the end
        if (cnf.sep_minim_to_use_mask & (1<<SepMinim::FlowCutterDstMinim)) {
            if( debug ) DEBUG(index);
            index++; if( foundBetterIndex == index ) break;
            int est_td = SeparatorEvaluators::estimateDepthBasedOnEdges(bestSep) + SeparatorEvaluators::estimateDepthBasedOnNodes( bestSep );
            if( est_td <= cnf.max_estimated_treedepth_for_flowcutter ) {
                if(debug) clog << "FC minimizer" << endl;

                FlowCutterMinimizer fcMin(sepEval, cnf);
                auto fcSep = fcMin.minimizeSeparator(bestSep);
                fcSep.updatePointers(*V);

                while (  (*sepEval)(fcSep, bestSep) && !(*sepEval)(bestSep, fcSep)  ) {
                    if(debug){ clog << "fcMinimizer makes beter solution!" << endl; DEBUG(bestSep); DEBUG(fcSep); ENDL(1); }
                    bestSep = fcSep;
                    fcSep = fcMin.minimizeSeparator(bestSep);
                    fcSep.updatePointers(*V);
                    foundBetterIndex = index;
                    if( cnf.sw.tle("main") ) return bestSep;
                }

                if(debug) clog << "Leaving FC minimizer" << endl;
            }
        }




        if( cnf.sw.tle("main") ) return bestSep;
        if (cnf.sep_minim_to_use_mask & (1<<SepMinim::ExpansionMinim)) {
            if ( cnf.write_logs && cnf.cur_rec_depth == 0 ) clog << "\t running ExpansionMinim" << endl;
            if( debug ) DEBUG(index);
            index++; if( foundBetterIndex == index ) break;
            {
                ExpansionMinimizer exMin(cnf);
                auto exSep = exMin.minimizeSeparator(bestSep);
                exSep.updatePointers(*V);

                while (  (*sepEval)(exSep, bestSep) && ! (*sepEval)(bestSep, exSep)  ) {
                    if(debug){
                        clog << "exMinimizer makes beter solution!" << endl;
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
    if ( (cnf.sep_minim_to_use_mask & (1<<SepMinim::GNEMinim)) && bestSep.stats.size <= cnf.max_separator_size_for_GNE_minimizer) {
        if ( cnf.write_logs && cnf.cur_rec_depth == 0 ) clog << "\t running GNEMinim" << endl;
        auto opt = (cnf.minimize_nodes_iteration ? GreedyNodeEdgeMinimizer::MINIMIZE_NODES : GreedyNodeEdgeMinimizer::MINIMIZE_EDGES);
        GreedyNodeEdgeMinimizer gneMin( cnf, opt );
        gneMin.sepEval = sepEval;

        auto gneSep = gneMin.minimizeSeparator(bestSep);
        gneSep.updatePointers(*V);

        while ((*sepEval)(gneSep, bestSep)) {
            if(debug){ cerr << "gneMinimizer makes beter solution!" << endl; DEBUG(bestSep); DEBUG(gneSep); ENDL(1); }
            bestSep = gneSep;
            gneSep = gneMin.minimizeSeparator(bestSep);
            gneSep.updatePointers(*V);
            foundBetterIndex = index;
            if ( cnf.sw.tle("main") ) return bestSep;
        }
    }




    if( cnf.sep_minim_to_use_mask & (1<<SepMinim::FlowCutterMinim) ){
        if ( cnf.write_logs && cnf.cur_rec_depth == 0 ) clog << "\t running FlowCutterMinim" << endl;
        FlowCutterMinimizer fcMin(sepEval,cnf);
        fcMin.MINIMIZATION_MODE = FlowCutterMinimizer::FURTHEST_POINT_MINIMIZATION;
        auto fcSep = fcMin.minimizeSeparator(bestSep);
        fcSep.updatePointers(*V);

        while (  (*sepEval)(fcSep, bestSep) && !(*sepEval)(bestSep, fcSep)  ) {
            if(debug){
                clog << "fcMinimizer with FPM mode makes better solution after whole minimization!" << endl;
                DEBUG(bestSep); DEBUG(fcSep); ENDL(1);
            }

            bestSep = fcSep; bestSep.updatePointers(*V);
            fcSep = fcMin.minimizeSeparator(bestSep);
            fcSep.updatePointers(*V);
            if ( cnf.sw.tle("main") ) return bestSep;
        }
    }

    if(debug) ENDL(1);

    return bestSep;
}

void TotalMinimizer::test() {


    exit(1);
}


