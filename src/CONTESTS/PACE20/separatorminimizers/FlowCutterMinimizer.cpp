/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <CONTESTS/PACE20/separatorminimizers/FlowCutterMinimizer.h>
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include <graphs/GraphReader.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <graphs/graphtraversals/BFS.h>
#include <CONTESTS/PACE20/separatorcreators/FlowCutter.h>
#include "graphs/GraphInducer.h"
#include "utils/StandardUtils.h"

Separator FlowCutterMinimizer::minimizeSeparator(Separator sep) {
    if( sep.stats.numberOfComponents == 1 ) return sep;

    VVI* V = sep.V;
    int N = V->size();

    VVI comps = ConnectedComponents::getConnectedComponents( *V, sep.nodes );

    VI edgesInComp( N,0 );
    VB inComp(N,false);
    for( VI& v : comps ){
        for(int d : v) inComp[d] = true;
        int edges = 0;
        for(int p : v){
            for(int d : (*V)[p]) if( inComp[d] ) edges++;
        }
        for(int p : v){
            edgesInComp[p] = edges;
            inComp[p] = false;
        }
    }

//    DEBUG(edgesInComp);

    sort( ALL(comps), [&edgesInComp]( VI& v1, VI& v2 ){ return edgesInComp[v1[0]] > edgesInComp[v2[0]]; } );


    if( MINIMIZATION_MODE == DISTANCE_MINIMIZATION ) return getDistanceMinimizedSeparator( V, sep, comps, edgesInComp );
    else return getFurthestPointMinimizedSeparator( sep, comps );

}

Separator
FlowCutterMinimizer::getDistanceMinimizedSeparator(VVI *V, Separator &sep, VVI &comps, VI& edgesInComp) {

//    double balance = Pace20Params::balance;
    double balance = 0.25;
    int totalEdges = GraphUtils::countEdges(*V);

//    cerr << "comps sizes: "; for(VI& v : comps) cerr << v.size() << " "; cerr << endl;

    double largestCompsEdges = 0;
    VI largestCompsNodes;
//    VI restCompsNodes;
    for( int i=0; i<comps.size(); i++ ){
        largestCompsNodes.insert( largestCompsNodes.end(), ALL(comps[i]) );
        largestCompsEdges += edgesInComp[ comps[i][0] ];
        if( largestCompsEdges > (1-balance) * totalEdges ){
//            for( int j=i+1; j<comps.size(); j++ ) restCompsNodes.insert( restCompsNodes.end(), ALL(comps[j]) );
            break;
        }
    }

    int N = V->size();

    int distLargerPart = leftDistance;
    int distSmallerPart = rightDistance;

    VI largerPartSources;
    VI smallerPartEnds;

    VB inLargerPart(N,false);
    for( int p : largestCompsNodes ) inLargerPart[p] = true;
    VVI layers = BFS::getBfsLayers( *V, sep.nodes );

    int L = min( (int)layers.size()-1, distLargerPart );
    for( int p : layers[L] ) if( inLargerPart[p] ) largerPartSources.push_back(p);

    int l = min( (int)layers.size()-1, distSmallerPart );
    for(int p : layers[l]) if( !inLargerPart[p] ) smallerPartEnds.push_back(p);

    if( smallerPartEnds.empty() || largerPartSources.empty() ) return sep;

//    DEBUG(L);
//    DEBUG(l);
//    DEBUG(largerPartSources);
//    DEBUG(smallerPartEnds);

    VI inducerSet;
    for( int i=0; i<=L; i++ ) for( int d : layers[i] ) if( inLargerPart[d] ) inducerSet.push_back(d);
    for( int i=0; i<=l; i++ ) for( int d : layers[i] ) if( !inLargerPart[d] ) inducerSet.push_back(d);

//    DEBUG(inducerSet);

//    DEBUG((*V));
    InducedGraph g = GraphInducer::induce( *V, inducerSet);



    VI sources = largerPartSources;
    for(int& d : sources) d = g.perm[d];

    VI ends = smallerPartEnds;
    for(int& d : ends) d = g.perm[d];


    // adding virtual source
    int n = g.V.size();
    g.V.push_back(VI()); g.nodes.push_back(-1);
    g.V.push_back(VI()); g.nodes.push_back(-1);


    for( int s : sources ) GraphUtils::addEdge( g.V, n, s );
    for( int e : ends ) GraphUtils::addEdge( g.V, n+1,e );

    sources.push_back(n);
    ends.push_back(n+1);

    assert( ConnectedComponents::getConnectedComponents(g.V).size() );

    FlowCutter fc(*sepEval);
    auto expV = fc.getExpansionGraph(g.V);
    for( int & e : ends ) e += n+2;

    auto seps = fc.getSeparatorsForSourcesAndTargets( g.V, expV, sources, ends );
    for(auto& sp : seps) sp.updatePointers(g.V);

    auto bestSep = *min_element( ALL(seps), *sepEval );
    bestSep.updatePointers(g.V);

    auto sepNodes = bestSep.nodes;
    for(int& d : sepNodes){

//        assert( g.nodes[d] != -1 );

        if( g.nodes[d] == -1 ){
            return sep;

        }
        else d = g.nodes[d];
    }
//    DEBUG(sepNodes);

    sort( ALL(sepNodes) );
    sepNodes.resize( unique(ALL(sepNodes)) - sepNodes.begin() );

    Separator res(*V,sepNodes);
    res.createSeparatorStats();


    return res;
}

Separator FlowCutterMinimizer::getFurthestPointMinimizedSeparator(Separator &sep, VVI &comps) {
    if( comps.size() == 1 ) return sep;

    VVI* V = sep.V;
    int N = V->size();
    VI C0 = comps[0];
    VI C1 = comps[1];

    VB inC0 = StandardUtils::toVB(N,C0);
    VB inC1 = StandardUtils::toVB(N,C1);

    double balance = 0.33;

    VVI layers = BFS::getBfsLayers( *V, sep.nodes );

    int cnt = 0;
    VI sources;
    for( int i=(int)layers.size()-1; i>=0; i-- ){
        random_shuffle( ALL(layers[i]) );
        for( int p : layers[i] ){
            if( inC0[p] ){
                sources.push_back(p);
                cnt++;
            }

            if( cnt > balance * C0.size() ) break;
        }

        if( cnt > balance * C0.size() ) break;
    }

//    DEBUG(sources);

    cnt = 0;
    VI targets;
    for( int i=(int)layers.size()-1; i>=0; i-- ){
        random_shuffle( ALL(layers[i]) );
        for( int p : layers[i] ){
            if( inC1[p] ){
                targets.push_back(p);
                cnt++;
            }

            if( cnt > balance * C1.size() ) break;
        }

        if( cnt > balance * C1.size() ) break;
    }

//    DEBUG(targets);

    FlowCutter fc(*sepEval);


    auto expV = fc.getExpansionGraph(*V);

    auto seps = fc.getSeparatorsForSourcesAndTargets( *V, expV, sources, targets );
    for(auto& sp : seps) sp.updatePointers(*V);

    auto bestSep = *min_element( ALL(seps), *sepEval );
    bestSep.updatePointers(*V);

    return bestSep;

}


void FlowCutterMinimizer::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );


    Separator sep( V, { 4,7,8,15,16,19,20 } );
    sep.createSeparatorStats();
    DEBUG(sep);

    FlowCutterMinimizer fcMinim( SeparatorEvaluators::sepEvalToUse );
    fcMinim.MINIMIZATION_MODE = FURTHEST_POINT_MINIMIZATION;
    sep = fcMinim.minimizeSeparator(sep);

    DEBUG(sep);


    exit(1);
}



