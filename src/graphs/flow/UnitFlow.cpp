/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <graphs/flow/UnitFlow.h>
#include <graphs/flow/MaxFlow.h>
#include <graphs/GraphUtils.h>
#include <graphs/GraphReader.h>
#include <graphs/generators/GraphGenerator.h>
#include "utils/TimeMeasurer.h"

#include "graphs/flow/UnitFlow.h"

UnitFlow::UnitFlow(VVI &V) {
    origV = V;
    useDfsAugmentation = false;
    preprocess();
}

int UnitFlow::calculateFlow( VI sources, VI ends, int maxFlowBound ){
    maxFlow = 0;
    if(maxFlowBound == -1) maxFlowBound = GraphUtils::countEdges(V);

    this->sources = unordered_set<int>(ALL(sources));
    this->targets = unordered_set<int>(ALL(ends));
    fill( ALL(isSource), false );
    fill( ALL(isTarget), false );


    for(int s : sources) isSource[s] = true;
    for(int s : ends) isTarget[s] = true;

//    GraphUtils::writeGraphHumanReadable(V);
    bool foundBetterFlow = true;
    while( maxFlow < maxFlowBound && foundBetterFlow ){

        foundBetterFlow = false;
        if( augmentFlowIteration() ){
            foundBetterFlow = true;
//            ENDL(1);
        }


    }
    return maxFlow;
}


VVI UnitFlow::getEdgeSaturation(){
    VVI sat(N);
    for(int i=0; i<N; i++){
        for( int j=0; j<origV[i].size(); j++ ) if( cap[i][j] == 0 ) sat[i].push_back( V[i][j] );
    }
    return sat;
}



void UnitFlow::addSource(int s) {
    assert( targets.count(s) == 0 );

    sources.insert(s);
    isSource[s] = true;
}

void UnitFlow::addTarget(int t) {
    assert( sources.count(t) == 0 );
    targets.insert(t);
    isTarget[t] = true;
}

void UnitFlow::removeTarget(int t) {
    targets.erase(t);
    isTarget[t] = false;
}

void UnitFlow::removeSource(int s) {
    sources.erase(s);
    isSource[s] = false;
}



void UnitFlow::augmentFlow() {
    while( augmentFlowIteration() ){
        // do nothing

//        DEBUG( getSourceReachableNodes() );
//        DEBUG( getTargetReachableNodes() );
//        ENDL(1);
    }
}

VI UnitFlow::getSourceReachableNodes() {
    VI reachable( ALL(sources) );
    for(int s : sources) was[s] = true;

    for( int i=0; i<reachable.size(); i++ ){
        int p = reachable[i];

        for( int i=0; i<V[p].size(); i++){
            if( cap[p][i] == 0 ) continue;
            int d = V[p][i];
            if( isSource[d] ) continue;

            if( !was[d] ){
                was[d] = true;
                reachable.push_back(d);
            }
        }
    }

    for(int r : reachable) was[r] = false;

    reachable.erase( reachable.begin(), reachable.begin() + sources.size() );

    return reachable;
}

VI UnitFlow::getSourceReachableNodes(int s, VB &was2) {
    VI reachable( 1,s );
    was[s] = true;

    for( int i=0; i<reachable.size(); i++ ){
        int p = reachable[i];

        for( int i=0; i<V[p].size(); i++){
            if( cap[p][i] == 0 ) continue;
            int d = V[p][i];
            if( isSource[d] || was2[d] || was[d] ) continue;

            if( !was[d] ){
                was[d] = true;
                reachable.push_back(d);
            }
        }
    }

    for(int r : reachable) was[r] = false;

    reachable.erase( reachable.begin(), reachable.begin() + 1 );

    return reachable;
}



VI UnitFlow::getTargetReachableNodes() {
    VI reachable( ALL(targets) );
    for(int t : targets) was[t] = true;

//    DEBUG(targets);
//    DEBUG(reachable);

    for( int i=0; i<reachable.size(); i++ ){
        int p = reachable[i];

//        DEBUG(p);
        for( int i=0; i<V[p].size(); i++){
            int d = V[p][i];
            if( isTarget[d] || was[d] ) continue;

//            DEBUG(d);

//            int ri = revInd[p][i];
//            if( cap[d][ri] == 0 ) continue;

            if( cap[d][ revInd[p][i] ] == 0 ) continue;

            if( !was[d] ){
                was[d] = true;
                reachable.push_back(d);
            }
        }
    }

    for(int r : reachable) was[r] = false;

    reachable.erase( reachable.begin(), reachable.begin() + targets.size() );

    return reachable;
}

