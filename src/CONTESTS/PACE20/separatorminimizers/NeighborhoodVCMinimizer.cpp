/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/separatorminimizers/NeighborhoodVCMinimizer.h>
#include <graphs/GraphUtils.h>
#include <graphs/GraphInducer.h>
#include <graphs/GraphReader.h>

#include "CONTESTS/PACE20/separatorminimizers/NeighborhoodVCMinimizer.h"
#include "graphs/VertexCover/BipartiteGraphVertexCover.h"

Separator NeighborhoodVCMinimizer::minimizeSeparator(Separator sep) {
//    cerr << "entering minimizer" << endl;
    VVI V = *sep.V;
    VI nodes = sep.nodes;
    int N = V.size();

//    DEBUG(nodes);
    VI neigh = GraphUtils::getNeighborhood( V, nodes, false );
//    DEBUG(neigh);
    VB inNodes(N,false);
    for(int d : nodes) inNodes[d] = true;

    for( int i = (int)neigh.size()-1; i>=0; i-- ){
        int p = neigh[i];
        if( inNodes[p] ){
            swap(neigh[i], neigh.back());
            neigh.pop_back();
        }
    }
    sort(ALL(neigh));
//    DEBUG(neigh);

//    DEBUG(nodes.size());
//    DEBUG(neigh.size());


    VI inducer = neigh;
    inducer.insert( inducer.end(), ALL(nodes) );

    sort(ALL(inducer));
//    DEBUG(inducer);

    InducedGraph gr = GraphInducer::induce(V,inducer);

    VB bipartition(gr.V.size(),false);
    for(int p : nodes) bipartition[ gr.perm[p] ] = true;

    VI vc = BipartiteGraphVertexCover::getVertexCoverOfBipartiteGraph( gr.V, bipartition );
    for(int& d : vc) d = gr.nodes[d];
//    DEBUG(vc.size());

    Separator newSep(V,vc);
    newSep.createSeparatorStats();
    newSep.updatePointers( *sep.V );

//    cerr << "leaving minimizer" << endl;
    return newSep;
}

void NeighborhoodVCMinimizer::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    VI nodes = { 1,2,3,4,5 };
    Separator sep(V,nodes);

    NeighborhoodVCMinimizer minim;
    auto newSep = minim.minimizeSeparator(sep);
    DEBUG(newSep);


    exit(1);



}