/*
 * Copyright (c) 2020, Sylwester Swat
*/


#include <CONTESTS/PACE20/separatorminimizers/GreedyNodeEdgeMinimizer.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include "datastructures/Heap.h"
#include "graphs/GraphReader.h"
#include "utils/StandardUtils.h"


Separator GreedyNodeEdgeMinimizer::minimizeSeparator(Separator sep, VVPII &sepGraph,  VI nodeW, VI edgeW) {
//    cerr << "entering minimizer" << endl;


    bool debug = false;

    nodeWeights = nodeW;
    edgeWeights = edgeW;


    if(debug) cerr << "Separator before minimization:" << endl << sep << endl;

    this->sepGraph = sepGraph;
    this->sep = &sep;
    sep.updatePointers(*sep.V);


    convertSepGraphToGr();
    n = sep.nodes.size();
    N = sepGraph.size();

    VI degInComps(n,0);
    for( int i=0; i<n; i++ ) for(int d : gr[i]) if( d >= n ) degInComps[i]++;

    if(debug) DEBUG(degInComps);

    VI neighCompSizesSum(n,0);
    VI edgeCompSizesSum(n,0);

    VI edgeWeightsSum(n,0); // sum of edges from node i to all comp-neighbors
    for( int i=0; i<n; i++ ){
        for(int j=0; j<gr[i].size(); j++) if( gr[i][j] >= n ) edgeWeightsSum[i] += grW[i][j];
    }

    if(debug) DEBUG(edgeWeightsSum);

    bool useGlobalTotalWeight = true;
    int totalWeight = 0;

    if( useGlobalTotalWeight ){
        if( minimizationType == MINIMIZE_NODES) totalWeight = sep.stats.originalGraphSize;
        else totalWeight = sep.stats.originalGraphEdges;
    }
//    int totalWeight = sep.nodes.size(); // in this case we consider balnce together with nodes in separator


    for( int i=0; i<n; i++ ){
        nodeWeights[i] = 1;
        for( int d : gr[i] ){
            if( d >= n ){
                neighCompSizesSum[i] += nodeWeights[d];
                edgeCompSizesSum[i] += edgeWeights[d];
            }
        }
    }
    if( !useGlobalTotalWeight ) {
        for (int i = n; i < N; i++) {
            if (minimizationType == MINIMIZE_NODES) totalWeight += nodeWeights[i];
            else totalWeight += edgeWeights[i];
        }
    }

    if(debug){
        DEBUG(totalWeight);
        DEBUG(nodeWeights);
        DEBUG(neighCompSizesSum);
        DEBUG(edgeCompSizesSum);
    }


    auto isBalanced = [=, &totalWeight, &neighCompSizesSum, &edgeCompSizesSum](int a){
        double balance = Pace20Params::balance;
        if( minimizationType == MINIMIZE_NODES ) return neighCompSizesSum[a] <= balance * totalWeight;
//        else return edgeCompSizesSum[a] <= balance * totalWeight;
        else return edgeCompSizesSum[a] + edgeWeightsSum[a] <= balance * totalWeight;
    };

    auto comp = [=, &neighCompSizesSum, &edgeCompSizesSum, &degInComps, &edgeWeightsSum]( int a, int b ) {
//        if( degInComps[a] == 1 && degInComps[b] != 1 ) return true;
//        if( degInComps[a] != 1 && degInComps[b] == 1 ) return false;


        /**
         * If there is a node that has no neighbors in any component, then i try to isolate it from separator and make it a new component.
         */
        if (degInComps[a] * degInComps[b] == 0 && degInComps[a] != degInComps[b]) return degInComps[a] < degInComps[b];
        else if (degInComps[a] * degInComps[b] == 0)
            return gr[a].size() <
                   gr[b].size(); // if both a and b have no neighbors in component, then i isolate the one with less neighbors in separator


//        if( degInComps[a] <= 1 && degInComps[b] <= 1 && degInComps[a] != degInComps[b] )  return degInComps[a] < degInComps[b];

////        if( degInComps[a] != degInComps[b] ) return degInComps[a] < degInComps[b];
//        if( degInComps[a] <= 1 && degInComps[b] != 1 ) return degInComps[a] < degInComps[b];
//        if( degInComps[a] != 1 && degInComps[b] <= 1 ) return degInComps[a] < degInComps[b];

        if (minimizationType == MINIMIZE_NODES && neighCompSizesSum[a] != neighCompSizesSum[b])
            return neighCompSizesSum[a] < neighCompSizesSum[b];

//        else if( minimizationType == MINIMIZE_EDGES && edgeCompSizesSum[a] != edgeCompSizesSum[b] ){
//            return edgeCompSizesSum[a] < edgeCompSizesSum[b];
//        }
        else if( minimizationType == MINIMIZE_EDGES && edgeCompSizesSum[a] + edgeWeightsSum[a] != edgeCompSizesSum[b] + edgeWeightsSum[b] ){
            // add sum of weights of all edges into components
            return edgeCompSizesSum[a] + edgeWeightsSum[a] < edgeCompSizesSum[b] + edgeWeightsSum[b];
        }

        else if( (int)gr[a].size() - degInComps[a] != (int)gr[b].size() - degInComps[b] ) return (int)gr[a].size() - degInComps[a] < (int)gr[b].size() - degInComps[b]; // selecting that node that has less neighbors in separator
        else return a < b;
    };


    Separator bestSep = sep; bestSep.stats.sep = &bestSep;
//    VI nodeRemovalOrder; // order of nodes that were removed from heap during minimization
//    int bestIndex=0; // bestIndex is number of beginning elements of nodeRemovalOrder that should be removed from bestSep.nodes to obtain best possible separator
    VB bestSepNodes(sep.nodes.size(),true);



    VB neighVec(N,false);
    VB nodesSet(N,false);
    VI totalEdges(N,0);

    auto mergeNodes = [=,&edgeWeightsSum, &neighCompSizesSum, &edgeCompSizesSum, &totalWeight, &degInComps, &neighVec, &totalEdges, &sep]( VI &nodes, VI & neigh, VB & nodesSet ){

        if(debug){
            DEBUG(nodes);
            DEBUG(neigh);
            DEBUG(nodesSet);
        }

        int wN = 0;
        int wE = 0;
        int mergeTo = N;
        for( int d : nodes ){
            if( d >= n ){
                wN += nodeWeights[d];
                wE += edgeWeights[d];
                mergeTo = min( mergeTo,d );
//                sep.stats.numberOfComponents--;
            }else{
                if( !useGlobalTotalWeight ) if( minimizationType == MINIMIZE_NODES ) totalWeight++;
                wN++;
                neighCompSizesSum[d] = 0;
                edgeCompSizesSum[d] = 0;
                nodeWeights[d] = 0;
                edgeWeights[d] = 0;

                edgeWeightsSum[d] = 0;

                degInComps[d] = 0;
                for( int j=0; j<gr[d].size(); j++ ){
                    int dd = gr[d][j];
                    if( dd >= n && nodesSet[dd] ){
                        wE += grW[d][j];
                        if( !useGlobalTotalWeight ) if( minimizationType == MINIMIZE_EDGES ) totalWeight += grW[d][j];
                    }
                }

            }
        }

        if(debug){
            ENDL(1);
            DEBUG(wN);
            DEBUG(wE);
            ENDL(1);
        }

        // perhaps add all nodes with neighborhood in sep to gr at the very beginning, when creating gr. Then we will not have to consider that case

        if( mergeTo == N ){

            if(debug){
                DEBUG(gr);
                cerr << "Nodes " << nodes << " have neighbors only in separator, creating new component" << endl;
            }

            gr.push_back(VI());
            grW.push_back(VI());
            nodeWeights.push_back(wN);
            edgeWeights.push_back(wE);

            nodesSet.push_back(false);
            neighVec.push_back(false);
            totalEdges.push_back(0);

//            sep.stats.numberOfComponents++;

            N++;
        }


        for(int p : nodes){
            for(int j=0; j<gr[p].size(); j++){
                int d = gr[p][j];
                if( nodesSet[d] ) continue;

                neighCompSizesSum[d] -= nodeWeights[p];
                edgeCompSizesSum[d] -= edgeWeights[p];


                if( p >= n ) degInComps[d]--;
            }
            nodeWeights[p] = 0;
            edgeWeights[p] = 0;
        }

        if(debug){
            DEBUG(neighCompSizesSum);
            DEBUG(wN);
            DEBUG(mergeTo);
        }

        for( int p : nodes ){
            for( int j=0; j<gr[p].size(); j++ ){
                int d = gr[p][j];
                if( nodesSet[d] ) totalEdges[d]++;
                else totalEdges[d] += grW[p][j];
            }
        }

        auto removeNodesFromGraph = [=, &neigh, &nodesSet, &edgeWeightsSum](){
            for( int d : neigh ){
                if( nodesSet[d]  ) continue;
                for( int j = (int)gr[d].size()-1; j>=0; j-- ){
                    int dd = gr[d][j];
                    if( nodesSet[dd] ){
                        swap( gr[d][j], gr[d].back() );
                        gr[d].pop_back();

//                        edgeWeightsSum[d] -= grW[d][j];

                        swap( grW[d][j], grW[d].back() );
                        grW[d].pop_back();
                    }
                }
            }
            for(int d : nodes){
                gr[d].clear();
                grW[d].clear();
            }
        };

        removeNodesFromGraph();

        nodeWeights[mergeTo] = wN;
        edgeWeights[mergeTo] = wE;

        for( int p : neigh ){
            if( nodesSet[p] ) continue;
            gr[p].push_back(mergeTo);
            gr[mergeTo].push_back(p);

            grW[p].push_back( totalEdges[p] );
            grW[mergeTo].push_back( totalEdges[p] ); // yes, here is again the same totalEdges[p] as above since the values are the same (symmetrical), we keep only one of them

//            edgeWeightsSum[p] += totalEdges[p];

            degInComps[p]++;
            neighCompSizesSum[p] += wN;
            edgeCompSizesSum[p] += wE;

        }


        for(int p : neigh) totalEdges[p] = 0; // clearing totalEdges

    };


    Heap<int> heap( comp ); // version with heap
    for(int i=0; i<n; i++){

        heap.push_back(i); // version with heap
//        if( !( isBalanced(i) || degInComps[i] == 1 ) ){ // version with heap
        if( !( isBalanced(i) || degInComps[i] <= 1 ) ){ // version with heap
            heap.removeFromHeap(i);
        }

    }


    int nodesRemoved = 0;

    if(debug){
        cerr << "Before any removal" << endl;
        DEBUG(gr);
        DEBUG(grW);
        DEBUG(nodeWeights);
        DEBUG(edgeWeights);
        DEBUG(degInComps);
        DEBUG(neighCompSizesSum);
        DEBUG(edgeCompSizesSum);
        DEBUG(edgeWeightsSum);
        DEBUG(totalWeight);
        ENDL(1);
    }

    VI neigh;
    neigh.reserve( 2*N );
    int nodesInsertedToZb = heap.size();


    if(debug) DEBUG(heap);

    while( !heap.empty() && nodesRemoved < sep.nodes.size() -1  ){
        nodesRemoved++;

        int p = heap.extract_min(); // version with heap

        if(debug){
            cerr << "removing node " << p << "(" << sep.nodes[p] << ") from separator" << endl;
        }

        VI nodes;
        nodes.reserve( gr[p].size() );
        nodes.push_back(p);
        for(int d : gr[p]){
            if( d >= n ) nodes.push_back(d);
            else edgeWeightsSum[d]++;
        }


        neigh.clear();
        for( int p : nodes ){
            for( int d : gr[p] ){
                if( !neighVec[d] ) neigh.push_back(d);
                neighVec[d] = true;
            }
        }


        for( int d : nodes ) nodesSet[d] = true;

        VI removedFromZb;
        for( int p : neigh ){
            if( nodesSet[p] ) continue;

            removedFromZb.push_back(p);
        }

        mergeNodes(nodes,neigh,nodesSet);

        if(debug){
            DEBUG(neighCompSizesSum);
            DEBUG(edgeCompSizesSum);
            DEBUG(neigh);
        }

        for(int p : removedFromZb){
            if( isBalanced(p) || degInComps[p] == 1 ){
                if(debug) cerr << "inserting to zb balanced node " << p << " with neighCompSizesSum = " << neighCompSizesSum[p] << " / totalWeight = " << totalWeight << endl;
                heap.set(p,p); // version with heap
                nodesInsertedToZb++;
            }else{
                heap.removeFromHeap(p); // version with heap
            }
        }


        if(debug){
            cerr << "After removing " << p << endl;
            DEBUG(gr);
            DEBUG(grW);
            DEBUG(nodeWeights);
            DEBUG(edgeWeights);
            DEBUG(degInComps);
            DEBUG(neighCompSizesSum);
            DEBUG(edgeCompSizesSum);
            DEBUG(edgeWeightsSum);
            DEBUG(totalWeight);
            ENDL(1);
        }

        for( int d : nodes ) nodesSet[d] = false;
        for( int p : neigh ) neighVec[p] = false;




        // TESTING ALL STATES CHECK!
        if( sepEval != nullptr ){

            sep.stats.maxCompSize = sep.stats.maxCompEdges = sep.stats.size = 0;
            unordered_set<int> comps;
            for(int i=0; i<n; i++){
                if( gr[i].empty() ) continue;
                sep.stats.size++;

                for(int d : gr[i]){
                    if( d >= n ){
                        sep.stats.maxCompSize = max( sep.stats.maxCompSize, nodeWeights[d] );
                        sep.stats.maxCompEdges = max( sep.stats.maxCompEdges, edgeWeights[d] );

                        comps.insert(d);
                    }
                }
            }

            sep.stats.numberOfComponents = comps.size();

            /*ENDL(5);
            cerr << "I am here at all" << endl;

            {
                cerr << "in GNE slow checking of correctness" << endl;
                VI tempNodes;
                for(int i=0; i<n; i++) if( !gr[i].empty() ) tempNodes.push_back( sep.nodes[i] );
                Separator temp(*sep.V,tempNodes);
                temp.createSeparatorStats();
//                DEBUG(sep.stats);
//                DEBUG(temp.stats);
                assert( temp.stats.maxCompSize == sep.stats.maxCompSize );
                assert( temp.stats.maxCompEdges == sep.stats.maxCompEdges );
            }*/

            if( debug ){
                DEBUG(sep);
                DEBUG(bestSep);
            }

            if( (*sepEval)( sep,bestSep ) ){
//                cerr << "\tsep is better than bestSep!" << endl << "nodes in sep: ";
//                for(int i=0; i<n; i++) if( !gr[i].empty() ) cerr << sep.nodes[i] << " "; cerr << endl;

                fill(ALL(bestSepNodes),false);
                for(int i=0; i<n; i++) if( !gr[i].empty() ) bestSepNodes[i] = true;

                bestSep.stats = sep.stats;
                bestSep.stats.sep = &bestSep;
            }
//            else{
//                cerr << "\tsep is WORSE than bestSep" << endl << "nodes in sep: ";
//                for(int i=0; i<n; i++) if( !gr[i].empty() ) cerr << sep.nodes[i] << " "; cerr << endl;
//                ENDL(2);
//            }
        }
        // END OF TESTING ALL STATES CHECK


    }




    VI oldNodes = sep.nodes;
    VI newNodes;
    for(int i=0; i<n; i++) if( gr[i].size() > 0 ) newNodes.push_back( i );



    if( sepEval != nullptr ){
        VI bestNodes = StandardUtils::toVI( bestSepNodes );

//        if( bestNodes.size() != newNodes.size() ){
//            cerr << "best sep found in the middle of minimization, bestNodes.size() should be > newNodes.size()" << endl;
//            DEBUG(bestNodes.size());
//            DEBUG(newNodes.size());
//            DEBUG(bestSep);
//            DEBUG(sep);
//            ENDL(1);
//        }

        for( int& d : bestNodes ) d = sep.nodes[d];

        bestSep.nodes = bestNodes;
//        cerr << "returning bestSep: " << bestSep << endl;
        return bestSep;
    }



    sep = Separator(*sep.V, newNodes);

    unordered_set<int> neigh2;
    for(int p : sep.nodes){
        for(int d : gr[p]){
            if( d >= n ){
                sep.stats.maxCompSize = max( sep.stats.maxCompSize, nodeWeights[d] );
                sep.stats.maxCompEdges = max( sep.stats.maxCompEdges, edgeWeights[d] );

                sep.stats.maxCompAvgDeg = max( sep.stats.maxCompAvgDeg, (double)2*edgeWeights[d] / nodeWeights[d]  );
                sep.stats.maxCompDensity = max( sep.stats.maxCompDensity, (double)2*edgeWeights[d] / ( nodeWeights[d] * max(1,(nodeWeights[d]-1)) )  );

                neigh2.insert(d);
            }
        }
    }

    for( int c : neigh2 ){
        sep.stats.numberOfComponents++;
        sep.stats.compsSizeSquares += nodeWeights[c] * nodeWeights[c];
    }

    sep.stats.size = sep.nodes.size();
    sep.stats.originalGraphEdges = GraphUtils::countEdges(*sep.V);
    sep.stats.originalGraphSize = sep.V->size();

    for( int& d : sep.nodes ) d = oldNodes[d];


    if(debug){
        cerr << "Separator after minimization" << endl;
        DEBUG(sep);
        ENDL(3);
    }

//    DEBUG(nodesInsertedToZb);

    if( sep.stats.size != sep.nodes.size() ){
        cerr << "in minimizer stats.size != nodes.size()" << endl;
        DEBUG(sep);
        exit(1);
    }

//cerr << "leaving minimizer" << endl;
//    ENDL(2);

    return sep;
}



