/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include "datastructures/FAU.h"


FAU::FAU( int n ) : p(new int[n]), w(new int[n]) {
    for(int x=0; x<n; x++) p[x] = w[x] = -1;
    for(int x=0; x<n; x++) w[x] = 1;
}

FAU::~FAU(){
    delete[] p;
    delete[] w;
}

/**
 *
 * @param x
 * @return id of element representing set containing x
 */
int FAU::Find(int x){
    return ( p[x]<0 ) ? x : p[x] = Find( p[x] );
}

/**
 *
 * @param p
 * @return number of elements in the set containing p
 */
int FAU::countNodesInComponent( int p ){
    return w[ Find(p) ];
}

/**
 * Merges two sets: containing x and containing y
 * @param x
 * @param y
 */
void FAU::Union( int x, int y ){
    if( (x = Find(x)) == (y = Find(y)) ) return;
    if( w[x] > w[y] ){
        p[y] = x;
        w[x] += w[y];
    }
    else{
        p[x] = y;
        w[y] += w[x];
    }
}