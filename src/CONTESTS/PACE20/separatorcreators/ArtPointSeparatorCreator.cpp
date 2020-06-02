/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/separatorcreators/ArtPointSeparatorCreator.h>
#include <graphs/components/ConnectedComponents.h>
#include <CONTESTS/PACE20/separatorminimizers/GreedyNodeEdgeMinimizer.h>
#include <graphs/GraphReader.h>
#include <datastructures/Heap.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <graphs/generators/GraphGenerator.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>

#include "CONTESTS/PACE20/separatorcreators/ArtPointSeparatorCreator.h"
#include "graphs/components/BridgesAndArtPoints.h"
#include "graphs/GraphUtils.h"





Separator ArtPointSeparatorCreator::getFirstBalancedArtPointSeparator(VVI &V) {
    this->V = V;
//    DEBUG(V);
    const int INF = Constants::INF;

    createArtPointGraph();
    if( N <= 1 ){
//        cerr << "No articulation point in graph returning separator of first |V|/2 nodes from V" << endl;
        VI nodes;
        for( int i=0; i<(V.size()+1)/2; i++ ) nodes.push_back(i);
        Separator sep(V,nodes);
        sep.createSeparatorStats();
//        DEBUG(sep);
        return sep;
    }

//    DEBUG(arts.size());

    VB was(N,false);
    VB forbidden = was;
    preOrder = VI(N,-1);
    low = VI(N,INF);

//    VI forb = { 1,2,4 };
//    for(int d : forb) forbidden[d] = true;


    VI sepNodes;
    int originalGraphEdges = GraphUtils::countEdges(V);



    PII best = getMostBalancedArtInSubgraph(0, was, forbidden);
    int bestNode = best.first;
    int bestVal = best.second;



    //********** FOR TESTING ONLY

        VI testNodes(1,arts[bestNode]);
        Separator testSep(V,testNodes);
        testSep.createSeparatorStats();

//        DEBUG(testSep.stats); exit(1);

        if( testSep.stats.maxCompEdges != bestVal ){
            cerr << "FOUND ERROR!" << endl;
            DEBUG(V);
            DEBUG(apT);
            DEBUG(apW);
            DEBUG(arts);
            DEBUG(preOrder);
            DEBUG(low);
            DEBUG(nodeWeights);
            DEBUG(edgeWeights);
            DEBUG(nodeWeightsSumInSubgraph);
            DEBUG(edgeWeightsSumInSubgraph);
            DEBUG(maxNodesInSubgraph);
            DEBUG(maxEdgesInSubgraph);
            DEBUG(bestNode);
            DEBUG(bestVal);
            DEBUG(testSep.stats);


            exit(1);
        }

        assert( testSep.stats.maxCompEdges == bestVal );

    //**********

    auto countEdgesInComponent = [=,&forbidden]( int p ){
        VI pComp = ConnectedComponents::getConnectedComponentForNode(apT,p,forbidden);
        for(int t : pComp) forbidden[t] = false;
        int pCompEdges = ConnectedComponents::countEdgeWeightsInComponent(apT,apW,p,forbidden);
        for(int t : pComp){
            forbidden[t] = false;
            pCompEdges += edgeWeights[t];
        }
        return pCompEdges;
    };


//    DEBUG(bestNode);
//    DEBUG(bestVal);

//    cerr << "best node found: " << ( bestNode == -1 ? -1 : arts[bestNode] ) << " with val " << bestVal << endl;

    VI edgesInComponent(N,-1);
    edgesInComponent[bestNode] = countEdgesInComponent(bestNode);


    auto comp = [=, &edgesInComponent](int a,int b){ return edgesInComponent[a] > edgesInComponent[b]; };
    Heap<int> heap(N,bestNode,comp);
    for( int i=0; i<N; i++ ){
        if( i != bestNode ){
            heap.removeFromHeap(i);
        }
    }


    VI bestValInComponent(N,Constants::INF);
    bestValInComponent[bestNode] = bestVal;
//    ENDL(2);
//    cerr << "starting!" << endl;
    while( !heap.empty() ){
//        DEBUG(heap);

        int p = heap.extract_min();

//        DEBUG(p);

        int pCompEdges = edgesInComponent[p];
//        double currentBalance = (double)edgesInComponent[p] / originalGraphEdges;
        double currentBalance = (double)bestValInComponent[p] / originalGraphEdges;
//        DEBUG(currentBalance);

//        if( pCompEdges <= Pace20Params::balance * originalGraphEdges ) continue;

//        cerr << "ADDING NODE TO SEP: " << p << " corresponding to original node: " << arts[p] << " with balance: "
//             << currentBalance << " = " << bestValInComponent[p] << " / " << originalGraphEdges << endl;

        edgesInComponent[p] = -1;
        sepNodes.push_back( arts[p] );


        bestNode = p;



        Separator sep(V,sepNodes);
        sep.createSeparatorStats();
        if( sep.stats.maxCompEdges <= Pace20Params::balance * originalGraphEdges ) break;



//        cerr << "best node found: " << ( bestNode == -1 ? -1 : arts[bestNode] ) << " with val " << bestVal << endl;
        if( bestNode == -1 ){
            cerr << "bestNode = -1, that means there is no balanced art-point separator" << endl;
            exit(1);
        }



        forbidden[bestNode] = true;
        VVI comps;
        for( int d : apT[bestNode] ){
            if( !forbidden[d] ){
                VI cmp = ConnectedComponents::getConnectedComponentForNode( apT,d,forbidden );
                comps.push_back(cmp);
            }
        }

        for(VI& v : comps) for(int d : v) forbidden[d] = false;

//        DEBUG(comps);
//        for( int i=0; i<comps.size(); i++ ){
//            DEBUG( comps[i].size() );
//            DEBUG( countEdgesInComponent( comps[i][0] ) );
//        }




        for( VI& v : comps ){
            int representative = -1;
            for( int d : v ) if( isArtPoint[d] ){
                representative = d;
                break;
            }
            if( representative == -1 ) continue;

            best = getMostBalancedArtInSubgraph(representative, was, forbidden);
            bestNode = best.first;
            bestVal = best.second;

            edgesInComponent[bestNode] = countEdgesInComponent(bestNode);
            if( edgesInComponent[bestNode] <= Pace20Params::balance * originalGraphEdges ) continue;

            double minLocalChange = 0.995;
//            double minLocalChange = 0.9999;
//            double minLocalChange = Pace20Params::balance;
            double localBalance = (double) bestVal / edgesInComponent[bestNode];
            // if the best vertex does not reduce maximum edges in component by more than minLocalChange, then i terminate (there may be gazillion 'small' art points
            // that do not make any general change to the graph
            if( localBalance > minLocalChange ){
//                DEBUG( arts[representative] );
//                cerr << "local balance = " << localBalance << " = " << bestVal << " / " << edgesInComponent[bestNode] << endl;
                continue;
            }

            bestValInComponent[bestNode] = bestVal;
            heap.set(bestNode,bestNode);
//            cerr << "inserting to heap component " << v << " with " << edgesInComponent[bestNode]
//                 << " edges and balance " << ( (double)edgesInComponent[bestNode] / originalGraphEdges ) << endl;
        }

//        ENDL(2);

    }

    Separator sep(V,sepNodes);
    sep.updatePointers(V);
    sep.createSeparatorStats();
//    DEBUG(sep.stats);

//    exit(1);
    return sep;

}

