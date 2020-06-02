/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <graphs/toposort/TopoSort.h>
#include <Constants.h>

#include "graphs/toposort/TopoSort.h"

TopoSort::TopoSort(VVI &structure) {
    V = structure;
    N = V.size();
}


void TopoSort::DFS( int num ){
    //	if( deg[num] ) return;
    kol.PB( num );
    was[num] = 1;

    REP(i,SIZE( V[num] ) ){
        deg[ V[num][i] ]--;
        if( !deg[V[num][i]] ) DFS( V[num][i] );
    }
}

void TopoSort::topoSort(){

    // this here is to ensure that we get 'relatively random' topological sort
    VI perm(V.size());
    iota(ALL(perm),0);
    random_shuffle(ALL(perm));
    for( VI & v : V ) random_shuffle(ALL(v));



    REP(i,SIZE(V)){
        int d = perm[i];
        if( !deg[d] && !was[d] ) DFS(d); // jesli juz nie ma ojscow  i jeszcze nie bylem w danym wierzcholku, to dodaje go do posortowanych
    }
}

VI TopoSort::sortTopologically(){
//    cerr << "in toposort, V:" << endl << V << endl;
    was = VB( SIZE(V),false );
    deg = VI( SIZE(V) );
    REP(i,SIZE(V)) REP(k,SIZE(V[i])) deg[ V[i][k] ]++;
    topoSort();

    return kol;
}

VI TopoSort::getLongestPath(){
    sortTopologically();
    VI maxy(N,0); // maxy[i] -> dlugosc najdluzszej sciezki zaczynjacej sie w wierzcholku i
    VI ind(N,Constants::INF); // ind[i] -> indeks syna na najdluzszej sciezce zaczynajacej sie w i

    FORD( i,SIZE(kol)-1,0 ){
        REP( k, SIZE( V[kol[i]] ) ){
            if( maxy[ kol[i] ] < maxy[ V[kol[i]][k] ] + 1 ){ // jesli zwieksza sie dlugosc sciezki, to przypisuje nowa makszymalna dlugosc i nowy index
                maxy[kol[i]] = maxy[ V[kol[i]][k] ] + 1;
                ind[ kol[i] ] = V[kol[i]][k];
            }
        }
    }

    int l_max = -1, ind_max = -1; // tutaj znajduje index wierzcholka bedacego poczatkiem najdluzszej sciezki
    REP(i,N){
        if( maxy[i] > l_max ){
            l_max = maxy[i];
            ind_max = i;
        }
    }

    VI l_path( 1,ind_max ); // tutaj bedzie sie znajdowala indeksami najdluzsza sciezka
    while( (ind_max = ind[ ind_max ]) < Constants::INF )	l_path.PB( ind_max );
    return l_path;
}