VI UnitFlow::getTargetReachableNodes(int t, VB &was2) {
    VI reachable(1,t);
    was[t] = true;

    for( int i=0; i<reachable.size(); i++ ){
        int p = reachable[i];

        for( int i=0; i<V[p].size(); i++){
            int d = V[p][i];
            if( isTarget[d] || was2[d] || was[d] ) continue;

//            int ri = revInd[p][i];
//            if( cap[d][ri] == 0 ) continue;

            if( cap[d][ revInd[p][i] ] == 0 ) continue;

            if( !was[d] ){
                was[d] = true;
                reachable.push_back(d);
            }
        }
    }

    for(int r : reachable) was[r] = false;

    reachable.erase( reachable.begin(), reachable.begin() + 1 );

    return reachable;
}

void UnitFlow::preprocess() {
    N = origV.size();
    V = VVI(N);
    isSource = VB(N,false);
    isTarget = VB(N,false);
    inLayer = VI(N,-1);
    cap = VVI(N);
    maxFlow = 0;
    revInd = VVI(N);
    was = VB(N,false);

    // version 3, for fast creation of edge saturation. Here edges in V[i] and origV[i] are the same up to origV[i].size()

    struct Edge{
        Edge(int aa, int bb, int in) : a(aa), b(bb), ind(in){ minab = min(a,b); maxab = max(a,b); }
        int a,b,minab,maxab,ind;
        bool operator<(const Edge& oth){ if( minab != oth.minab ) return minab < oth.minab; else return maxab < oth.maxab; }
        bool operator==(const Edge& oth){ return minab == oth.minab && maxab == oth.maxab; }
    };


    V = origV;
    cap = V; // temporary, just initialization of sizes of vector cap
    revInd = V; // temporary, just initialization of sizes of vector revInd

    vector<Edge> edges;
    edges.reserve( 2*GraphUtils::countEdges(origV) );
    for(int i=0; i<N; i++){
        for(int j=0; j<origV[i].size(); j++){
            cap[i][j] = 1; // setting initial capacities
            edges.emplace_back( i,V[i][j],j );
        }
    }

    sort(ALL(edges));

    int p = 0;
    while( p < edges.size() ){
        bool isBidirectional = false;
        if( p+1 < edges.size() && edges[p+1] == edges[p] ) isBidirectional = true;

        if( isBidirectional ){
            revInd[ edges[p].a ][ edges[p].ind ] = edges[p+1].ind;
            revInd[ edges[p+1].a ][ edges[p+1].ind ] = edges[p].ind;

            p+=2;
        }else{
            int a = edges[p].a;
            int b = edges[p].b;

            V[b].push_back(a);
            cap[b].push_back(0);
            revInd[b].push_back( edges[p].ind );

            revInd[a][ edges[p].ind ] = (int)V[b].size()-1;
            p++;
        }
    }

//    DEBUG(edges);
//    DEBUG(bidirectionalEdges);
//
//    DEBUG(V);
//    DEBUG(cap);
//    DEBUG(revInd);

    for( int i=0; i<N; i++ ){
        for( int j=0; j<V[i].size(); j++ ){
            int d = V[i][j];
            int ri = revInd[i][j];
            assert( V[d][ri] == i );
        }
    }

}

VI UnitFlow::createLayers() {
    for(int s : sources) inLayer[s] = 0;
    VI neigh( ALL(sources) );
//    VI neigh; neigh.reserve( N ); neigh.insert( neigh.end(), ALL(sources) );


    int foundTarget = -1;

    for( int i=0; i<neigh.size(); i++ ){
        int p = neigh[i];
        if( foundTarget != -1 && inLayer[p] >= inLayer[foundTarget] ) continue; // i create layers only up to the layer, where the first target is found


        for( int j=0; j<V[p].size(); j++ ){
            if( cap[p][j] <= 0 ) continue; // if no capacity, then there is no such edge.
            int d = V[p][j];
            if( isSource[d] || inLayer[d] != -1 ) continue;

            neigh.push_back(d);
            inLayer[d] = inLayer[p]+1;
            if( isTarget[d] && foundTarget == -1 ){
                foundTarget = d;
            }
        }
    }


    return neigh;

}