void ArtPointSeparatorCreator::createArtPointGraph() {


    arts = BridgesAndArtPoints::getBridgesAndArtPoints(V).first;
    Separator sep(V,arts);
    sep.createSeparatorStats();


    VVPII sepGraph;
    SeparatorMinimizer::createSeparatorGraphDataForSeparator( sep, sepGraph, nodeWeights, edgeWeights );
    N = sepGraph.size();
    n = arts.size();


    apT = VVI(N);
    apW = VVI(N);
    isArtPoint = VB(N,false);
    for(int i=0; i<n; i++) isArtPoint[i] = true;


    for( int i=0; i<N; i++ ){
        for( PII p : sepGraph[i] ){
            apT[i].push_back(p.first);
            apW[i].push_back(p.second);
        }
    }

//    DEBUG(arts);
//    DEBUG(apT);
//    DEBUG(apW);
//    DEBUG(nodeWeights);
//    DEBUG(edgeWeights);

    nodeWeightsSum = edgeWeightsSum = nodeWeightsSumInSubgraph = edgeWeightsSumInSubgraph = maxNodesInSubgraph = maxEdgesInSubgraph = VI(N,0);
}

vector<Separator> ArtPointSeparatorCreator::createSeparators(VVI &V, int repeats) {
    Separator firstBalanced = getFirstBalancedArtPointSeparator(V);
    return {firstBalanced};
}









