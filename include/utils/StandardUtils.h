//
// Created by sylwester on 9/6/19.
//

#ifndef ALGORITHMSPROJECT_STANDARDUTILS_H
#define ALGORITHMSPROJECT_STANDARDUTILS_H

#include "IntGenerator.h"
#include "Makros.h"
#include "RandomNumberGenerators.h"

namespace StandardUtils{

    /**
     * This function returns smallest integer d int [p,q] such that fun returns true for d as argument or -1 if such d
     * does not exist. In other words if the virtual table has form {0,0,0,0,0,0,0,0,1,1,1,1,1,1} binary search will
     * return index of the first true element
     */
    extern int binarySearch( int p, int q, function<bool (int)> objComp );

    /**
     * Removes an array that is obtained by removing from A all elements that are in B. Elements in A that are not
     * removed preserve their relative order. A helper array of size max(A) is required.
     * Elements in B may be in any order
     */
    extern VI removeFromArrayPreserveOrder(VI & A, VI & B, VB & helper );

    /**
     * Removes all elements from [A] that are equal to [val].
     * Does NOT preserve the order of elements in [A]
     * @tparam _T
     */
    template<class _T>
    void removeAll( vector<_T> & A, _T val ) {
        for( int i=(int)A.size()-1; i>=0; i-- ){
            if( A[i] == val ){
                swap(A[i], A.back());
                A.pop_back();
            }
        }
    }

    /**
     * The same as [removeFromArrayPreserveOrder], but removes the elements in-place, without creating new array for
     * result.
     */
    extern void removeFromArrayPreserveOrderInplace(VI & A, VI & B, VB & helper );

    /**
     * Removes an array that is obtained by removing from A all elements that are in B. Elements in A that are not
     * removed NEED NOT preserve their relative order.
     * Works in time O( AlogA + BlogB ) - sorts ranges then takes the difference.
     */
    extern void removeFromArrayInplace(VI & A, VI B );

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
     * Given a list partition. creates layers such that res[partition[i]] contains i.
     * @param partition
     * @return
     */
    extern VVI partitionToLayers( VI & partition );

