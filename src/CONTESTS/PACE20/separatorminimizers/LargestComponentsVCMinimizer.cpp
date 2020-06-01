//
// Created by sylwester on 3/29/20.
//

#include <CONTESTS/PACE20/separatorminimizers/LargestComponentsVCMinimizer.h>
#include <graphs/components/ConnectedComponents.h>
#include <graphs/GraphUtils.h>
#include <graphs/GraphReader.h>
#include <graphs/VertexCover/BipartiteGraphVertexCover.h>
#include <graphs/GraphInducer.h>

#include "CONTESTS/PACE20/separatorminimizers/LargestComponentsVCMinimizer.h"


Separator LargestComponentsVCMinimizer::minimizeSeparator(Separator sep) {
//    cerr << "in lcM" << endl;
    VVI comps = ConnectedComponents::getConnectedComponents( *sep.V, sep.nodes );

    sort( ALL(comps), []( VI& v1, VI& v2 ){ return v1.size() > v2.size(); } );

    int sizesSum = accumulate( ALL(comps), 0, [](int a, VI& c){ return a + c.size(); } );

    int minNeighCompSize = sizeBalance * sizesSum;

//    DEBUG(sizesSum);
//    DEBUG(minNeighCompSize);
    int ind = 1;
    int curSize = 0;
    VI cmp;
    for( int i=0; i<comps.size(); i++ ){
        cmp.insert(cmp.end(), ALL(comps[i]));
        curSize += comps[i].size();
        if( curSize >= minNeighCompSize ) break;
    }

    VI neigh = GraphUtils::getNeighborhoodOfAInB( *sep.V, sep.nodes, cmp, false );

//    DEBUG(sep.nodes);
//    DEBUG(cmp);
//    DEBUG(neigh);

    VI inducer = sep.nodes;
    inducer.insert( inducer.end(), ALL(neigh) );

    sort(ALL(inducer));
//    DEBUG(inducer);

    InducedGraph gr = GraphInducer::induce(*sep.V,inducer);

//    DEBUG(gr.V);
    unordered_set<int> inSep(ALL(sep.nodes));
    for( int i=0; i<sep.nodes.size(); i++ ){
        int p = sep.nodes[i];
        int mappedP = gr.perm[p];
        for( int j=(int)gr.V[mappedP].size()-1; j>=0; j-- ){
            int d = gr.V[mappedP][j];
            if( inSep.count( gr.nodes[d] ) ){
                swap( gr.V[mappedP][j], gr.V[mappedP].back() );
                gr.V[mappedP].pop_back();
            }
        }
    }

//    DEBUG(gr.V);

    VB bipartition(gr.V.size(),false);
    for(int p : neigh) bipartition[ gr.perm[p] ] = true;

//    for( int p : inducer ){
//        cerr << "gr.perm[" << p << "] = " << gr.perm[p] << endl;
//    }


    VI vc = BipartiteGraphVertexCover::getVertexCoverOfBipartiteGraph( gr.V, bipartition );
    for(int& d : vc) d = gr.nodes[d];

//    DEBUG(vc);

    Separator newSep(*sep.V, vc);
    newSep.createSeparatorStats();
    newSep.updatePointers(*sep.V);

//    DEBUG(sep);
//    DEBUG(newSep);

//    exit(1);
    return newSep;

}

void LargestComponentsVCMinimizer::test(){

    VVI V;
    V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    VI nodes = { 0,1,2,3,4 };
    Separator sep(V,nodes);
    sep.createSeparatorStats();
    DEBUG(sep);

    LargestComponentsVCMinimizer minim;
    auto newSep = minim.minimizeSeparator(sep);
    DEBUG(newSep);


    exit(1);

}

Separator LargestComponentsVCMinimizer::minimizeSeparator(Separator sep, double sizeBalance) {
    this->sizeBalance = sizeBalance;
    return minimizeSeparator(sep);
}

