/*
 * Copyright (c) 2020, Sylwester Swat
*/
#include "CONTESTS/PACE20/separatorminimizers/FlowMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/GreedyNodeEdgeMinimizer.h"
#include "CONTESTS/PACE20/separatorminimizers/LargestComponentsVCMinimizer.h"
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include <graphs/GraphReader.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <graphs/graphtraversals/BFS.h>
#include <graphs/GraphInducer.h>
#include "graphs/flow/DisjointPaths.h"



Separator FlowMinimizer::minimizeSeparator( Separator sep ){
//    sep.createSeparatorStats();
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

    double balance = Pace20Params::balance;
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

    int distLargerPart = 2;
    int distSmallerPart = 2;

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
    VVI paths = DisjointPaths::getSetOfDisjointPaths( g.V,sources,ends );

    for( VI& pth : paths ) for(int& d : pth) d = g.nodes[d];
//    DEBUG(paths);

    VI internalNodes;
    for( VI& pth : paths ) for(int i=1; i<pth.size()-1; i++) internalNodes.push_back( pth[i] );

//    DEBUG(internalNodes);

    GreedyNodeEdgeMinimizer gneMinim(GreedyNodeEdgeMinimizer::MINIMIZE_EDGES);
    LargestComponentsVCMinimizer lcMinim;


    Separator res(*V,internalNodes);
    res.createSeparatorStats();
//    DEBUG(res);

    if( res.stats.size <= Pace20Params::maxSeparatorSizeForGNEMinimizer ){
        res = gneMinim.minimizeSeparator(res);
//        cerr << "Minimizing witg gne" << endl;
    }
    else{
//        cerr << "minimizing with lc" << endl;
        res = lcMinim.minimizeSeparator(res);
    }

//    res.updatePointers(*V);
//    res.createSeparatorStats();
//    DEBUG(res);


    return res;

}


void FlowMinimizer::test(){
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    VI nodes = { 4,5,6 };
    DEBUG(nodes);
    Separator sep(V,nodes);
    sep.createSeparatorStats();
    DEBUG(sep);

    FlowMinimizer minim;
    auto newSep = minim.minimizeSeparator(sep);
    DEBUG(newSep);


    exit(1);
}