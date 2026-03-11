/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <contests/pace20/depthtreecreators/DepthTreeCreatorLarge.h>
#include <contests/pace20/DTKernelizer.h>
#include <Makros.h>
#include <contests/pace20/separatorcreators/BFSSeparatorCreator.h>
#include <contests/pace20/separatorcreators/FlowSeparatorCreator.h>
#include <contests/pace20/separatorcreators/ComponentExpansionSeparatorCreator.h>
#include <contests/pace20/separatorcreators/RemovalOrderSeparatorCreator.h>
#include <contests/pace20/separatorminimizers/GreedyNodeEdgeMinimizer.h>
#include <contests/pace20/separatorminimizers/SnapToNonpathNodesMinimizer.h>
#include <contests/pace20/separatorminimizers/FlowMinimizer.h>
#include <contests/pace20/SeparatorEvaluators.h>
#include <contests/pace20/DepthTreeIrrelevantNodeShifter.h>
#include <graphs/GraphInducer.h>
#include <contests/pace20/ComponentTreeMerger.h>
#include <contests/pace20/Pace20Params.h>
#include <graphs/flow/MaxFlow.h>
#include <graphs/landmarks/LandmarkCreator.h>
#include <contests/pace20/separatorminimizers/NeighborhoodVCMinimizer.h>
#include <contests/pace20/separatorminimizers/LargestComponentsVCMinimizer.h>
#include <contests/pace20/DepthTreePivotMaker.h>
#include <contests/pace20/separatorcreators/ArtPointSeparatorCreator.h>
#include <graphs/GraphUtils.h>
#include <graphs/cliques/CliqueExtension.h>
#include <graphs/trees/Tree.h>
#include <contests/pace20/separatorminimizers/TotalMinimizer.h>
#include <contests/pace20/separatorcreators/CliqueSeparatorCreator.h>
#include <graphs/vertex_cover/VCUtils.h>
#include <contests/pace20/DTKernelizerDeg3.h>
#include <contests/pace20/DTKernelizerDeg4.h>
#include <contests/pace20/separatorcreators/FlowCutter.h>

#include <graphs/vertex_cover/approximation/PartitionSVC.h>
#include <graphs/vertex_cover/SolutionVC.h>
#include <utils/StandardUtils.h>
#include <graphs/trees/Centroids.h>

#include "graphs/components/ConnectedComponents.h"
#include "graphs/components/BridgesAndArtPoints.h"
#include "datastructures/FAU.h"
#include "graphs/flow/DisjointPaths.h"
#include "graphs/flow/UnitFlow.h"

DepthTreeCreatorLarge::DepthTreeCreatorLarge(VVI &V, int recurrenceDepth, Config c, bool randomSepEval) : DepthTreeCreator(V,recurrenceDepth,c) {
    this->randomSepEval = randomSepEval;
}

