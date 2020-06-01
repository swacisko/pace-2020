//
// Created by sylwester on 3/16/20.
//

#include <utils/TimeMeasurer.h>
#include <graphs/graphtraversals/BFS.h>

#include "CONTESTS/PACE20/Pace20.h"

#include "CONTESTS/PACE20/separatorcreators/FlowSeparatorCreator.h"
#include "CONTESTS/PACE20/separatorcreators/ArtPointSeparatorCreator.h"
#include "CONTESTS/PACE20/separatorcreators/ComponentExpansionSeparatorCreator.h"
#include "CONTESTS/PACE20/separatorcreators/FlowCutter.h"
#include "CONTESTS/PACE20/separatorcreators/CliqueSeparatorCreator.h"
#include "CONTESTS/PACE20/separatorcreators/NodeAddOrderSeparatorCreator.h"

#include "CONTESTS/PACE20/depthtreecreators/DepthTreeCreatorExact.h"

#include "CONTESTS/PACE20/separatorminimizers/GreedyNodeEdgeMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/NeighborhoodVCMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/SnapToNonpathNodesMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/BFSMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/LargestComponentsVCMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/ExpansionMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/TotalMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/FlowMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/FlowCutterMinimizer.h"

#include "CONTESTS/PACE20/DepthTreeIrrelevantNodeShifter.h"
#include "CONTESTS/PACE20/ComponentTreeMerger.h"
#include "CONTESTS/PACE20/DepthTreePivotMaker.h"
#include "CONTESTS/PACE20/TotalPivotMaker.h"
#include "CONTESTS/PACE20/DTKernelizer.h"
#include "CONTESTS/PACE20/DTKernelizerDeg3.h"
#include "CONTESTS/PACE20/DTKernelizerDeg4.h"
#include "CONTESTS/PACE20/ImbalancedTreeImprover.h"
#include "CONTESTS/PACE20/SubtreeRerunnerImprover.h"

#include "graphs/components/ConnectedComponents.h"
#include "graphs/VertexCover/VCUtils.h"
#include "graphs/generators/GraphGenerator.h"


namespace Pace20{

    DepthTree *volatile globalBestTree = nullptr;


    void addSigtermCheck(){
        struct sigaction action;
        memset(&action, 0, sizeof(struct sigaction));
        action.sa_handler = Pace20Params::terminate;
        sigaction(SIGTERM, &action, NULL);
    }

    void increaseStack(){
//        const rlim_t kStackSize = 256L * 1024L * 1024L;   // min stack size = 64 Mb
        const rlim_t kStackSize = 4L * 256L * 1024L * 1024L;   // min stack size = 64 Mb
        struct rlimit rl;
        int result;

        result = getrlimit(RLIMIT_STACK, &rl);
        if (result == 0)
        {
            if (rl.rlim_cur < kStackSize)
            {
                rl.rlim_cur = kStackSize;
                result = setrlimit(RLIMIT_STACK, &rl);
                if (result != 0)
                {
                    fprintf(stderr, "setrlimit returned result = %d\n", result);
                }
            }
        }
    }



