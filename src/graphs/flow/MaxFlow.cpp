//
// Created by sylwester on 8/12/19.
//

#include <graphs/flow/MaxFlow.h>

#include "graphs/flow/MaxFlow.h"
#include "graphs/GraphUtils.h"


MaxFlow::MaxFlow(VVI &structure, MaxFlow::VVWT &weights) {
    V = structure;
    W = weights;

    makeBdirectionalStructure();
}

void MaxFlow::makeBdirectionalStructure() {
    auto rev = transpose(V);
    for(int i=0; i<V.size(); i++){
        V[i].insert( V[i].end(), ALL(rev[i]) );
        sort(ALL(V[i]));
        V[i].resize( unique(ALL(V[i])) - V[i].begin() );
    }
}


/**
 * Function
 * @param beg source of the flow
 * @param end the end node of the flow
 * @return value of max flow from the source, to the end
 */
MaxFlow::wtype MaxFlow::calculateMaxFlow(int beg, int end, MaxFlow::wtype maxFlowBound) {
//    kraw = VVWT( SIZE(W), VWT( SIZE(W),0 ) );
    kraw = VVWT( SIZE(W) ); // unordered_map (memory efficient) version.

    wtype ile = 0;
    MAX_FLOW = 0;

    int cnt = 1;

//    if( V.size() == 20 ){
//        DEBUG(beg);
//        DEBUG(end);
//        GraphUtils::writeGraphHumanReadable(V);
//        GraphUtils::writeGraphHumanReadable(V2);
//        DEBUG(kraw);
//        DEBUG(W);
//        ENDL(2);
//    }

    while( LayerBFS( beg,end ) ){


        canReach.clear();
        canReach = VB( SIZE(V), true );
        blockFlowDfs( beg,end,Constants::INF,ile );

//        if( V.size() == 20 ){
//            cerr << "max flow iteration #" << cnt++ << endl;
//            GraphUtils::writeGraphHumanReadable(V);
//            GraphUtils::writeGraphHumanReadable(V2);
//            DEBUG(kraw);
//            DEBUG(W);
//            ENDL(2);
//        }

        if( maxFlowBound != -1 && MAX_FLOW >= maxFlowBound ) break;

    }
    return MAX_FLOW;
}




VVI MaxFlow::transpose( VVI & v ){ // funkcja zwraca graf transponowany
    VVI g( SIZE(v) );
    REP( i,SIZE(v) )	REP( k,SIZE(v[i]) )	g[ v[i][k] ].PB(i);
    return g;
}


bool MaxFlow::LayerBFS( int beg, int end ){
    VI dist(V.size(),Constants::INF); // odleglosc od zrodla przy tworzeniu garfu warstwowego
    dist[beg] = 0;

    V2 = VVI( V.size() ); // to jest potrzebne do stworzenia grafu warstwowego

    VI neigh( 1,beg ); // vector dla BFSa - kolejne wierzcholki

    bool found = false;
    for(int i=0; i < SIZE(neigh); i++){
        int p = neigh[i];

        if( found && dist[ p ] > dist[ neigh[i-1] ] ) break; // uwaga!! nie powinno sie zdarzyc, ze odleglosc bedzie rowna 0, ale moze... jesli beg == end to SIGSEGV wyleci

        for(int k=0; k < SIZE( V[p] ); k++){
            int d = V[p][k];

            if( d == end && W[p][d] > 0 ) found = true;
            if( dist[d] == Constants::INF && W[p][d] > 0 ){ // jesli jeszcze nie bylem w tym wierzcholku
                dist[d] = dist[ p ] + 1;
                V2[p].push_back( d ); // to jest graf skierowany, weic tylko jedna krawedz dodaje
                neigh.push_back( d );
            }
            else if( dist[ d ] == dist[ p ] + 1 && W[p][d] > 0 ){
                V2[p].push_back(d);
            }
        }
    }

    if( !found ) return false; // jesli nie ma sciezki, to zzwroc falsz
    return true;
}


bool MaxFlow::blockFlowDfs( int num, int end, wtype minC, wtype & ile ){
    if( num == end ){ ile = Constants::INF; MAX_FLOW += minC; return true;	}
    if( !canReach[num] ) return false;
    wtype suma = 0,t;

    for(int i=0; i< SIZE( V2[num] ); i++ ){
        int d = V2[num][i];
        t = min( minC - suma, W[num][ d ] );

        if( blockFlowDfs( d,end, t, ile ) ){
            t = min( t,ile ); // zawsze ile < t poza przypadkiem, gdy V2[num][i] == end
            kraw[num][ d ] += t;
            kraw[d][num] -= t;
            suma += t;
            W[num][ d ] -= t;
            W[ d ][num] += t;
            ile = suma;
            if( suma == minC ) return true;
        }
    }
    canReach[num] = false;
    return ( ile = suma ) > 0;
}

MaxFlow::wtype MaxFlow::getMAX_FLOW() const {
    return MAX_FLOW;
}