DepthTree DepthTreeCreatorLarge::getDepthTree() {

    if( cnf.sw.tle("main") || ( Pace20Params::useOnlyArtPoints && recDepth > 50 ) ){
        // Pace20Params::temp += V->size();
        DepthTree dt(*V);
        dt.root = 0;
        dt.par[0] = -1;
        dt.height = V->size();
        for( int i=1; i < V->size(); i++ ) dt.par[i] = i-1;
        return dt;
    }

    if( V->size() == 1 ){
        // Pace20Params::temp += V->size();
        DepthTree dt(*V);
        dt.root = 0;
        dt.par[0] = -1;
        dt.height = 1;
        return dt;
    }


//    DEBUG(V->size());



    if( USE_KERNELIZATION ) {
        if (Pace20Params::useKernelization &&
            V->size() >= Pace20Params::minGraphSizeForKernelization /*&& recDepth < 3*/ && GraphUtils::countNodesWithDegree(*V, 1, 2) > 0) {
            DTKernelizer dtKernelizer(*V,cnf);
            VVI newV;

//            cerr << "kernelizing!" << endl;

//        GraphUtils::writeBasicGraphStatistics(*V);

            bool useSubgraphKernelization = true;


            if (useSubgraphKernelization) newV = dtKernelizer.getKernelizedGraphSubgraphs(); // harder kernelization
            else newV = dtKernelizer.getKernelizedGraph(); // soft kernelization - only dangling trees and paths

//        GraphUtils::writeBasicGraphStatistics(newV); //exit(1);
//        ENDL(5);

            if (newV.size() != V->size()) {
                DepthTree dt((*V));
                if (!newV.empty()) {
                    DepthTreeCreatorLarge dtCL(newV, recDepth,cnf);
                    dtCL.setSeparatorCreatorsMode(SEPARATOR_CREATORS_MODE);
                    dtCL.MINIMIZE_SEPARATORS = MINIMIZE_SEPARATORS;
                    dtCL.USE_KERNELIZATION = USE_KERNELIZATION; // here it will always be true, since we are in kernelization section
                    dtCL.USE_DEG3_KERNELIZATION = USE_DEG3_KERNELIZATION; // here it will always be true, since we are in kernelization section
                    dtCL.USE_DEG4_KERNELIZATION = USE_DEG4_KERNELIZATION; // here it will always be true, since we are in kernelization section

                    dt = dtCL.getDepthTree();
                    assert(dt.root >= 0 && dt.root < newV.size());
                } else dt.root = -1;


                DepthTree newDt(*V);
                if (useSubgraphKernelization) newDt = dtKernelizer.dekernelizeSubgraphs(dt);
                else newDt = dtKernelizer.dekernelize(dt);


                if (recDepth == 0 && newDt.height - dt.height > 0) {
                    cerr << "dt.height:    " << dt.height << endl;
                    cerr << "newDt.height: " << newDt.height << endl;
                    cerr << "height difference: " << newDt.height - dt.height << endl;
                    ENDL(1);
                }

                newDt.V = V;
                dt = newDt;

//                dt = DepthTreePivotMaker::makeAllPivots(dt); // maybe do not do that here (time consuming in case of small kernelizations)

                assert(dt.isCorrect());

                return dt;
            }


        } else if ( Pace20Params::useKernelization && USE_DEG3_KERNELIZATION && V->size() >= Pace20Params::minGraphSizeForKernelization /*  Pace20Params::minGraphSizeForDeg3Kernelization */ &&
                    recDepth == 0 && GraphUtils::countNodesWithDegree(*V, 3, 3) > 0) {
            DTKernelizerDeg3 ker(*V,cnf);
            VVI newV = ker.kernelize();
            DepthTree dt((*V));
            cerr << "Kernelized degree3 nodes, starting new DepthTreeCreatorLarge with recDepth = 1" << endl;
            DepthTreeCreatorLarge dtCL(newV, recDepth + 1,cnf);
            dtCL.setSeparatorCreatorsMode(SEPARATOR_CREATORS_MODE);
            dtCL.MINIMIZE_SEPARATORS = MINIMIZE_SEPARATORS;
            dtCL.USE_KERNELIZATION = USE_KERNELIZATION;
            dtCL.USE_DEG3_KERNELIZATION = false;
            dtCL.USE_DEG4_KERNELIZATION = USE_DEG4_KERNELIZATION;

            dt = dtCL.getDepthTree();
            dt = ker.dekernelize(dt);
            dt.V = V;

            dt = DepthTreePivotMaker::makeAllPivots(dt);

            assert(dt.isCorrect());

            return dt;
        } else if( USE_DEG4_KERNELIZATION && V->size() >= Pace20Params::minGraphSizeForKernelization && GraphUtils::countNodesWithDegree(*V, 4,4) > 0 ){
            DTKernelizerDeg4 ker(*V,cnf);
            VVI newV = ker.kernelize();
            DepthTree dt((*V));
            cerr << "Kernelized degree4 nodes" << endl;
            DepthTreeCreatorLarge dtCL(newV, recDepth,cnf);
            dtCL.setSeparatorCreatorsMode(SEPARATOR_CREATORS_MODE);
            dtCL.MINIMIZE_SEPARATORS = MINIMIZE_SEPARATORS;
            dtCL.USE_KERNELIZATION = USE_KERNELIZATION;
            dtCL.USE_DEG3_KERNELIZATION = false;
            dtCL.USE_DEG4_KERNELIZATION = false;

            dt = dtCL.getDepthTree();
            dt = ker.dekernelize(dt);
            dt.V = V;

            dt = DepthTreePivotMaker::makeAllPivots(dt);

            assert(dt.isCorrect());

            return dt;
        }
    }

    const int MAX_SOURCES = min( Pace20Params::maxSources, (int)V->size()-1 );


    auto sepEval = SeparatorEvaluators::sepEvalToUse;



    Separator bestSep(*V, VI(1,0) );
    bestSep.createSeparatorStats();

    vector<Separator> bestSeps( 1,bestSep );



    auto sortAndResizeSeparatorsForRecursion = [=,&bestSeps, &sepEval](){
        sort( ALL(bestSeps), sepEval );
        auto it = unique( ALL(bestSeps), [&sepEval]( Separator& s1, Separator& s2 ){
            return s1.stats.size * (s1.stats.maxCompSize+1) * (s1.stats.maxCompEdges+1) == s2.stats.size * (s2.stats.maxCompSize+1) * (s2.stats.maxCompEdges+1);
        } );

        int SS = min( (int)( it - bestSeps.begin() ), Pace20Params::maxBestSepsForRecursion);
        while( bestSeps.size() > SS ) bestSeps.pop_back();
        for(auto& sp : bestSeps) sp.updatePointers(*V);
    };

    auto sortAndResizeSeparatorsForMinimization = [=,&bestSeps, &sepEval](){
        sort( ALL(bestSeps), sepEval );
        auto it = unique( ALL(bestSeps), [&sepEval]( Separator& s1, Separator& s2 ){
            return 1ll * s1.stats.size * (s1.stats.maxCompSize+1) * (s1.stats.maxCompEdges+1) == 1ll * s2.stats.size * (s2.stats.maxCompSize+1) * (s2.stats.maxCompEdges+1);
        } );

        int SS = min( (int)( it - bestSeps.begin() ), Pace20Params::maxBestSepsForMinimizers);
        while( bestSeps.size() > SS ) bestSeps.pop_back();
        for(auto& sp : bestSeps) sp.updatePointers(*V);
    };


    double DENSITY_THRESHOLD = 3;
    bool isDenseGraph = (double) GraphUtils::countEdges(*V) / (double)V->size() > DENSITY_THRESHOLD;



    if( SEPARATOR_CREATORS_MODE == RAND ){
        if( rand()%3 ) SEPARATOR_CREATORS_MODE |= ART_POINTS_CREATOR;
        if( rand()%3 ) SEPARATOR_CREATORS_MODE |= BFS_CREATOR;
        if( rand()%3 ) SEPARATOR_CREATORS_MODE |= COMP_EXP_CREATOR;

        if( SEPARATOR_CREATORS_MODE == 0 ) SEPARATOR_CREATORS_MODE |= COMP_EXP_CREATOR;
    }



    if( SEPARATOR_CREATORS_MODE & ART_POINTS_CREATOR ){ // ARTICULATION POINTS
        if( recDepth == 0 ) cerr << "\tcreating art-points" << flush;
        ArtPointSeparatorCreator apCr(cnf);
        vector<Separator>  apSeps = apCr.createSeparators(*V, Pace20Params::maxSources);
        bestSeps.insert( bestSeps.end(), ALL(apSeps) );
        for( auto& sp : bestSeps ) sp.updatePointers(*V);

        assert( apSeps.size() == 1 );
    }



    if( SEPARATOR_CREATORS_MODE & BFS_CREATOR ){ // BFS
        if( recDepth == 0 ) cerr << "\tcreating bfs" << flush;
        BFSSeparatorCreator sepCr(*V,cnf);
        vector<Separator>  bfsSeps = sepCr.createSeparators(*V, Pace20Params::maxSources);
        bestSeps.insert( bestSeps.end(), ALL(bfsSeps) );
        for( auto& sp : bestSeps ) sp.updatePointers(*V);
    }

    if( SEPARATOR_CREATORS_MODE & FLOW_CREATOR){ // FLOW
        if( recDepth == 0 ) cerr << "\tcreating flow" << flush;
        GreedyNodeEdgeMinimizer minimizer(cnf, Pace20Params::minimizeNodesIteration ? GreedyNodeEdgeMinimizer::MINIMIZE_NODES : GreedyNodeEdgeMinimizer::MINIMIZE_EDGES );
        minimizer.sepEval = &sepEval;
        FlowSeparatorCreator sepFl(cnf,&minimizer);
        int repeats = 3*Pace20Params::maxSources;
        vector<Separator> flowSeps = sepFl.createSeparators( *V, repeats );
        bestSeps.insert( bestSeps.end(), ALL(flowSeps) );
        for( auto& sp : bestSeps ) sp.updatePointers(*V);
    }


    if( SEPARATOR_CREATORS_MODE & COMP_EXP_CREATOR ){ // ARTICULATION POINTS
        if( recDepth == 0 ) cerr << "\tcreating component expansion" << flush;
        ComponentExpansionSeparatorCreator ceCr( sepEval, cnf );

        double factor = 2;
        if( recDepth == 0 ) factor = 5;
        else if( recDepth == 1 ) factor = 4;
        else if( recDepth == 2 ) factor = 3;

        if( Pace20Params::quickAndWeakTreeCreation ) factor = 1.01 * ( (double)1 /  Pace20Params::maxSources );

        int D1 = 1, D2 = 3;

        if( recDepth <= D1 ){
            ceCr.setOrdersToCreate( ceCr.TIGHTEST_NODE_ORDER + ceCr.LEAST_NEIGHBORS_ORDER + ceCr.BFS_ORDER );
            ceCr.setOrdersToOptimize( ceCr.TIGHTEST_NODE_ORDER + ceCr.LEAST_NEIGHBORS_ORDER );
        }else if( recDepth <= D2 ){
            ceCr.setOrdersToCreate( ceCr.TIGHTEST_NODE_ORDER + ceCr.LEAST_NEIGHBORS_ORDER );
            ceCr.setOrdersToOptimize( ceCr.TIGHTEST_NODE_ORDER + ceCr.LEAST_NEIGHBORS_ORDER );
        }

        vector<Separator>  ceSeps = ceCr.createSeparators(*V, factor * Pace20Params::maxSources);
        bestSeps.insert( bestSeps.end(), ALL(ceSeps) );
        for( auto& sp : bestSeps ) sp.updatePointers(*V);

    }


    if( SEPARATOR_CREATORS_MODE & CLIQUE_CREATOR ){ // ARTICULATION POINTS
        if( recDepth == 0 ) cerr << "\tcreating clique separators" << flush;
        CliqueSeparatorCreator clqCr(*V,cnf);
        vector<Separator>  ceSeps = clqCr.createSeparators(*V, 3*Pace20Params::maxSources);
        bestSeps.insert( bestSeps.end(), ALL(ceSeps) );
        for( auto& sp : bestSeps ) sp.updatePointers(*V);
    }





    sortAndResizeSeparatorsForMinimization();

    // section for correction of found separators
    if( MINIMIZE_SEPARATORS ) {
        if (recDepth == 0) cerr << "\tminimizing best separators" << flush;
        int BSS = bestSeps.size();
        for (int i = 0; i < BSS; i++) {
            bestSep = bestSeps[i];

            TotalMinimizer totMin(&sepEval, cnf);
            bestSep = totMin.minimizeSeparator(bestSep);
            bestSeps.push_back(bestSep);
        }

        for (auto &sp : bestSeps) sp.updatePointers(*V);
    }

    sortAndResizeSeparatorsForMinimization();

    bestSep = bestSeps[0];

    double FC_THR = 1'200;
    bool hardGraphCondition = ( bestSep.stats.size > 100  && bestSep.stats.size < 1500  && GraphUtils::countEdges(*V) < 100'000);
    if(  (SEPARATOR_CREATORS_MODE & FLOWCUTTER_CREATOR)
         &&   (  (recDepth <= 7
                 && V->size() > 20
                 && GraphUtils::countEdges(*V) < 7 * V->size()
                 && bestSeps[0].stats.size > 1
                 && SeparatorEvaluators::estimateDepthBasedOnEdges(bestSep) + SeparatorEvaluators::estimateDepthBasedOnNodes( bestSep ) < FC_THR
              )
         || hardGraphCondition )
     ){


        if( recDepth == 0 ){ cerr << "\tflow cutter" << flush;  }

        FlowCutter fc(sepEval, cnf);

        int ITERATIONS = 8;

        if( hardGraphCondition ){
            ITERATIONS = 8;
        }

        auto seps = fc.createSeparators( *V, min( ITERATIONS, (int)V->size() ) );

        bestSeps.insert(bestSeps.end(), ALL(seps));
        for( auto& sp : bestSeps ) sp.updatePointers(*V);

        for( auto& sp : seps ){
            sp.updatePointers(*V);
            if( sepEval(sp, bestSep) ){
                bestSep = sp;
                bestSep.updatePointers(*V);
            }
        }

        sortAndResizeSeparatorsForMinimization();
    }



    if( PARTITION_SEPARATORS ){

        int SEPARATOR_PARTITION_THRESHOLD = 15;
        if (bestSep.stats.size >= SEPARATOR_PARTITION_THRESHOLD) {

            cerr << "bestSep before partitioning: " << bestSep << endl;

            VI nodes = bestSep.nodes;
            random_shuffle(ALL(nodes));
            double factor = 0.5;
            nodes.resize( nodes.size() * factor);
            bestSep = Separator(*V, nodes);
            bestSep.createSeparatorStats();

            cerr << "bestSep after partitioning: " << bestSep << endl;

        }
    }




    // Pace20Params::temp += bestSep.nodes.size();
    if( V->size() > 500 && recDepth <= 10 ){
        cerr << endl;
        for(int i=0; i<recDepth; i++) cerr << "  ";
        cerr << recDepth << ": ";
        DEBUG(bestSep);

        ENDL(1);
    }



    sortAndResizeSeparatorsForRecursion();

    auto getBestDTForSeparator = [=,&bestSeps]( Separator& bestSep ){
        if( bestSep.stats.size != bestSep.nodes.size() ){
            cerr << "in getBestDTForSeparator, stats.size() != nodes.size()" << endl;
            DEBUG(bestSep);
            exit(1);
        }

        VVI comps = ConnectedComponents::getConnectedComponents(*V,bestSep.nodes);

        sort( ALL(comps), [](auto & v1, auto& v2){ return v1.size() > v2.size(); } );

        int suma = accumulate( ALL(comps),0, [](int a, VI& b){ return a + b.size(); } );

        assert( V->size() == suma + bestSep.stats.size );

        vector<DepthTree> compTrees;
        for( VI & cmp : comps ){
            InducedGraph compGraph = GraphInducer::induce( *V, cmp );

            DepthTreeCreatorLarge dtCrL( compGraph.V, recDepth+1, cnf, randomSepEval );
            dtCrL.setSeparatorCreatorsMode( SEPARATOR_CREATORS_MODE );
            dtCrL.MINIMIZE_SEPARATORS = MINIMIZE_SEPARATORS;
            dtCrL.USE_KERNELIZATION = USE_KERNELIZATION;
            dtCrL.USE_DEG3_KERNELIZATION = USE_DEG3_KERNELIZATION;
            dtCrL.USE_DEG4_KERNELIZATION = USE_DEG4_KERNELIZATION;

            DepthTree cmpTree = dtCrL.getDepthTree();
            cmpTree.V = nullptr;

            cmpTree.root = compGraph.nodes.at(cmpTree.root);
            unordered_map<int,int> newPar;
            for( PII p : cmpTree.par ){
                newPar[ compGraph.nodes[ p.first ] ] = ( p.second == -1 ? -1 : compGraph.nodes[ p.second ] );
            }
            cmpTree.par = newPar;
            compTrees.push_back(cmpTree);
        }

        ComponentTreeMerger merger( *V, bestSep, comps, compTrees );
        DepthTree dt = merger.mergeComponents();

        if( bestSep.stats.size != bestSep.nodes.size() ){
            cerr << "in getBestDTForSeparator, stats.size() != nodes.size()" << endl;
            DEBUG(bestSep);
            exit(1);
        }
        return dt;
    };



    DepthTree dt = getBestDTForSeparator( bestSep );

    int sepsToCheck = Pace20Params::maxBestSepsForRecursion;
    sepsToCheck = (int)ceil( (double) Pace20Params::maxBestSepsForRecursion / (1 << min(30,recDepth)) );
    sepsToCheck = min( sepsToCheck, (int)bestSeps.size() );

    for( int i=1; i<sepsToCheck; i++ ){
//        if( V->size() > 150 ){ for( int j=0; j<recDepth; j++ ) cerr << "  "; cerr << bestSeps[i] << endl; }
        DepthTree dt2 = getBestDTForSeparator( bestSeps[i] );
        if( dt2.height < dt.height ) dt = dt2;
    }

    assert( dt.isCorrect() );

    dt.V = &*V;
    assert( dt.height == dt.calculateHeight() );
    if( cnf.sw.tle("main") ) return dt;




    {
        if( recDepth <= 3 ) DepthTreeIrrelevantNodeShifter::shiftIrrelevantNodes(dt);
        assert( dt.isCorrect() );
    }

    if( cnf.sw.tle("main") ) return dt;


    dt = DepthTreePivotMaker::makeAllPivots(dt);

    assert( dt.height == dt.calculateHeight() );
    assert( dt.root >= 0 && dt.root < V->size() );
    assert( dt.isCorrect() );

    if(  GraphUtils::countEdges(*V) == (int)V->size()-1 ){ // this should be equivalent to isTree, since V should be connected
        VI centroid = Centroids::getCentroidDecomposition(*V);
        DepthTree centrDt(*V);
        for( int i=0; i<centroid.size(); i++ ){
            centrDt.par[i] = centroid[i];
            if(centroid[i] == -1) centrDt.root = i;
        }
        centrDt.height = centrDt.calculateHeight();
        if( centrDt.height < dt.height ){
            dt = centrDt;
        }
    }

    return dt;

}

void DepthTreeCreatorLarge::testBridgesAndArtPoints() {
        vector<Separator> bestSeps;

        auto arts = BridgesAndArtPoints::getBridgesAndArtPoints(*V).first;
        VVI comps = ConnectedComponents::getConnectedComponents(*V, arts);
        sort( ALL(comps), [](auto &v1, auto &v2){ return v1.size() > v2.size(); } );
        VB inArt(V->size(), false); for(int p : arts) inArt[p] = true;
        for( int i=0; i < comps.size() && comps[i].size() > 100  ; i++ ){
            unordered_set<int> boundary;
            for(int p : comps[i]) for( int d : (*V)[p] ) if( inArt[d] ) boundary.insert(d);
            cerr << "component of size " << comps[i].size() << " has " << boundary.size() << " articulation points on its boundary, creating separator from those art-points" << endl;
            Separator sep(*V, VI(ALL(boundary)));
            sep.createSeparatorStats();
            DEBUG(sep);

            cerr << "minimizing with lcMin" << endl;
            LargestComponentsVCMinimizer lcMin(cnf);
            sep = lcMin.minimizeSeparator(sep);
            DEBUG(sep);

            cerr << "minimizing with lcGreedy" << endl;
            GreedyNodeEdgeMinimizer gneMin( cnf, GreedyNodeEdgeMinimizer::MINIMIZE_EDGES );
            gneMin.sepEval = &SeparatorEvaluators::sepEvalToUse;
            sep = gneMin.minimizeSeparator(sep);
            DEBUG(sep);
            if( sep.stats.size < 2000 ) bestSeps.push_back(sep);
            ENDL(2);
        }



}

void DepthTreeCreatorLarge::testVCorARTPOINTKernelization() {

    bool useVCKernelization = false;
    if( useVCKernelization && recDepth == 0 ){
        PartitionSVC vcCreator(*V);
        vcCreator.setSupressAllLogs(true); vcCreator.setMaxIterations( 200 ); vcCreator.setMaxRunTime(5'000);vcCreator.getSvcParams().alpha = 0.5;
        vcCreator.getSvcParams().iterationsPerSubgraph = 200;vcCreator.getSvcParams().setInitialSolutionForSubgraph = false;vcCreator.setTakeFirstBestSolution(false);
        vcCreator.getSvcParams().initialSolutionIterations = 1;vcCreator.run();
        VI vc = ((SolutionVC*) vcCreator.getBestSolution())->getVC();
        VI is = GraphUtils::getComplimentaryNodes( *V, vc );

        DEBUG(vc.size());
        DEBUG(is.size());
        assert( vc.size() + is.size() == V->size() );
        assert( VCUtils::isVertexCover( *V,vc ) );

        set<PII> inV;

        for( int i=0; i<V->size(); i++ ) for( int d : (*V)[i] ) if(d>i) inV.insert( {i,d} );
        InducedGraph g = GraphInducer::induce(*V, vc);

        GraphUtils::writeBasicGraphStatistics(g.V);

        int added = 0;
        for( int p : is ){
            VI v = (*V)[p];
            sort(ALL(v));
            for( int i=0; i<v.size(); i++ ){
                int a = v[i];
                for( int k=i+1; k<v.size(); k++ ){
                    int b = v[k];
                    if( inV.count( {a,b} ) ) continue;

                    inV.insert( {a,b} );
                    added++;
                    GraphUtils::addEdge( g.V, g.perm[a], g.perm[b]  );
                }
            }
        }

        DEBUG(added);

        GraphUtils::writeBasicGraphStatistics(g.V);

        DepthTreeCreatorLarge dtCr(g.V,recDepth+1, cnf);
        auto dt = dtCr.getDepthTree();

        cerr << "depth tree height of a VC-like kernelized graph: " << dt.height << endl;
        exit(1);
    }


    bool useArtPointKernelization = false;
    if( useArtPointKernelization && recDepth <= 1 ){
        ArtPointSeparatorCreator apCr(cnf);
        vector<Separator>  apSeps = apCr.createSeparators(*V, Pace20Params::maxSources);
        apSeps[0].updatePointers(*V);

        DEBUG(V->size());

        if( SeparatorEvaluators::isBalanced(apSeps[0], Pace20Params::balance) == false ){
            cerr << "art-point separator is not balanced!" << endl;
            VI arts = BridgesAndArtPoints::getBridgesAndArtPoints(*V).first;
            VVI comps = ConnectedComponents::getConnectedComponents(*V, arts);
            sort( ALL(comps), [](auto &v1, auto &v2){ return v1.size() > v2.size(); } );
            VB inArt(V->size(), false); for(int p : arts) inArt[p] = true;
            VI cmp = comps[0];
            cerr << "largestComponent has size: " << cmp.size() << endl;
            InducedGraph g = GraphInducer::induce( *V, cmp );
            DepthTreeCreatorLarge dtCr(g.V,recDepth+1, cnf);
            auto dt = dtCr.getDepthTree();

            cerr << "dt.height of art-point kernelized largest component: " << dt.height << endl;
        }
        else{
            cerr << "art point separator is balanced!" << endl;
            DEBUG( apSeps[0] );
        }
    }

}

Separator DepthTreeCreatorLarge::testMatchingEdgesContraction() {

    VPII edges = GraphUtils::getGraphEdges(*V);
    VB was(V->size());
    random_shuffle(ALL(edges));

    VPII matching;
    VI matched(V->size(),-1);
    for( PII e : edges ){
        if( !was[e.first] && !was[e.second] ){
            matching.push_back(e);
            was[e.first] = was[e.second] = true;
        }
    }

    VVI V2 = *V;

    set<int> valid;
    for(int i=0; i<V->size(); i++) valid.insert(i);
    for( PII e : matching ){
        GraphUtils::contractEdge( V2, e.first, e.second );

        valid.erase(e.first);

        matched[e.first] = e.second;
        matched[e.second] = e.first;
    }

    VI indSet( ALL(valid) );
    InducedGraph g = GraphInducer::induce( V2, indSet );

    int REPS = 1;
    auto sepEval = SeparatorEvaluators::sepEvalToUse;

    vector<Separator> bestSeps;

    for(int i=0; i < REPS; i++){

        FlowCutter fc(sepEval, cnf);
        auto seps = fc.createSeparators( g.V,5 );

        for(auto& sp : seps) sp.updatePointers(g.V);

        auto bestSep = *min_element( ALL(seps), sepEval );
        seps = {bestSep};

        TotalMinimizer totMin(&sepEval, cnf);
        for(auto& sp : seps) sp = totMin.minimizeSeparator( sp );
        for(auto& sp : seps) sp.updatePointers(g.V);

        bestSep = *min_element( ALL(seps), sepEval );

        for( auto& sp : seps ){

            VI nodes = sp.nodes;
            for(int& d : nodes) d = g.nodes[d];

            int T = nodes.size();
            for( int i=0; i<T; i++ ) if( matched[nodes[i]] != -1 ) nodes.push_back( matched[nodes[i]] );
            sort(ALL(nodes));
            nodes.resize( unique(ALL(nodes)) - nodes.begin() );
            sp = Separator(*V,nodes);
            sp.createSeparatorStats();

            sp = totMin.minimizeSeparator(sp);
            sp.updatePointers(*V);
        }

        bestSep = *min_element( ALL(seps), sepEval );
        bestSeps.push_back(bestSep);
    }

    for(auto& sp : bestSeps) sp.updatePointers(*V);

    return *min_element( ALL(bestSeps), sepEval );

}

void DepthTreeCreatorLarge::test() {

    Pace20Params::quickAndWeakTreeCreation = false;
    Config cnf{};

    for( int i=50; i<100; i++ ) {
        int N = i;

        int R = 100;
        while(R--){
            VVI V = GraphGenerator::getRandomTreePrufer(N);

            VI centroid = Centroids::getCentroidDecomposition(V);
            DepthTree centrDt(V);
            for( int i=0; i<centroid.size(); i++ ){
                centrDt.par[i] = centroid[i];
                if(centroid[i] == -1) centrDt.root = i;
            }
            centrDt.height = centrDt.calculateHeight();

            DepthTreeCreatorLarge dtcl(V,0, cnf);
            dtcl.USE_KERNELIZATION = false;
            DepthTree dt = dtcl.getDepthTree();

            if( centrDt.height < dt.height ){
                ENDL(3);
                DEBUG(V);
                DEBUG(*dt.V);
                DEBUG(dt);
                DEBUG(centrDt);
                exit(1);
            }
        }
    }
}