void GreedyNodeEdgeMinimizer::convertSepGraphToGr() {

    int N = sepGraph.size();
    gr = VVI(N);
    grW = VVI(N);

    for( int i=0; i<N; i++ ){
        gr[i].reserve(5);
        grW[i].reserve(5);
    }

    int n = sep->nodes.size();

    for( int i=0; i<n; i++ ){
        for( PII d : sepGraph[i] ){
            gr[i].push_back(d.first);
            grW[i].push_back(d.second);

            if( d.first >= n ){
                gr[d.first].push_back(i);
                grW[d.first].push_back(d.second);
            }
        }
    }
}

Separator GreedyNodeEdgeMinimizer::minimizeSeparator(Separator sep) {
    createSeparatorGraphDataForSeparator( sep, sepGraph, nodeWeights, edgeWeights );
    sep = minimizeSeparator( sep,sepGraph,nodeWeights, edgeWeights );
    return sep;
}


void GreedyNodeEdgeMinimizer::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    GreedyNodeEdgeMinimizer minim(MINIMIZE_EDGES);


    VI nodes = { 0,1,2,3 };
    Separator sep( V, nodes );
    sep.createSeparatorStats();

    DEBUG(sep);

    sep = minim.minimizeSeparator(sep);

    DEBUG(sep);



    exit(1);

}


