/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/separatorminimizers/SeparatorMinimizer.h>
#include <graphs/components/ConnectedComponents.h>

void SeparatorMinimizer::createSeparatorGraphDataForSeparator(Separator &sep, VVPII &sepGraph, VI &nodeWeights, VI &edgeWeights) {
    VVI comps = ConnectedComponents::getConnectedComponents( *sep.V, sep.nodes );

    int N = sep.nodes.size();
    int C = comps.size();
    VI compId(sep.V->size(),-1);
    for( int i=0; i<comps.size(); i++ ){
        for( int d : comps[i] ) compId[d] = N+i;
    }

    for(int i=0; i<sep.nodes.size(); i++) compId[ sep.nodes[i] ] = i;

    vector< unordered_map<int,int> > degrees(N);
    for(int i=0; i<sep.nodes.size(); i++){
        int p = sep.nodes[i];
        for( int d : (*sep.V)[p] ){
            degrees[i][ compId[d] ]++;
        }
    }

    sepGraph = VVPII(N+C);
    for(int i=0; i<N; i++){
        for( PII p : degrees[i] ){
            sepGraph[i].push_back(p);
            if( p.first >= N ) sepGraph[ p.first ].push_back( {i,p.second} );
        }
    }

//    DEBUG(degrees);
//    DEBUG(sepGraph);
//    exit(1);

    nodeWeights = VI(N+C,0);
    edgeWeights = VI(N+C,0);
    for(int i=0; i<N; i++) nodeWeights[i] = 1;
    for(int i=N; i<N+C; i++){
        nodeWeights[i] = comps[i-N].size();
        for(int p : comps[i-N]){
            for(int d : (*sep.V)[p] ){
                if( compId[d] == i ) edgeWeights[i]++;
            }
        }
        edgeWeights[i] >>= 1;
    }

//    DEBUG(comps);


}