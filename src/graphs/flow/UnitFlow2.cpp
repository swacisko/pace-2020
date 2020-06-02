/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <graphs/flow/UnitFlow2.h>
#include <graphs/flow/MaxFlow.h>
#include <graphs/GraphUtils.h>
#include <graphs/GraphReader.h>
#include <graphs/generators/GraphGenerator.h>
#include "utils/TimeMeasurer.h"

#include "graphs/flow/UnitFlow2.h"

UnitFlow2::UnitFlow2(VVI &V) {
    origV = V;
    useDfsAugmentation = false;
    preprocess();
}

int UnitFlow2::calculateFlow( VI sources, VI ends, int maxFlowBound ){
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
        }
    }

    updateRevV();
    return maxFlow;
}

void UnitFlow2::updateRevV() {
    for( PII& e : edgesToReverseInRevV ){
        int a = e.first, b = e.second;
        for( int i=(int)revV[a].size()-1; i>=0; i-- ){
            if( revV[a][i] == b ){
                swap(revV[a][i], revV[a].back() );
                revV[a].pop_back();
                revV[b].push_back(a);
                break;
            }
        }
    }
    edgesToReverseInRevV.clear();
}


VVI UnitFlow2::getEdgeSaturation(){
    ERROR("", "getEdgeSaturation not supported yet");
    VVI sat(N);
//    for(int i=0; i<N; i++){
//        for( int j=0; j<origV[i].size(); j++ ) if( cap[i][j] == 0 ) sat[i].push_back( V[i][j] );
//    }
    return sat;
}



void UnitFlow2::addSource(int s) {
    assert( targets.count(s) == 0 );

    sources.insert(s);
    isSource[s] = true;
}

void UnitFlow2::addTarget(int t) {
    assert( sources.count(t) == 0 );
    targets.insert(t);
    isTarget[t] = true;
}

void UnitFlow2::removeTarget(int t) {
    targets.erase(t);
    isTarget[t] = false;

    for( int t1 : V[t] ) GraphUtils::removeEdge( revV,t1,t,true );
    for( int t1 : revV[t] ) GraphUtils::removeEdge( V, t1,t,true );

    V[t].clear();
    revV[t].clear();

}

void UnitFlow2::removeSource(int s) {
    sources.erase(s);
    isSource[s] = false;

    for( int s1 : V[s] ) GraphUtils::removeEdge( revV,s1,s,true );
    for( int s1 : revV[s] ) GraphUtils::removeEdge( V, s1,s,true );

    V[s].clear();
    revV[s].clear();
}


void UnitFlow2::augmentFlow() {
    while( augmentFlowIteration() ){
        // do nothing

//        DEBUG( getSourceReachableNodes() );
//        DEBUG( getTargetReachableNodes() );
//        ENDL(1);
    }

    updateRevV();
}