    void run( int argc, char **argv  ){
        TimeMeasurer::stopMeasurement("PACE20");


        addSigtermCheck();
        increaseStack();

//        TotalPivotMaker::test();
//        DepthTreeCreatorExact::test();
//        DepthTreePivotMaker::test();
//        SubtreeRerunnerImprover::test();
//        DepthTreeCreatorLarge::test();
//        ImbalancedTreeImprover::test();
//        DepthTreeFromTreeWidth::test();
//        FlowCutterMinimizer::test();
//        FlowCutter::test();
//        DTKernelizerDeg3::test();
//        DTKernelizerDeg4::test();
//        TreewidthSeparatorCreator::test();
//        DepthTreeIrrelevantNodeShifter::test();
//        BFSMinimizer::test();
//        ExpansionMinimizer::test();
//        CliqueSeparatorCreator::test();
//        TotalMinimizer::test();
//        ComponentExpansionSeparatorCreator::test();
//        NodeAddOrderSeparatorCreator::test();
//        FlowMinimizer::test();
//        SnapToNonpathNodesMinimizer::test();
//        DTKernelizer::test();
//        GreedyNodeEdgeMinimizer::test();
//        FlowSeparatorCreator::test();
//        ComponentTreeMerger::test();
//        RemovalOrderSeparatorCreator::test();
//        ArtPointSeparatorCreator::test();
//        NeighborhoodVCMinimizer::test();
//        DepthTree::test();
//        LargestComponentsVCMinimizer::test();

//        string filename = "heur_101.gr";
//        fstream str(filename);
//        VVI V = GraphReader::readGraphDIMACSWunweighed(str,false);

        VVI V = GraphReader::readGraphDIMACSWunweighed(cin,false);

        DEBUG(V.size());
        DEBUG( GraphUtils::countEdges(V) );

        Pace20Params::inputGraphSize = V.size();
        Pace20Params::inputGraphEdges = GraphUtils::countEdges(V);


        VVI initKernV; // this is initially kernelized V - V after subgraph kernelization, before deg3 kernelization, since deg3 kernelization may yield different results depending on nodes in IS
        DTKernelizer initKernelizer(V);
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
            Pace20Params::maxSources = 10;
//            Pace20Params::maxBestSepsForMinimizers = 15;
        }
        if( V.size() < 1'000 ){
            reps = 10'000;
            Pace20Params::maxSources = 10;
//            Pace20Params::maxBestSepsForMinimizers = 15;
        }else if( V.size() < 10'000 ){
            reps = 2'000;
//            Pace20Params::maxBestSepsForMinimizers = 10;
        }else if( V.size() < 100'000 ){
            reps = 500;
        }
        else if( V.size() < 300'000 ){
            Pace20Params::maxSources = 4;
            Pace20Params::maxBestSepsForMinimizers = 5;
//            Pace20Params::balance = 0.9;
//            Pace20Params::maxSeparatorSizeForGNEMinimizer = 350; // #TEST
            reps = 100;
        }
        else /*if( V.size() >= 300'000 )*/{
            Pace20Params::maxSources = 4;
            Pace20Params::maxBestSepsForMinimizers = 5;
//            Pace20Params::balance = 0.9;
//            Pace20Params::maxSeparatorSizeForGNEMinimizer = 350; // #TEST
            reps = 20;
        }


