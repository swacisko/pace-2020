//
// Created by sylwester on 8/27/19.
//

#include "combinatorics/CombinatoricUtils.h"
#include <utils/RandomNumberGenerators.h>
#include <combinatorics/CombinatoricUtils.h>


namespace CombinatoricUtils{


    VI getRandomPermutation(int N){
        VI perm(N);
        iota(ALL(perm),0);
        random_shuffle(ALL(perm));
        return perm;
    }



    VI getRandomSequence( int U, int N ) {
        VI seq(N);
        UniformIntGenerator g( 0,U );
        for(int i=0; i<N; i++) seq[i] = g.rand();
        return seq;
    }


    VI getRandomSubset( int U, int L ){
        UniformIntGenerator gen(0,U);

        VI res;
        if( L < U / 20 ){
            unordered_set<int> zb;
            while( zb.size() < L ) zb.insert( gen.rand() );
            res = VI(ALL(zb));
        }else{
            VI perm = getRandomPermutation(U+1);
            return VI( perm.begin(), perm.begin() + L );
        }
        return res;
    }


    VI getFullSetDifference(int N, VI A) {
        VI perm(N);
        iota(ALL(perm),0);
        VI res;
        VI A2 = A;
        sort( ALL(A2) );
        set_difference( ALL(perm), ALL(A2), back_inserter(res) );
        return res;
    }

    void allPartitions(VI &sets, function<void(VI &, int ind, int d, bool added)> fun) {

        VI seq;
        function< void( int ind ) > generator = [&generator, &sets, &fun, &seq]( int ind ){


                for( int i=0; i<sets[ind]; i++ ){
                    seq.push_back(i);
                    fun( seq, ind,i,true );

                    if( ind +1 < sets.size() ) generator(ind+1);

                    seq.pop_back();
                    fun( seq, ind,i,false );

                }
        };


        generator(0);
    }


}