bool UnitFlow::augmentBFS(){
    VI visited = createLayers();
//    cerr << "layers created" << endl;

    function<bool(int)> dfs = [=, &dfs]( int num ){
//        cerr << "num = " << num << endl;

        if( isTarget[num] ) return true;
        was[num] = true;


        for( int i=0; i<V[num].size(); i++ ){
            if( cap[num][i] == 0 ) continue;

            int d = V[num][i];
            if( was[d] || inLayer[d] != inLayer[num] + 1 ) continue;

//            cerr << "\tchecking neighbor " << d << endl;

            if( dfs(d) ){
                was[num] = false; // resetting was, perhaps there is another path via this node

//                cerr << "found path, reversing edge " << num << " -> " << d << endl;

                // now reversing capacities of edge num->d
                cap[num][i]--;

//                int ri = revInd[num][i];
//                cap[d][ri]++;
                cap[d][ revInd[num][i] ]++;

                // and returning true, as path is found
                return true;
            }
        }

        return false;
    };

    bool improved = false;
    for( int s : sources ){

        assert( isTarget[s] == false );

//        DEBUG(s);
        while( !was[s] && dfs(s) ){
            improved = true;
            maxFlow++;
//            ENDL(1);
        }
    }

    for( int p : visited ){
        inLayer[p] = -1;
        was[p] = false;
    }

//    DEBUG(cap);

    return improved;

}

bool UnitFlow::augmentDFS(){

    function<bool(int) > dfs = [=, &dfs](int num){
        if( was[num] ) return false;
        if( isTarget[num] ) return true;
        was[num] = true;

        for( int i=0; i<V[num].size(); i++ ){ // here we look at neighbors before proceeding by dfs traverse - it may be faster than simple dfs
            if( cap[num][i] == 0 ) continue;
            int d = V[num][i];
            if( isTarget[d] ){
                was[num] = false;
                cap[num][i]--;
                cap[d][ revInd[num][i] ]++;
                return true;
            }
        }

        for( int i=0; i<V[num].size(); i++ ){
            if( cap[num][i] == 0 ) continue;

            int d = V[num][i];
            if( was[d] ) continue;

            if( dfs(d) ){
                was[num] = false; // resetting was, perhaps there is another path via this node

                cap[num][i]--;
                int ri = revInd[num][i];
                cap[d][ri]++;

                return true;
            }
        }

        return false;

    };

    VB oldWas = was;
    bool improved = false;
    for( int s : sources ){
        assert( isTarget[s] == false );
        while( !was[s] && dfs(s) ){
            improved = true;
            maxFlow++;
        }
    }
    swap( was, oldWas );
    return improved;
}

bool UnitFlow::augmentFlowIteration() {
    if( useDfsAugmentation ) return augmentDFS();
    else return augmentBFS();
}

void UnitFlow::writeSaturatedEdges() {
    auto sat = getEdgeSaturation();
    cerr << "Saturated edges: " << endl;
    for( int i=0; i<N; i++ ){
        for(int d : sat[i]) cerr << "(" << i << "," << d << ")   ";
    }
    cerr << endl;
}



void UnitFlow::test() {
//    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
//    DEBUG(V.size());
//    DEBUG( GraphUtils::countEdges(V) );

    VVI V = { {1}, {2}, {3}, {4}, {1} };

    UnitFlow uf(V);
    uf.calculateFlow(  {0}, {4} );
    uf.writeSaturatedEdges();
    DEBUG(uf.getSourceReachableNodes());
    DEBUG(uf.getTargetReachableNodes());





    bool testTime = false;
    if(testTime){
        cerr << "Starting flow time measurements" << endl;
        int REPETS = 5;
        int N = 50'000;
        for( int i=0; i<REPETS; i++ ) {
            cerr << "\r#" << (i+1) << " / " << REPETS << flush;
            VVI V = GraphGenerator::getRandomGraph( N, 2*N );
            VI sources = {0,1,2,3,4};
            VI targets = {5,6,7,8,9};



            TimeMeasurer::startMeasurement("UnitFlow");
            UnitFlow uf(V);
            int ufFlow = uf.calculateFlow( sources,targets );
            uf.getEdgeSaturation();

            TimeMeasurer::stopMeasurement("UnitFlow");



            V.push_back({});
            for(int s : sources){
                V.back().push_back(s);
                V[s].push_back( V.size()-1 );
            }

            V.push_back({});
            for(int t : targets){
                V.back().push_back(t);
                V[t].push_back( V.size()-1 );
            }

            TimeMeasurer::startMeasurement("MaxFlow");
            vector<unordered_map<int,int>> W(N+2);
            for(int i=0; i<N+2; i++) for(int d : V[i]) W[i][d] = 1;
            for(int s : sources) W[N][s] = Constants::INF;
            for(int t : targets) W[t][N+1] = Constants::INF;
            MaxFlow mf(V,W);
            int mfFlow = mf.calculateMaxFlow( N,N+1 );
            mf.getEdgeSaturation();

            TimeMeasurer::stopMeasurement("MaxFlow");

            assert( ufFlow == mfFlow );

//            DEBUG(ufFlow);
        }

        cerr << "PASSED!" << endl;

        TimeMeasurer::writeAllMeasurements();
    }


    exit(1);
}


