/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <graphs/scc/StronglyConnectedComponents.h>
#include <Constants.h>

#include "graphs/scc/StronglyConnectedComponents.h"

StronglyConnectedComponents::StronglyConnectedComponents(VVI &structure) : V(structure) {
    N = V.size();
}

void StronglyConnectedComponents::createStronglyConnectedComponents(){
    was = VB(N,0);
    compParent = VI(N,Constants::INF);
    PostOrder = VI(N);
    int ponum = 0;
    REP( i,N ) if( !was[i] ) PO_DFS( i,ponum ); // tutaj wyznaczam dla wszystkich wierzcholkow numery PostOrder

    VI sorted( N ); // tablica sorted[i] oznacza numer wierzcholka o numerze post_order rownym i
    REP( i,N ) sorted[ PostOrder[i] ] = i;

    V = transpose( V ); // odwracam graf aby wykonac na nim odpowiednie dfsy

    was = VB(N,0);
    FORD( i,N-1,0 ){ // zaczynjac od wierzcholka o najwiekszym numerze PostOrder w odwroconym grafie
        temp_comp.clear();
        if( !was[ sorted[i] ] ) Add_PO_DFS( sorted[i] ); // jesli jeszcze wierzcholek o numerze sorted[i] nie nalezy do spojen skladowej to go dodajemy wraz z cala silnie spojna skladowa
        if( SIZE(temp_comp) ) Comps.PB( temp_comp );
    }

    V = transpose( V ); // odwracam graf, aby byl w pozycji poczatkowej

//    cerr << "creating compParent" << endl;

    for( int i=0; i<Comps.size(); i++ ){
        for(int d : Comps[i]){
            compParent[d] = i;
        }
    }
}


void StronglyConnectedComponents::PO_DFS( int num, int & ponum ){ // funkcja wyznaczajaca numery Post_Order
    was[ num ] = true;
    REP( i,SIZE(V[num]) ) if( !was[ V[num][i] ] ) PO_DFS( V[num][i], ponum );
    PostOrder[ num ] = ponum++; // tutaj zapisuje numer PostOrder i zwiekszam ponum. Jest on przesylany przez referencje,
    //wiec w innych rekurencjach tez bedzie zwiekszony
}

void StronglyConnectedComponents::Add_PO_DFS( int num ){ // Funkcja dodajaca do tymczasowej silnie spojnej skladowej kolejne weirzcholki
    was[num] = true;
    temp_comp.PB( num );
    REP(i, SIZE(V[num]) ) if( !was[ V[num][i] ] ) Add_PO_DFS( V[num][i] );
}

VVI StronglyConnectedComponents::transpose( VVI & v ){ // funkcja zwraca graf transponowany
    VVI g( SIZE(v) );
    REP( i,SIZE(v) )	REP( k,SIZE(v[i]) )	g[ v[i][k] ].PB(i);
    return g;
}

VVI StronglyConnectedComponents::getStronglyConnectedComponentGraph() {
    if( Comps.empty() ) createStronglyConnectedComponents();

//    cerr << "Comps: " << Comps << endl;

    VVI sccGraph( Comps.size() );

    for( int i=0; i<Comps.size(); i++ ){
        unordered_set<int> neighbors;
        for( int p : Comps[i] ){
            for( int d : V[p] ){
                if( compParent[d] != i ) neighbors.insert( compParent[d] );
            }
        }

        sccGraph[i] = VI( ALL(neighbors) );
    }

    return sccGraph;
}

const VI &StronglyConnectedComponents::getCompParent() const {
    return compParent;
}