PII ArtPointSeparatorCreator::getMostBalancedArtInSubgraph(int num, VB &was, VB &forbidden) {
    bool debug = false;

    VI compNodes = ConnectedComponents::getConnectedComponentForNode(apT,num,forbidden);

    if(debug)DEBUG(compNodes);

    for( int t : compNodes ) forbidden[t] = false;

    int totalNodes = 0;
    int totalEdges = 0;
    function< void(int) > dfsGetTotals = [=, &totalNodes, &totalEdges, &was, &dfsGetTotals]( int num ){
//        DEBUG(num);
        was[num] = true;
        totalNodes += nodeWeights[num];
        totalEdges += 2*edgeWeights[num];

        for( int i=0; i < apT[num].size(); i++ ){
            int d = apT[num][i];
            if( !forbidden[d] ){
                totalEdges += apW[num][i];
                if( !was[d] ) dfsGetTotals(d);
            }
        }
    };


    dfsGetTotals(num);

    totalEdges >>= 1;

    if(debug){
        DEBUG(totalNodes);
        DEBUG(totalEdges);
    }

    for( int t : compNodes ){
        was[t] = false;
        preOrder[t] = -1;
        low[t] = Constants::INF;
        maxNodesInSubgraph[t] = maxEdgesInSubgraph[t] = 0;
        nodeWeightsSum[t] = edgeWeightsSum[t] = nodeWeightsSumInSubgraph[t] = edgeWeightsSumInSubgraph[t] = 0;
    }


    unordered_set<int> wasWithGivenLow;
    function< void(int, const int) > getNodesWithGivenLow =
            [=,&getNodesWithGivenLow,&was,&forbidden, &wasWithGivenLow](int num,  const int givenLow){

        if( wasWithGivenLow.count(num) ) return;
        if( preOrder[num] <= givenLow ) return;

                if(debug) cerr << "entering wasWithGivenLow(), num = " << num << "   givenLow = " << givenLow << endl;

        wasWithGivenLow.insert(num);

        for( int d : apT[num] ){
            if(forbidden[d]) continue;
            if( low[d] == givenLow ) getNodesWithGivenLow(d,givenLow);
//            if( low[d] >= givenLow ) getNodesWithGivenLow(d,givenLow); // ONLY FOR TESTING!

        }

    };


    function< void(int,int,int, VB&, VI&) > propagate = [=, &propagate, &was, &forbidden](int num, int par, int lowVal, VB& visited, VI& path){
        if( debug ) cerr << "Propagating from " << par << " to " << num << endl;
        visited[num] = true;
        path.push_back(num);
        for( int d : apT[num] ){
            if( forbidden[d] || d == par || visited[d] ) continue;
            if( low[d] < preOrder[num] ) propagate( d,num,lowVal, visited, path );
        }
        low[num] = lowVal;
//        visited[num] = false;
    };

    VB visitedPropagate(N,false);
    visitedPropagate[num] = true;

    int cnt = 0;
    function< void(int,int) > dfsLow = [=, &dfsLow, &was, &forbidden, &cnt, &wasWithGivenLow, &propagate, &visitedPropagate](int num, int par){
//        if(debug) DEBUG(num);

        was[num] = true;
        if( preOrder[num] == -1 ){
            preOrder[num] = cnt;
//            cerr << "setting preOrder[" << num << "] to " << cnt << endl;
        }
        if( low[num] == Constants::INF ) low[num] = preOrder[num];
        cnt++;

        for( int d : apT[num] ){
            if( forbidden[d] ) continue;

            if( d != par ){
                if( !was[d] ){
                    dfsLow(d,num);
//                    low[num] = min( low[num], low[d] );

                    if( low[d] < low[num] ){
                        low[num] = low[d];
                    }else if( low[d] == preOrder[num] ){
                        // TESTING propagation

                        VI path(1,num);
                        visitedPropagate[num] = true;
                        propagate(d,num, low[d], visitedPropagate, path);
                        if(debug)DEBUG(path);
                        for(int xx : path) visitedPropagate[xx] = false;
                    }

                }else{
                    low[num] = min( low[num], preOrder[d] );
                }
            }
        }





        // TESTING ONLY!!
//        for( int d : apT[num] ){
//            if( forbidden[d] ) continue;
//            if( d != par ){
//                if( low[d] < preOrder[d] ) low[d] = min( low[d], low[num] );
//            }
//        }
    };

    dfsLow(num,num);

    if(debug){
        DEBUG(preOrder);
        DEBUG(low);
    }


    for( int t : compNodes ){
        was[t] = false;
    }

    function< void(int,int) > dfsFindSizes = [=, &dfsFindSizes, &was, &forbidden, &cnt, &wasWithGivenLow](int num, int par){
//        DEBUG(num);
        was[num] = true;

        for( int d : apT[num] ){
            if( forbidden[d] ) continue;
            if( d != par && !was[d] ) dfsFindSizes(d,num);
        }


        nodeWeightsSumInSubgraph[num] = nodeWeights[num];
        edgeWeightsSumInSubgraph[num] = edgeWeights[num];

        for( int i=0; i<apT[num].size(); i++ ){
            int d = apT[num][i];
            if( forbidden[d] ) continue;

            if( d != par  ){

                if(preOrder[d] == low[d] && preOrder[d] > preOrder[num] ){
                    nodeWeightsSumInSubgraph[num] += nodeWeightsSumInSubgraph[d];

                    edgeWeightsSumInSubgraph[num] += edgeWeightsSumInSubgraph[d];
                    edgeWeightsSumInSubgraph[num] += apW[num][i];

                    maxNodesInSubgraph[num] = max( maxNodesInSubgraph[num], nodeWeightsSumInSubgraph[d] );
                    maxEdgesInSubgraph[num] = max( maxEdgesInSubgraph[num], edgeWeightsSumInSubgraph[d] );
                }

                if( low[d] == preOrder[num] ) edgeWeightsSumInSubgraph[num] += apW[num][i];
            }
        }

        if(debug){
            ENDL(1);
            DEBUG(num);
            DEBUG(par);
            DEBUG(wasWithGivenLow);
            DEBUG(nodeWeightsSumInSubgraph[num]);
            DEBUG(edgeWeightsSumInSubgraph[num]);
            DEBUG(maxNodesInSubgraph[num]);
            DEBUG(maxEdgesInSubgraph[num]);
        }

        unordered_set<int> wasInCluster;

        for( int dd : apT[num] ) {
            if( forbidden[dd] ) continue;
            if( (dd == par) || wasInCluster.count(dd) || low[dd] != preOrder[num] ) continue;

            wasWithGivenLow.clear();
            wasWithGivenLow.insert(num);

            if(debug) DEBUG(dd);
//            getNodesWithGivenLow(num, preOrder[num]);
            getNodesWithGivenLow(dd, preOrder[num]);

            if(debug){
                DEBUG(num);
                DEBUG(par);
                DEBUG(wasWithGivenLow);
            }
            wasWithGivenLow.erase(num);

            if(debug){
                DEBUG(wasWithGivenLow);
                DEBUG(nodeWeightsSumInSubgraph[num]);
                DEBUG(edgeWeightsSumInSubgraph[num]);
                DEBUG(maxNodesInSubgraph[num]);
                DEBUG(maxEdgesInSubgraph[num]);
            }

            wasInCluster.insert( ALL(wasWithGivenLow) );

            int tempSumNodes = 0, tempSumEdges = 0;
            int edgesBetween = 0;
            for (int d : wasWithGivenLow) {
                nodeWeightsSumInSubgraph[num] += nodeWeightsSumInSubgraph[d];
                edgeWeightsSumInSubgraph[num] += edgeWeightsSumInSubgraph[d];

                tempSumNodes += nodeWeightsSumInSubgraph[d];
                tempSumEdges += edgeWeightsSumInSubgraph[d];

                for (int i = 0; i < apT[d].size(); i++) {
                    int p = apT[d][i];
                    if( forbidden[p] ) continue;

                    if (wasWithGivenLow.count((p))) edgesBetween += apW[d][i];
                }
            }

            edgesBetween >>= 1;

            if(debug){
                DEBUG(tempSumNodes);
                DEBUG(tempSumEdges);
                DEBUG(edgesBetween);
            }

            edgeWeightsSumInSubgraph[num] += edgesBetween;
            maxNodesInSubgraph[num] = max( maxNodesInSubgraph[num], tempSumNodes );
            maxEdgesInSubgraph[num] = max( maxEdgesInSubgraph[num], tempSumEdges + edgesBetween);

            if(debug){
                DEBUG(nodeWeightsSumInSubgraph[num]);
                DEBUG(edgeWeightsSumInSubgraph[num]);
                DEBUG(maxNodesInSubgraph[num]);
                DEBUG(maxEdgesInSubgraph[num]);
                ENDL(1);
            }


            wasWithGivenLow.clear();
        }
    };

    dfsFindSizes(num,num);

    if(debug){
        ENDL(5);
        DEBUG(preOrder);
        cerr << "     "; DEBUG(low);

        DEBUG(nodeWeightsSumInSubgraph);
        DEBUG(edgeWeightsSumInSubgraph);
        DEBUG(maxNodesInSubgraph);
        DEBUG(maxEdgesInSubgraph);
    }



    int M = Constants::INF;
    int bestNode = -1;

    for( int t : compNodes ){
        if( isArtPoint[t] == false ) continue;

        if(debug) DEBUG(t);

        int m = maxEdgesInSubgraph[t];

        int excessEdges = 0;
        for( int i=0; i<apT[t].size(); i++ ){
            int d = apT[t][i];
            if( forbidden[d] ) continue;
            if(debug) cerr << "\td = " << d << endl;

//            if( low[d] >= preOrder[t] ) m = max( m, maxEdgesInSubgraph[d] );
//            else excessEdges += apW[t][i];

            if( low[d] < preOrder[t] ) excessEdges += apW[t][i];
        }

        m = max( m, totalEdges - excessEdges - edgeWeightsSumInSubgraph[t] );

        if(debug){
            DEBUG(excessEdges);
            DEBUG(m);
        }

        if( m < M ){
            M = m;
            bestNode = t;
        }

        if(debug) ENDL(1);
    }

    if(debug){
        DEBUG(bestNode);
        DEBUG(M);
    }

//    exit(7);



    // clearing in the end
    for( int t : compNodes ){
        was[t] = false;
//        preOrder[t] = -1;
//        low[t] = Constants::INF;
//        nodeWeightsSum[t] = edgeWeightsSum[t] = nodeWeightsSumInSubgraph[t] = edgeWeightsSumInSubgraph[t] = 0;
    }


    return {bestNode,M};
}

