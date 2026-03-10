//
// Created by sylwester on 1/3/21.
//

#ifndef ALGORITHMSPROJECT_COLLECTIONOPERATORS_H
#define ALGORITHMSPROJECT_COLLECTIONOPERATORS_H

#include "Makros.h"

template<class _T, class _E>
void operator+=(vector<_T>& v, _E w){
    v.insert( v.end(), ALL(w) );
}

template<class _T, class _E>
void operator+=(set<_T>& v, _E w){
    v.insert( ALL(w) );
}

template<class _T, class _E>
void operator+=(unordered_set<_T>& v, _E w){
    v.insert( ALL(w) );
}

/**
 * Creating sum of the collections, v+w
 */
template<class _T, class _E>
_T operator+(_T v, _E w){
    v += w;
    return v;
}

/**
 * Reversing the collection by calling reverse(ALL(v))
 */
template<class _T>
vector<_T> operator-(vector<_T> v){
    reverse(ALL(v));
    return v;
}

/**
 * Checking whether two vectors are identical
 */
template<class _T>
bool operator==( vector<_T> & x, vector<_T> & y ){
    return equal( ALL(x), ALL(y) );
}

#endif //ALGORITHMSPROJECT_COLLECTIONOPERATORS_H