    /**
     * Given layers, creates a prtition such that i is in layer partition[i].
     */
    extern VI layersToPartition( VVI & layers );

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
    _T binaryPower( _T x, int a, function< _T(_T,_T) > multiply = [](_T x, _T y ){ return x*y; },
                    function< _T(_T) > square = [](_T x){ return x*x; } ){
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
    void writeInBinary( _T x, int bits = -1 ) {
        int i = 0;
        while ((1ull << (i + 1)) <= x && i+1 < bits) i++;

        for (int j = 0; j < min(bits, bits - i - 1); j++) clog << 0;
        while (i >= 0) {
            clog << __builtin_popcountll((x & (1ull << i)));
            i--;
        }
        clog << endl;
    }

    /**
     * Creates and returns a string with binary representation of $M$-bit integer [val].
     * If [M] is -1, then returned string will start with the most significant bit.
     */
    string toBinary( LL val, int bits = -1 );

    /**
     * Creates and returns a subarray of array [s] with beginning in index beg, and end in index end
     */
    template<class _T>
    vector<_T> getSubarray(vector<_T> s, int beg, int end){
        vector<_T> res;
        while(beg <= end){
            res.push_back(s[beg]);
            beg++;
        }
        return res;
    }


    /**
     * Implementation of function similar to that of python split.
     * Trims only spaces from the end of the string, no other white characters are removed.
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

    /**
     * Equivalent to python slicing. Extracts and returns in a vector all elements from [a,b) that are at positions
     * a, a+step, a+2*step, ...
     * It is also possible to pass a > b and step < 0
     */
    template<class _T>
    vector<_T> slice( vector<_T> & v, int a, int b, int step=1 ){
        assert(step != 0);
        vector<_T> res;
        res.reserve( abs( (b-a) / step ) );
        if(a<=b){
            assert(step > 0);
            while(a<b){
                res.push_back( v[a] );
                a += step;
            }
        }
        else{
            assert(step < 0);
            while(a>b){
                res.push_back(v[a]);
                a += step;
            }
        }
        return res;
    }


    // template<class _T, class _rnd>
    // void shuffle( vector<_T> & V, _rnd rnd ){
    //     std::uniform_int_distribution<long long> unif( 0, 10ll * V.size() );
    //
    //     for( int i=(int)V.size()-1; i>=0; i-- ){
    //         int ind = unif(rnd) % (i+1);
    //         if( ind != i ) swap( V[i], V[ind] );
    //     }
    // }
    //
    // template<class _T>
    // void shuffle( vector<_T> & V ){
    //     // UniformIntGenerator rnd(0,1e9);
    //     IntGenerator rnd;
    //     shuffle(V, rnd);
    // }

    template<class _T>
    void shuffle( vector<_T> & V, IntGenerator & rnd ){
        for( int i=(int)V.size()-1; i>=0; i-- ){
            int ind = rnd.nextInt(i+1);
            if( ind != i ) swap( V[i], V[ind] );
        }
    }

    /**
     * Creates a vector of pairs from two vectors. E.g. from [0,4,7], ['a', 's'] it will make [ {0,'a'}, {4,'s'} ]
     */
    template<class _t, class _s>
    vector< pair<_t,_s> > zip( vector<_t> a, vector<_s> b ){
        vector<pair<_t,_s>> res(min(a.size(), b.size()));
        for( int i=0; i<min(a.size(), b.size()); i++ ) res[i] = {a[i], b[i]};
        return res;
    }

    /**
     * Reverse to the [zip] function - creates two vectors from a vector of pairs.
     */
    template<class _t, class _s>
    pair<vector<_t>, vector<_s> > unzip( vector< pair<_t,_s> > &&a ){
        pair<vector<_t>, vector<_s> > res;
        res.first.reserve(a.size());
        res.second.reserve(a.size());
        for( int i=0; i<a.size(); i++ ){
            res.first.push_back(a[i].first);
            res.second.push_back(a[i].second);
        }
        return res;
    }

    /**
     * Creates all possible pairs (x,y) with x \in a and y \in b.
     */
    template<class T>
    vector<pair<T,T>> product( vector<T> a, vector<T> b ){
        vector<pair<T,T>> res;
        res.reserve(a.size() * b.size());

        for( int i=0; i<a.size(); i++ ){
            for(int j=0; j<b.size(); j++){
                res.emplace_back( a[i], b[j] );
            }
        }
        return res;
    }

    /**
     * Does the product if the predicate [pred] returns true.
     * [pred] must be callable pred(_T,_T) and return a boolean value
     */
    template<class T, class _P>
    vector<pair<T,T>> productIf( vector<T> a, vector<T> b, _P pred ){
        vector<pair<T,T>> res;
        res.reserve(a.size() * b.size());

        for( int i=0; i<a.size(); i++ ){
            for(int j=0; j<b.size(); j++){
                if( pred(a[i], b[j]) ) res.emplace_back( a[i], b[j] );
            }
        }
        return res;
    }


    /**
     * Makes the standard unique operations:
     *      sort(ALL(v));
     *      v.resize( unique(ALL(v)) - v.begin() );
     */
    template<class _T>
    void makeUnique( vector<_T> & v ){
        sort(ALL(v));
        v.resize( unique(ALL(v)) - v.begin() );
    }


    /**
     * Checks if vector [v] contains element [el].
     */
    template<class _T>
    bool find( vector<_T> & v, _T el ){ return find(ALL(v),el) != v.end(); }

    /**
     * Finds and returns the union of given two collections containing bounded integers
     * @param helper array neccessary to index all elements in
     * @return
     */
    template<class _T, class _S>
    VI setUnion( _T A, _S B, VB & helper ){
        VI res = A;
        for( auto a : A ) helper[a] = true;
        for( auto b : B ) if(!helper[b]){ helper[b] = true; res.push_back(b); }
        for( auto a : A ) helper[a] = false; for( auto a : B ) helper[a] = false;
        return res;
    }

    /**
     * Adds to [A] (using push_back) all elements from [B] that are not marked in [helper].
     * Updates [helper] for all added elements.
     */
    template<class _T, class _S>
    void setUnionAdd( _T &A, _S &B, VB & helper ){
        for( auto b : B ) if(!helper[b]){ helper[b] = true; A.push_back(b); }
    }

    /**
     * Finds and returns the intersection of given two collections containing bounded integers
     * @param helper array neccessary to index all elements in
     * @return
     */
    template<class _T, class _S>
    VI setIntersection( _T A, _S B, VB & helper ){
        VI res;
        for( auto a : A ) helper[a] = true;
        for( auto b : B ) if(helper[b]){ helper[b] = false; res.push_back(b); }
        for( auto a : A ) helper[a] = false; for( auto a : B ) helper[a] = false;
        return res;
    }

    /**
    * Finds and returns the difference A \ B of given two collections containing bounded integers
    * @param helper array neccessary to index all elements in
    * @return
    */
    template<class _T, class _S>
    VI setDifference( _T A, _S B, VB & helper ){
        VI res;
        for( auto b : B ) helper[b] = true;
        for( auto a : A ) if(!helper[a]){ helper[a] = true; res.push_back(a); }
        for( auto a : A ) helper[a] = false; for( auto a : B ) helper[a] = false;
        return res;
    }

    /**
     * Function remaps given array [A] to integers, so that the lexicographic order is preserved.
     * E.g. for array [0,50,22,7], it will remap that array to [0,3,2,1].
     * Function will also create and return map with  A->res mapping  (from example 50 -> 3, 22 -> 2, 7->1, 0->0),
     * and a remapper vector (from example 0->0, 1->7, 2->22, 3->50)
     */
    template<class _T>
    tuple< VI, unordered_map<_T,int>, vector<_T> > remapToIntPreserveLexicographicOrder( vector<_T> A ){
        set<_T> zb(ALL(A));
        unordered_map<_T,int> mapper;

        int cnt = 0;
        for( _T d : zb ) if(mapper.count(d) == 0) mapper[d] = cnt++;

        vector<_T> remapper(cnt);
        for( auto & [k,v] : mapper ) remapper[v] = k;

        VI tab(A.size());
        for(int i=0; i<A.size(); i++) tab[i] = mapper[A[i]];
        return make_tuple( tab, mapper, remapper );
    }

    /**
     * Creates a random string of length [length] with lower case english letters only.
     */
    string getRandomString(int length);

    /**
     * Function creates a representative of given integer in the system where digits are in the set
     * [possible_characters].
     *
     * To do so, let S = possible_characters.size().
     * Then we add to created string possible_characters[ val%S ], divide val/S and repeat until val = 0.
     *
     */
    string convertIntegerToString( LL val, vector<char> &possible_characters);

    /**
     * We will create a string from [val] using [convertIntegerToString].
     * If use_small_letters is true, then small letter will be used. The same for use_capital_letters and
     * use_digits.
     * At least one of the options need to be specified.
     */
    string convertIntegerToString( LL val, bool use_small_letters, bool use_capital_letters, bool use_digits);

    /**
     * For each integer value in [collection], transforms it to a string using [convertIntegerToString], then returns
     * accumulated value.
     * @param collection collection containing integer values.
     */
    template<class _T>
    string convertArrayToString( _T collection, bool use_small_letters, bool use_capital_letters, bool use_digits ){
        string res = "";
        for( auto & a : collection ) res += convertIntegerToString(a, use_small_letters, use_capital_letters, use_digits);
        return res;
    }

}

#endif //ALGORITHMSPROJECT_STANDARDUTILS_H
