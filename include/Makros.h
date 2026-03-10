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
#include <climits>

using namespace std;

#define REP( x,y ) for( int x=0; x<(y); ++x )
#define FORD( x,y,z ) for( int x=y; x>=(z); --x )
#define FOR(x,b,e) for( int x = b; x <= (e); ++x )
#define SIZE(v) (int)v.size()
#define ALL(c) c.begin(),c.end()
#define REM(v,j) {swap(v[j],v.back()); v.pop_back();}
#define REMCVAL(tab, v) {for(int __tmp = (int)v.size()-1; __tmp >=0; __tmp--) if(tab[v[__tmp]]) REM(v,__tmp)}
#define REMCIND(tab, v) {for(int __tmp = (int)v.size()-1; __tmp >=0; __tmp--) if(tab[__tmp]) REM(v,__tmp)}
#define REMCFUN(fun, v) {for(int __tmp = (int)v.size()-1; __tmp >=0; __tmp--) if(fun(__tmp)) REM(v,__tmp)}
#define SUMSIZES(tab) accumulate(ALL(tab), 0, [&](int s, auto & v){return s + v.size();})
#define VAR(v,n) __typeof(n) v=(n)
#define FOREACH(i,c) for( VAR(i,c.begin());i!=c.end();++i )
#define PB push_back
#define MP make_pair
#define ST first
#define ND second
#define WRITE( V ){ FOREACH(it,V) cerr << *it << ", "; cerr << endl; }

#define DEBUG_NOW_AND_HERE true
//#define DEBUG(x) if( DEBUG_NOW_AND_HERE ) cerr << #x << ": " << x << endl;
#define DEBUG(x) if( DEBUG_NOW_AND_HERE ) clog << #x << ": " << x << endl;
#define DEBUGV(v,ind) if( DEBUG_NOW_AND_HERE ) clog << #v << "[" << ind << "]: " << v[ind] << endl;
#define DEBUG2(x,y) if( DEBUG_NOW_AND_HERE ) { DEBUG(x); DEBUG(y); }
#define DEBUG3(x,y,z) if( DEBUG_NOW_AND_HERE ) { DEBUG(x); DEBUG(y); DEBUG(z); }

#define ASSERT( cond, msg ) { if(!(cond)){ DEBUG(msg); assert(cond); } }

#define ERROR( x,s ) {  cerr << s << endl; DEBUG(x); exit(1); }

#define WRP(p) "(" << p.ST << "," << p.ND << ")"
#define WRITE_PAIRS { FOREACH(it,V) cerr << WRP(*it) << ", "; cerr << endl; }
#define WRITE_ALL(V,s,t) { cerr << s << endl;  REP( i,SIZE(V) ){ cerr  << i+t << " ---- ";  FOREACH(it,V[i]) cerr << *it+t << ", "; cerr << endl;     } }
#define CLEAR( dst,quant ) memset( dst,0, (quant)*sizeof( __typeof(*dst) ) );
#define ENDL(x) REP(crow,(x)) clog << endl;
#define ENDLS(x,c) REP(crow,(x)) clog << c << flush;




typedef long long LL;
typedef unsigned long long ULL;

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
void writeCollectionToStream(ostream& str, _T& col );

// template<class _T>
// void writeCollectionToStream(ostream& str, _T& col ){
//     str << "{";
//     int ile = 0;
//     for(auto t : col){
//         if(ile++ > 0) str << ", ";
//         str << t;
//     }
//     str << "}";
// }

template<class _P, class _Q, class _R>
void writeCollectionToStream(ostream& str, tuple<_P,_Q,_R> col ){
    str << "{" << get<0>(col) << ", " << get<1>(col) << ", " << get<2>(col) << "}";
}


/**
 * Fibonacci hashing.
 * This should be used in unordered_map to avoid nasty collisions.
 */
struct fib_hash {
    static uint64_t splitmix64(uint64_t x)
    {

        // 0x9e3779b97f4a7c15,
        // 0xbf58476d1ce4e5b9,
        // 0x94d049bb133111eb are numbers
        // that are obtained by dividing
        // high powers of two with Phi
        // (1.6180..) In this way the
        // value of x is modified
        // to evenly distribute
        // keys in hash table
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    int operator()(uint64_t x) const
    {
        static const uint64_t random
//                = std::chrono::steady_clock::now().time_since_epoch().count();

                // some FIXED random number, makes unordered_map work deterministically
                = 9'501'928'476'029'387ll;

        // The above line generates a
        // random number using
        // high precision clock
        return splitmix64(
                // It returns final hash value
                x + random);
    }
};

struct quick_hash {
    static const uint64_t random = 9'501'928'476'029'387ll;

    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        return x ^ (x >> 31);
    }

    int operator()(uint64_t x) const {
        return splitmix64(x + random);
    }
};


template<class _T>
ostream& operator<<( ostream& str, vector<_T> v ){ writeCollectionToStream(str, v); return str; }

template<class _T>
ostream& operator<<( ostream& str, deque<_T> v ){ writeCollectionToStream(str, v); return str; }

template<class _P, class _Q, class _R>
ostream& operator<<( ostream& str, tuple<_P,_Q,_R> v ){ writeCollectionToStream(str, v); return str; }

template<class _T>
ostream& operator<<( ostream& str, set<_T> v ){ writeCollectionToStream(str, v); return str; }

template<class _T>
ostream& operator<<( ostream& str, unordered_set<_T> v ){ writeCollectionToStream(str, v); return str; }

template<class _T, class _H>
ostream& operator<<( ostream& str, unordered_set<_T, _H> v ){ writeCollectionToStream(str, v); return str; }

template<class _T, class _E>
ostream& operator<<( ostream& str, map<_T, _E> v ){ writeCollectionToStream(str, v); return str; }

template<class _T, class _E>
ostream& operator<<( ostream& str, unordered_map<_T, _E> v ){ writeCollectionToStream(str, v); return str; }

template<class _T, class _E, class _H>
ostream& operator<<( ostream& str, unordered_map<_T, _E, _H> v ){ writeCollectionToStream(str, v); return str; }

template<class _T>
void writeCollectionToStream(ostream& str, _T& col ){
    str << "{";
    int ile = 0;
    for(auto t : col){
        if(ile++ > 0) str << ", ";
        str << t;
    }
    str << "}";
}

struct pairhash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
            return x.first ^ ( x.second * 171'018'971 );
    }
};

template<class _T>
struct Triple{
    Triple(){}
    Triple(_T first, _T second, _T third){ st = first; nd = second; rd = third; }
    _T st,nd,rd;
    bool operator<(const Triple & oth) const {
        if( st != oth.st ) return st < oth.st;
//        if( nd != oth.nd ) return nd < oth.nd;
//        if( nd != oth.nd )
            return nd < oth.nd;
//        return rd < oth.rd;
    }
    string toString(){ stringstream str;str << "(" << st << "," << nd << "," << rd << ")";return str.str(); }
};

template<class _T>
struct Quadruple{
    Quadruple(){}
    Quadruple(_T first, _T second, _T third, _T foruth){  st = first; nd = second; rd = third; th = foruth; }
    _T st,nd,rd, th;
};




#endif //ALGORITHMSPROJECT_MAKROS_H