VI UnitFlow2::getSourceReachableNodes() {
//    cerr << "SR1" << endl;
    VI reachable( ALL(sources) );
    for(int s : sources) was[s] = true;

    for( int i=0; i<reachable.size(); i++ ){
        int p = reachable[i];

        for( int i=0; i<V[p].size(); i++){
            if( isSource[ V[p][i] ] ) continue;
            int d = V[p][i];

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

VI UnitFlow2::getSourceReachableNodes(int s, VB &was2) {
//    cerr << "SR2" << endl;
    VI reachable( 1,s );
    was[s] = true;

    for( int i=0; i<reachable.size(); i++ ){
        int p = reachable[i];

        for( int i=0; i<V[p].size(); i++){
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



VI UnitFlow2::getTargetReachableNodes() {
//    cerr << "TR1" << endl;
    VI reachable( ALL(targets) );
    for(int t : targets) was[t] = true;

//    DEBUG(targets);
//    DEBUG(reachable);

    for( int i=0; i<reachable.size(); i++ ){
        int p = reachable[i];

//        DEBUG(p);
        for( int i=0; i<revV[p].size(); i++){
            int d = revV[p][i];
            if( isTarget[d] || was[d] ) continue;

            was[d] = true;
            reachable.push_back(d);

        }
    }

    for(int r : reachable) was[r] = false;

    reachable.erase( reachable.begin(), reachable.begin() + targets.size() );

    return reachable;
}

VI UnitFlow2::getTargetReachableNodes(int t, VB &was2) {
//    cerr << "TR1" << endl;
    VI reachable(1,t);
    was[t] = true;

    for( int i=0; i<reachable.size(); i++ ){
        int p = reachable[i];

        for( int i=0; i<revV[p].size(); i++){
            int d = revV[p][i];
            if( isTarget[d] || was2[d] || was[d] ) continue;

            was[d] = true;
            reachable.push_back(d);
        }
    }

    for(int r : reachable) was[r] = false;

    reachable.erase( reachable.begin(), reachable.begin() + 1 );

    return reachable;
}

void UnitFlow2::preprocess() {
    N = origV.size();
    V = VVI(N);
    isSource = VB(N,false);
    isTarget = VB(N,false);
    inLayer = VI(N,-1);
    maxFlow = 0;
    was = VB(N,false);
    V = origV;
    revV = GraphUtils::reverseGraph(V);
    edgesToReverseInRevV.reserve(N);
}

VI UnitFlow2::createLayers() {
//    cerr << "layers" << endl;
    for(int s : sources) inLayer[s] = 0;
    VI neigh( ALL(sources) );
//    VI neigh; neigh.reserve( N ); neigh.insert( neigh.end(), ALL(sources) );

    int foundTarget = -1;

    for( int i=0; i<neigh.size(); i++ ){
        int p = neigh[i];
        if( foundTarget != -1 && inLayer[p] >= inLayer[foundTarget] ) continue; // i create layers only up to the layer, where the first target is found

        for( int j=0; j<V[p].size(); j++ ){
            int d = V[p][j];
            if( was[d] || inLayer[d] != -1 ) continue;

            neigh.push_back(d);
            inLayer[d] = inLayer[p]+1;
            if( foundTarget == -1 && isTarget[d] ) foundTarget = d;
        }
    }

    return neigh;
}

bool UnitFlow2::augmentBFS(){
//    cerr << "augBFS" << endl;
    VI visited = createLayers();

    function<bool(int)> dfs = [=, &dfs]( int num ){
        if( isTarget[num] ) return true;
        was[num] = true;


        for( int i=0; i<V[num].size(); i++ ){

            int d = V[num][i];
            if( was[d] || inLayer[d] != inLayer[num] + 1 ) continue;

            if( dfs(d) ){
                was[num] = false; // resetting was, perhaps there is another path via this node

                // now reversing edge num->d
                V[d].push_back(num);
                swap( V[num][i], V[num].back() );
                V[num].pop_back();
                edgesToReverseInRevV.push_back( {d,num} );

                // and returning true, as path is found
                return true;
            }
        }
        return false;
    };

    bool improved = false;
    for( int s : sources ){
        assert( isTarget[s] == false );
        while( !was[s] && dfs(s) ){
            improved = true;
            maxFlow++;
        }
    }

    for( int p : visited ){
        inLayer[p] = -1;
        was[p] = false;
    }

    return improved;
}

bool UnitFlow2::augmentDFS(){

    function<bool(int) > dfs = [=, &dfs](int num){
        if( was[num] ) return false;
        if( isTarget[num] ) return true;
        was[num] = true;

        for( int i=0; i<V[num].size(); i++ ){ // here we look at neighbors before proceeding by dfs traverse - it may be faster than simple dfs
            int d = V[num][i];
            if( isTarget[d] ){
                was[num] = false;

                V[d].push_back(num);
                swap( V[num][i], V[num].back() );
                V[num].pop_back();
                edgesToReverseInRevV.push_back( {d,num} );

                return true;
            }
        }

        for( int i=0; i<V[num].size(); i++ ){

            if( was[ V[num][i] ] ) continue;
            int d = V[num][i];

            if( dfs(d) ){
                was[num] = false; // resetting was, perhaps there is another path via this node

                V[d].push_back(num);
                swap( V[num][i], V[num].back() );
                V[num].pop_back();
                edgesToReverseInRevV.push_back( {d,num} );

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

bool UnitFlow2::augmentFlowIteration() {
    if( useDfsAugmentation ) return augmentDFS();
    else return augmentBFS();
}

void UnitFlow2::writeSaturatedEdges() {
    auto sat = getEdgeSaturation();
    cerr << "Saturated edges: " << endl;
    for( int i=0; i<N; i++ ){
        for(int d : sat[i]) cerr << "(" << i << "," << d << ")   ";
    }
    cerr << endl;
}



void UnitFlow2::test() {
//    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
//    DEBUG(V.size());
//    DEBUG( GraphUtils::countEdges(V) );

   /* VVI V = { {1}, {2}, {3}, {4}, {1} };

    UnitFlow2 uf(V);
    uf.calculateFlow(  {0}, {4} );
    uf.writeSaturatedEdges();
    DEBUG(uf.getSourceReachableNodes());
    DEBUG(uf.getTargetReachableNodes());*/





    bool testTime = true;
    if(testTime){
        cerr << "Starting flow time measurements" << endl;
        int REPETS = 20;
        int N = 100'000;
        for( int i=0; i<REPETS; i++ ) {
            cerr << "\r#" << (i+1) << " / " << REPETS << flush;
            VVI V = GraphGenerator::getRandomGraph( N, 10*N );
            VI sources = {0,1,2,3,4};
            VI targets = {5,6,7,8,9};



            TimeMeasurer::startMeasurement("UnitFlow2");
            UnitFlow2 uf(V);
            int ufFlow = uf.calculateFlow( sources,targets );
//            uf.getEdgeSaturation();

            TimeMeasurer::stopMeasurement("UnitFlow2");



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
//            mf.getEdgeSaturation();

            TimeMeasurer::stopMeasurement("MaxFlow");

            assert( ufFlow == mfFlow );

//            DEBUG(ufFlow);
        }

        cerr << "PASSED!" << endl;

        TimeMeasurer::writeAllMeasurements();
    }


    exit(1);
}




