/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <utils/TimeMeasurer.h>
#include <graphs/graphtraversals/BFS.h>

#include "contests/pace20/Pace20.h"
#include <sys/resource.h>

#include "GraphReader.h"
#include "GraphUtils.h"
#include "SeparatorEvaluators.h"
#include "StandardUtils.h"

#include "contests/pace20/DepthTreeIrrelevantNodeShifter.h"
#include "contests/pace20/DTKernelizer.h"
#include "contests/pace20/ImbalancedTreeImprover.h"


namespace Pace20{

    void run( int argc, char **argv  ){

        Config cnf{};
        cnf.max_time_millis = 60'000;
        cnf.startMain();

        VVI V = GraphReader::readGraphDIMACSWunweighed(cin,false);

        DEBUG(V.size());
        DEBUG( GraphUtils::countEdges(V) );


        VVI initKernV; // this is initially kernelized V - V after subgraph kernelization, before deg3 kernelization, since deg3 kernelization may yield different results depending on nodes in IS
        DTKernelizer initKernelizer(V,cnf);
        bool useInitialKernelization = true;

        if(useInitialKernelization){
            cerr << "Initial kernelization in progress" << endl;
            initKernV = initKernelizer.getKernelizedGraphSubgraphs(); // harder kernelization
            cerr << "Initial kernelization done" << endl;
        }



        int reps = 50;
        DepthTree bestTree(V);
        bestTree.height = V.size();
        VI treeHeights;



        if( V.size() < 500 ) {
            reps = 10'000;
            cnf.sep_cr_max_sources = 10;
        }
        if( V.size() < 1'000 ){
            reps = 10'000;
            cnf.sep_cr_max_sources = 10;
        }else if( V.size() < 10'000 ){
            reps = 2'000;
        }else if( V.size() < 100'000 ){
            reps = 500;
        }
        else if( V.size() < 300'000 ){
            cnf.sep_cr_max_sources = 4;
            cnf.max_best_seps_for_minimizers = 5;
            reps = 100;
        }
        else {
            cnf.sep_cr_max_sources = 4;
            cnf.max_best_seps_for_minimizers = 5;
            reps = 20;
        }


        if( GraphUtils::countEdges(V) < 1'000 ) cnf.max_best_seps_for_recursion = 8;
        else if( GraphUtils::countEdges(V) < 2'000 ) cnf.max_best_seps_for_recursion = 6;
        else if( GraphUtils::countEdges(V) < 3'000 ) cnf.max_best_seps_for_recursion = 4;



        for(int r=0; r<reps; r++){

            if( r >= 5 ){
                cnf.min_graph_size_for_kernelization = 100 + 150 * (1+( r % 5 )) * (1+( r % 5 ));
                if( r % 4 == 0 ) cnf.min_graph_size_for_kernelization = Constants::INF;

                if( r % 12 >= 5 && r%12 <= 7)  cnf.sep_balance = 0.5 + (double)(rand()%900) / 2000;
                else cnf.sep_balance = 0.97;
            }


            cerr << "\rRepetition #" << r << " / " << reps << flush;


            if( r > 1 ){
                int opt = r%3;
                if( opt == 2 ){
                    SeparatorEvaluators::nodeScaleFactor = 0.95; SeparatorEvaluators::edgeScaleFactor = 0.05;
                    cnf.minimize_nodes_iteration = true;
                }
                else if(opt==0){
                    SeparatorEvaluators::nodeScaleFactor = 0.05; SeparatorEvaluators::edgeScaleFactor = 0.95;
                    cnf.minimize_nodes_iteration = false;
                }else{
                    SeparatorEvaluators::nodeScaleFactor = 0.5; SeparatorEvaluators::edgeScaleFactor = 0.5;
                    if( opt&1 ) cnf.minimize_nodes_iteration = false;
                    else cnf.minimize_nodes_iteration = true;
                }
            }


            useInitialKernelization = ( cnf.preprocessing_to_use_mask != Prepr::NoPrepr && V.size() >= cnf.min_graph_size_for_kernelization );
            DepthTreeCreatorLarge *creator = nullptr;
            if( useInitialKernelization ) creator = new DepthTreeCreatorLarge( initKernV ,0, cnf );
            else creator = new DepthTreeCreatorLarge( V ,0, cnf );

            DepthTree dtree = creator->getDepthTree(); // creating tree

            if( useInitialKernelization ){ dtree = initKernelizer.dekernelizeSubgraphs(dtree); dtree.V = &V; } // dekernelizing if used initially kernelized V

            cerr << endl << endl << "  Iteration tree height: " << dtree.height << endl << endl << endl;
            treeHeights.push_back( dtree.height );


            if( dtree.height < bestTree.height ){
                auto temp = dtree;
                swap( bestTree, temp );
            }

            if(cnf.sw.tle("main")) break;
            if( r > 0 && V.size() < 5'000 && dtree.height < bestTree.height + 3 ){
                ImbalancedTreeImprover improver(cnf);  cerr << "improving imbalanced tree if possible" << endl;
                SeparatorEvaluators::nodeScaleFactor = 0.4;
                SeparatorEvaluators::edgeScaleFactor = 0.6;
                cnf.minimize_nodes_iteration = false;
                auto impDt = improver.improve(dtree);  DEBUG(impDt.height);
                if( impDt.height < bestTree.height ) { auto temp = impDt; swap( bestTree, temp ); }
                if( impDt.height < dtree.height ) swap( dtree, impDt );
            }


            cerr << endl << endl << "  Iteration tree height after improvements: " << dtree.height << endl << endl << endl;
            if( creator != nullptr ){ delete creator; creator = nullptr;}

            if(cnf.sw.tle("main")) break;

            IntGenerator rnd;
            for( VI& v : V ) StandardUtils::shuffle(v,rnd);
        }

        DEBUG(bestTree.height);
        bestTree.write();

        cnf.sw.writeAll();
    }


}