//
// Created by sylwester on 8/7/19.
//

#ifndef ALGORITHMSPROJECT_MAKROS_H
#define ALGORITHMSPROJECT_MAKROS_H



#include<cstdio>
#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<complex>
#include<stack>
#include<list>
#include<bitset>
#include<set>
#include<unordered_set>
#include<unordered_map>
#include<iterator>
#include<cmath>
#include<queue>
#include<ctime>
#include<string.h>
#include<fstream>
#include<sstream>
#include<algorithm>
#include <numeric>
#include<chrono>
#include<random>
#include<functional>
#include<utility>
#include <assert.h>
//#include<conio.h>

using namespace std;

#define REP( x,y ) for( int x=0; x<(y); ++x )
#define FORD( x,y,z ) for( int x=y; x>=(z); --x )
#define FOR(x,b,e) for( int x = b; x <= (e); ++x )
#define SIZE(v) (int)v.size()
#define ALL(c) c.begin(),c.end()
#define VAR(v,n) __typeof(n) v=(n)
#define FOREACH(i,c) for( VAR(i,c.begin());i!=c.end();++i )
#define PB push_back
#define MP make_pair
#define ST first
#define ND second
#define WRITE( V ){ FOREACH(it,V) cerr << *it << ", "; cerr << endl; }

#define DEBUG_NOW_AND_HERE true
#define DEBUG(x) if( DEBUG_NOW_AND_HERE ) cerr << #x << ": " << x << endl;

#define ERROR( x,s ) {  cerr << s << endl; DEBUG(x); exit(1); }

#define WRP(p) "(" << p.ST << "," << p.ND << ")"
#define WRITE_PAIRS { FOREACH(it,V) cerr << WRP(*it) << ", "; cerr << endl; }
#define WRITE_ALL(V,s,t) { cerr << s << endl;  REP( i,SIZE(V) ){ cerr  << i+t << " ---- ";  FOREACH(it,V[i]) cerr << *it+t << ", "; cerr << endl;     } }
#define CLEAR( dst,quant ) memset( dst,0, (quant)*sizeof( __typeof(*dst) ) );
#define ENDL(x) REP(crow,(x)) cerr << endl;




typedef long long LL;

typedef vector<int> VI;
typedef vector< VI > VVI;
typedef vector<VVI> VVVI;

typedef vector<double> VD;
typedef vector< VD > VVD;

typedef vector<bool> VB;
typedef vector< VB > VVB;

typedef pair<int,int> PII;
typedef vector<PII> VPII;
typedef vector<VPII> VVPII;

typedef vector<LL> VLL;
typedef vector<VLL> VVLL;

typedef pair<LL,LL> PLL;
typedef vector<PLL> VPLL;





template<class _T, class _E>
ostream& operator<<( ostream& str, const pair<_T,_E> & pair){
    str << "(" << pair.first << "," << pair.second << ")";
    return str;
}

template<class _T>
ostream& operator<<( ostream& str, const vector<_T> & vec ){
    int ile = 0;
    str << "{";
    for( auto p : vec ){
        if(ile > 0) str << ", ";
        ile++;
        str << p;
    }
    str << "}";
    return str;
}

template<class _T>
ostream& operator<<( ostream& str, const deque<_T> & vec ){
    int ile = 0;
    str << "{";
    for( auto p : vec ){
        if(ile > 0) str << ", ";
        ile++;
        str << p;
    }
    str << "}";
    return str;
}


template<class _T>
ostream& operator<<( ostream& str, const set<_T> & vec ){
    int ile = 0;
    str << "{";
    for( auto p : vec ){
        if(ile > 0) str << ", ";
        ile++;
        str << p;
    }
    str << "}";
    return str;
}


template<class _T>
ostream& operator<<( ostream& str, const unordered_set<_T> & vec ){
    int ile = 0;
    str << "{";
    for( auto p : vec ){
        if(ile > 0) str << ", ";
        ile++;
        str << p;
    }
    str << "}";
    return str;
}

template<class _T, class _E>
ostream& operator<<( ostream& str, const map<_T, _E> & vec ){
    int ile = 0;
    str << "{";
    for( auto p : vec ){
        if(ile > 0) str << ", ";
        ile++;
        pair<_T,_E> P = {p.first, p.second};
        str << P;
    }
    str << "}";
    return str;
}

template<class _T, class _E>
ostream& operator<<( ostream& str, const unordered_map<_T, _E> & vec ){
    int ile = 0;
    str << "{";
    for( auto p : vec ){
        if(ile > 0) str << ", ";
        ile++;
        pair<_T,_E> P = {p.first, p.second};
        str << P;
    }
    str << "}";
    return str;
}




struct pairhash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
//        return std::hash<T>()(x.first) ^ (  std::hash<U>()(x.second) ^ (std::hash<T>( x.first )+1) ) ;
            return x.first ^ ( x.second + 171 );
    }
};






#endif //ALGORITHMSPROJECT_MAKROS_H
