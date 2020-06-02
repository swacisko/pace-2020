/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_STANDARDUTILS_H
#define ALGORITHMSPROJECT_STANDARDUTILS_H

#include "Makros.h"

namespace StandardUtils{

    /**
     * this function returns smallest integer d int [p,q] such that fun returns true for d as argument or -1 if such d does not exist.
     * in other words if the table has form {0,0,0,0,0,0,0,0,1,1,1,1,1,1} bijary search will return index of the first true element
     */
    extern int binarySearch( int p, int q, function<bool (int)> objComp );


    /**
     *
     * @param tab
     * @return vector containing indices for which tab[i] is true
     */
    extern VI extractIndices(VB & tab);


    extern VI toVI(VB& v);

    /**
     *
     * @param N
     * @param v
     * @return VB of soze N in which v[i] is set for all i in {0,v.size()-1}
     */
    extern VB toVB(int N, VI& v);

    /**
     * Binary power x^a.
     * @tparam _T
     * @param x
     * @param a this need to be at least 1
     * @param multiply this function is used to multiply two objects
     * @param square this function is used to square two objects. By default it is multiplication, but in some cases squaring may be faster than usual product.
     * @return
     */
    template<class _T>
    _T binaryPower( _T x, int a, function< _T(_T,_T) > multiply, function< _T(_T) > square = [](_T x){ return x*x; } ){
        if( a == 1 ) return x;
        else if( a & 1 ){
            _T t = binaryPower(x, (a-1)>>1, multiply, square );
            return multiply( x, square(t) );
        }else{
            _T t = binaryPower(x, a>>1, multiply, square );
            return square(t);
        }
    }

    /**
     * Writes given integer number in binary format (right bit is least significant).
     * @tparam _T
     * @param x
     */
    template<class _T>
    void writeInBinary( _T x, int bits = -1 ){
        int i = 0;
        while( (1<<(i+1)) <= x ) i++;

        for( int j=0; j<min(bits,bits - i-1); j++ ) cerr << 0;
        while( i >= 0 ){
            cerr << __builtin_popcount( (x & (1<<i)) );
            i--;
        }
        cerr << endl;
    }

    /**
     * Creates and returns a subarray of array @{s} with beginning in index beg, and end in index end-1
     * @tparam _T
     * @param s
     * @param beg
     * @param end
     * @return
     */
    template<class _T>
    vector<_T> getSubarray(vector<_T> s, int beg, int end){
        vector<_T> res;
        while(beg < end){
            res.push_back(s[beg]);
            beg++;
        }
        return res;
    }

    /**
     * Implementation of function similar to that of python split
     * @param s
     * @param pat
     * @return
     */
    vector<string> split( string & s, string pat = " " );

    /**
     * The same as v1.insert( v1.end(), ALL(v2) );
     * @tparam _T
     * @param v1
     * @param v2
     */
    template<class _T>
    void append( vector<_T> & v1, vector<_T> & v2 ){ v1.insert( v1.end(), ALL(v2) ); }


}

#endif //ALGORITHMSPROJECT_STANDARDUTILS_H