int ArtPointSeparatorCreator::getMostBalancedArtPoint(int num, int par, VB &was) {
    return 0;
}

void ArtPointSeparatorCreator::test(){
//    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
//    DEBUG(V.size());
//    DEBUG( GraphUtils::countEdges(V) );


/*    VVI V =  {{1}, {0, 8, 2}, {1, 9, 5, 7, 8}, {7, 5}, {7}, {3, 8, 2}, {8}, {4, 3, 2}, {6, 5, 1, 2}, {2}};*/
//    VVI V =  {{1, 3, 2}, {6, 8, 0}, {4, 0}, {7, 0, 9, 8}, {2, 8}, {9}, {1}, {3, 9}, {4, 1, 3}, {5, 3, 7}};
//    ArtPointSeparatorCreator apSepCr;
//    vector<Separator> res = apSepCr.createSeparators(V,1);
//    exit(1);


    int REPS = 10;
    for( int i=0; i<REPS; i++ ){
        cerr << "\rtesting: " << (i+1) << " / " << REPS << flush;

        int N = 10'000;
        VVI V = GraphGenerator::getRandomTreePrufer(N);

        set<PII> zb;
        for( int j=0; j<N; j++ ){
            for(int d : V[j]){
                if( d > j ) zb.insert({j,d});
            }
        }

        int randomEdges = 10;
        set<PII> edges;
        while(edges.size() < randomEdges){
            int a = rand()%N;
            int b = rand()%N;
            while(a==b) b = rand()%N;
            if( a > b ) swap(a,b);
            if( zb.count({a,b}) ) continue;

            edges.insert( {a,b} );
        }

        for(PII e : edges){
            V[e.first].push_back( e.second );
            V[e.second].push_back(e.first);
        }



        ArtPointSeparatorCreator apSepCr;
        vector<Separator> res = apSepCr.createSeparators(V,1);
        for( auto& s : res ) s.updatePointers(V);
        Separator sep = res[0];
        sep.createSeparatorStats();

        int bestArtPoint = sep.nodes[0];

        auto checkBestArtNaive = [&V, &bestArtPoint](){
            VI arts = BridgesAndArtPoints::getBridgesAndArtPoints(V).first;

//            DEBUG(arts.size());

//            SeparatorEvaluator sepEval = SeparatorEvaluators::estimatedDepthTreeEdge;
            SeparatorEvaluator sepEval = SeparatorEvaluators::sepEvalToUse;

            Separator bestSep( V, {bestArtPoint} );
            bestSep.createSeparatorStats();
//            DEBUG(bestSep);

            int cnt = 0;
            for( int a : arts ){
                cerr << "\rchecking naively each art-point: #" << ++cnt << " / " << arts.size() << flush;

                Separator temp(V,{a});
                temp.createSeparatorStats();

                if( sepEval(temp, bestSep) ){
                    cerr << a << " is better art point that " << bestArtPoint << endl;
                    DEBUG( temp );
                    DEBUG(bestSep);
                    exit(1);
                }

            }
        };

        checkBestArtNaive();
        ENDL(1);
    }

    cerr << "PASSED!" << endl;



    exit(1);


}