        if( GraphUtils::countEdges(V) < 1'000 ) Pace20Params::maxBestSepsForRecursion = 8;
        else if( GraphUtils::countEdges(V) < 2'000 ) Pace20Params::maxBestSepsForRecursion = 6;
        else if( GraphUtils::countEdges(V) < 3'000 ) Pace20Params::maxBestSepsForRecursion = 4;


//        reps = 10;

//            Pace20Params::maxSources = 30;
//            Pace20Params::maxBestSepsForMinimizers = 2;
//            Pace20Params::minGraphSizeForKernelization = 1e9;


        for(int r=0; r<reps; r++){

            if( r >= 5 ){
                Pace20Params::minGraphSizeForKernelization = 100 + 150 * (1+( r % 5 )) * (1+( r % 5 ));
                if( r % 4 == 0 ) Pace20Params::minGraphSizeForKernelization = Constants::INF;

                if( r % 12 >= 5 && r%12 <= 7)  Pace20Params::balance = 0.5 + (double)(rand()%900) / 2000;
                else  Pace20Params::balance = 0.97;
            }


            cerr << "\rRepetition #" << r << " / " << reps << flush;


            if( r > 1 ){ // first one is qucikAndWeak, second one is with default set parameters, the rest are altered
                int opt = r%3;
                if( opt == 2 ){
                    SeparatorEvaluators::sepEvalToUse = SeparatorEvaluators::estimatedDepthTreeEdgePlusNode;
                    SeparatorEvaluators::nodeScaleFactor = 0.95; SeparatorEvaluators::edgeScaleFactor = 0.05;
                    Pace20Params::minimizeNodesIteration = true;
                }
                else if(opt==0){
                    SeparatorEvaluators::sepEvalToUse = SeparatorEvaluators::estimatedDepthTreeEdgePlusNode;
                    SeparatorEvaluators::nodeScaleFactor = 0.05; SeparatorEvaluators::edgeScaleFactor = 0.95;
                    Pace20Params::minimizeNodesIteration = false;
                }else{
                    SeparatorEvaluators::sepEvalToUse = SeparatorEvaluators::estimatedDepthTreeEdgePlusNode;
                    SeparatorEvaluators::nodeScaleFactor = 0.5; SeparatorEvaluators::edgeScaleFactor = 0.5;
                    if( opt&1 ) Pace20Params::minimizeNodesIteration = false;
                    else Pace20Params::minimizeNodesIteration = true;
                }
            }


            useInitialKernelization = ( Pace20Params::useKernelization && V.size() >= Pace20Params::minGraphSizeForKernelization );
            DepthTreeCreatorLarge *creator = nullptr;
            if( useInitialKernelization ) creator = new DepthTreeCreatorLarge( initKernV ,0 );
            else{
                creator = new DepthTreeCreatorLarge( V ,0 );
            }

            if( Pace20Params::quickAndWeakTreeCreation ){
                creator->MINIMIZE_SEPARATORS = false;
                creator->setSeparatorCreatorsMode( DepthTreeCreatorLarge::COMP_EXP_CREATOR );
            }
            else if( globalBestTree != nullptr && globalBestTree->height < 50 ){
                creator->USE_DEG3_KERNELIZATION = false; // #TEST disabling deg3 kernelization for graphs with small treedepth
            }


            DepthTree dtree = creator->getDepthTree(); // creating tree

            if( useInitialKernelization ){ dtree = initKernelizer.dekernelizeSubgraphs(dtree); dtree.V = &V; } // dekernelizing if used initially kernelized V

            cerr << endl << endl << "  Iteration tree height: " << dtree.height << endl << endl << endl;
            treeHeights.push_back( dtree.height );


            if( dtree.height < bestTree.height ){
                Pace20Params::outputWriterLock.lock();
                auto temp = dtree;
                swap( bestTree, temp );
                globalBestTree = &bestTree;
                Pace20Params::outputWriterLock.unlock();
            }

            if(Pace20Params::tle) break;
            if( r > 0 && V.size() < 5'000 && dtree.height < bestTree.height + 3 ){
                ImbalancedTreeImprover improver;  cerr << "improving imbalanced tree if possible" << endl;
                SeparatorEvaluators::nodeScaleFactor = 0.4; SeparatorEvaluators::edgeScaleFactor = 0.6; Pace20Params::minimizeNodesIteration = false;
                auto impDt = improver.improve(dtree);  DEBUG(impDt.height);
                if( impDt.height < bestTree.height ) {
                    Pace20Params::outputWriterLock.lock();
                    auto temp = impDt; swap( bestTree, temp ); globalBestTree = &bestTree;
                    Pace20Params::outputWriterLock.unlock();
                }

                if( impDt.height < dtree.height ) swap( dtree, impDt );
            }

           /* {// TOTAL PIVOT MAKER SECTION, so far it fails some assertions assert( dt.isCorrect() )
                TotalPivotMaker totMaker(dtree,0); cerr << "totalPivotMAker in action" << endl;
                auto impDt = totMaker.makePivots();
                assert( impDt.height <= dtree.height );
                if( impDt.height < bestTree.height ) {
                    cerr << "Total pivot maker makes still better, impDt.height = " << impDt.height << " < " << dtree.height << " = dtree.height" << endl;
                    Pace20Params::outputWriterLock.lock();
                    auto temp = impDt; swap( bestTree, temp ); globalBestTree = &bestTree;
                    Pace20Params::outputWriterLock.unlock();
                }
                if( impDt.height < dtree.height ) swap( dtree, impDt );

            }*/

           /* if( !Pace20Params::quickAndWeakTreeCreation ){
                ENDL(5);
                SubtreeRerunnerImprover improver;
                auto impDt = improver.improve( dtree,0.2 );
                DEBUG(impDt.height);
//                while( impDt.height < bestTree.height ){
                if( impDt.height < bestTree.height ){
                    Pace20Params::outputWriterLock.lock();
                    auto temp = impDt; swap( bestTree, temp ); globalBestTree = &bestTree;
                    Pace20Params::outputWriterLock.unlock();
//                    impDt = improver.improve( dtree,0.3 ); // version with while, but repetitive improvement is already in improver.improve()
                }
                cerr << "After improvements, iteration tree height: " << impDt.height << endl;
                ENDL(5);
            }*/


            cerr << endl << endl << "  Iteration tree height after improvements: " << dtree.height << endl << endl << endl;
            if( creator != nullptr ){ delete creator; creator = nullptr;}


            if(Pace20Params::tle) break;

            for( VI& v : V ) random_shuffle(ALL(v));

            Pace20Params::quickAndWeakTreeCreation = false;

            if( globalBestTree->height > 1500 ) Pace20Params::balance = 0.6; /* #TEST for tests with large depth we force balance in first iterations*/
            if( Pace20Params::inputGraphSize > 100'000 && globalBestTree->height > 300 ) Pace20Params::minGraphSizeForKernelization = 300; // #TEST

        }

        DEBUG(bestTree.height);

        { // write answer - this section probabyl will no be reached, since answer will be written in teminate() function after receiving SIGTERM
            Pace20Params::outputWriterLock.lock();
            bestTree.write();
//            Pace20Params::outputWriterLock.unlock(); // if we wrote an answer we do not release the lock
        }

        DEBUG(bestTree.height);


        TimeMeasurer::stopMeasurement("PACE20");
        TimeMeasurer::writeAllMeasurements();

    }


}