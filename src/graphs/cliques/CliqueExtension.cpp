/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <graphs/cliques/CliqueExtension.h>

#include "graphs/cliques/CliqueExtension.h"
#include "graphs/cliques/CliqueUtils.h"
#include "graphs/GraphUtils.h"
#include "graphs/GraphInducer.h"
#include "graphs/GraphReader.h"

VI CliqueExtension::maximizeCliqueRandom( VVI& V, VI& clq ){
    if( CliqueUtils::isClique(V,clq) == false ){
        cerr << "clq is not a clique" << endl; exit(1);
    }


}



VI CliqueExtension::maximizeCliqueGreedy(VVI& V, VI clq){
    assert( CliqueUtils::isClique(V,clq) );

    VI A = clq;
//    DEBUG(A);
    unordered_set<int> zbA(ALL(A));
    map<int,int> deg;
    for( int a : A ){
        for(int d : V[a]){
            if( !zbA.count(d) ) deg[d]++;
        }
    }

    VI B;
    for( PII p : deg ){
        if( p.second == A.size() ) B.push_back(p.first);
    }

    random_shuffle(ALL(B)); // this just makes some randomness in moving neighbors from B to A

//    DEBUG(B);


    VI inducer = A; inducer.insert( inducer.end(), ALL(B) );

    InducedGraph g = GraphInducer::induce( V, inducer );
    for( int & a : A ) a = g.perm[a];
    for( int & b : B ) b = g.perm[b];

    int n = g.V.size();
    VI neighCnt( n,0 );
    VB inA(n,false), inB(n,false);
    for(int a : A) inA[a] = true;
    for(int b : B) inB[b] = true;

    for( int p : B ){
        for(int d : g.V[p]){
            if( inB[d] ) neighCnt[d]++;
        }
    }

    VB Nx(n,false);

    auto comp =  [&neighCnt]( int a, int b ){ return neighCnt[a] < neighCnt[b]; };

    while( !B.empty() ){

        if( neighCnt[ *min_element( ALL(B), comp ) ] == (int)B.size()-1 ){
//            cerr << "B: {"; for( int b : B ) cerr << g.nodes[b] << ", "; cerr << "} already fills the clique" << endl;
            A.insert(A.end(), ALL(B));
            break;
        }

        auto it = max_element( ALL(B), comp );
        int x = *it;

//        DEBUG(g.nodes[x]);
//        for( int i=0; i<neighCnt.size(); i++ ) cerr << "node " << g.nodes[i] << " has " << neighCnt[i] << " neighbors in B" << endl;
//        for( int i : B ) cerr << "node " << g.nodes[i] << " has " << neighCnt[i] << " neighbors in B" << endl;
//        ENDL(1);

        A.push_back(x);
        inB[x] = false;
        inA[x] = true;

        for( int d : g.V[x] ) Nx[d] = true;
        for( int i=(int)B.size()-1; i>=0; i-- ){
            int b = B[i];
            if( !Nx[b] ){
                swap( B[i], B.back() );
                B.pop_back();
                inB[b] = false;

                for( int d : g.V[b] ) neighCnt[d]--;

            }else{
                neighCnt[b]--;
            }
        }
        for( int d : g.V[x] ) Nx[d] = false;
    }

    for(int& a : A) a = g.nodes[a];

//    DEBUG(A);
    assert(CliqueUtils::isClique(V,A));

    return A;
}



void CliqueExtension::test(){
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);

    VI clq = { 11 };

    DEBUG( maximizeCliqueGreedy(V,clq) );

    exit(1);

}

VI CliqueExtension::findMaximalNodeCliqueExtension(VVI &V) {
    VI clq, bestClq;
    for(int i=0; i<V.size(); i++){

        if( !V[i].empty() ) clq = maximizeCliqueGreedy(V, {i} );
        else clq = {i};

        if( clq.size() > bestClq.size() ) bestClq = clq;
    }
    return bestClq;
}


