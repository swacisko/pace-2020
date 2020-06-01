//
// Created by sylwester on 8/14/19.
//

#include <numbertheory/NumberTheoryUtils.h>

#include "numbertheory/NumberTheoryUtils.h"


namespace NumberTheoryUtils{

    LL expMod(long long a, long long p, long long mod){
        if( p == 0 ) return 1;
        else if( p == 1 ) return ( a >= mod ? a % mod : a );
        else{
            __int128 res = 0;
            if( p&1 ){
                res = a;
                res *= expMod(a, p - 1, mod);
                if( res >= mod ) res %= mod;
            }else{
                long long temp = expMod(a, p >> 1, mod);
                res = temp;
                res *= temp;
                if( res >= mod ) res %= mod;
            }


            return (LL)res;
        }
    }



    int extendedEuclidean(LL a, LL b, LL &l, LL &k){
        if(!a){
            l = 0;
            k = 1;
            return b;
        }
        LL d = extendedEuclidean(b % a, a, k, l);

        l -= ( b/a )*k;
        return d;
    }


    int modularInverse(LL a, LL m){
        LL x,y;
        if(extendedEuclidean((LL)a, (LL)m, x, y) != 1 ) return -1;
        x %= m;
        if( x<0 ) x+=m;
        return x;
    }

    VLL modularInverseArray(int n, LL P) {
        VLL inv(n+1,-1);
        for(int i = 2; i < n; ++i) inv[i] = (n - (n/i) * inv[n%i] % n) % n;
        return inv;
    }


    void test() {

        int a = 42, b = 70;
        LL l,k;

        int gcdw = extendedEuclidean((LL)a, (LL)b, l, k);
        DEBUG(gcdw);
        DEBUG(a);
        DEBUG(b);
        DEBUG(l);
        DEBUG(k);
        DEBUG( a*l+b*k );


        int revmod42_71 = modularInverse(42, 71);
        DEBUG(revmod42_71);

        exit(1);
    }

    VLL convertToBase(LL N, LL B) {
        VLL res;
        while( N > 0 ){
            res.push_back( N % B );
            N /= B;
        }
        return res;
    }


    LL convertToDecimalLL(VLL V, LL B) {
        LL res = 0;
        for( int i=V.size()-1; i>=0; i-- ){
            res *= B;
            res += V[i];
        }
        return res;

    }




}