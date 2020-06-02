/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_COMBINATORICUTILS_H
#define ALGORITHMSPROJECT_COMBINATORICUTILS_H

#include <utils/RandomNumberGenerators.h>
#include "Makros.h"

namespace CombinatoricUtils{

    /**
     *
     * @param N size of permutation
     * @return random permutation of {0, ... ,N-1}
     */
    extern VI getRandomPermutation( int N );

    /**
     *
     * @param N
     * @param A
     * @return set difference  {0, ... , N-1} \ A
     */
    extern VI getFullSetDifference( int N, VI  A );

    /**
     *
     * @param U
     * @param N
     * @return random N-element sequence with elements from universe {0, ... , U}
     */
    extern VI getRandomSequence( int U, int N );

    /**
     *
     * @param U
     * @param L
     * @return random subset of elements from universe {0,...,U}
     */
    extern VI getRandomSubset( int U, int L );


    /**
     *
     * @tparam _T
     * @param universe
     * @param L
     * @return random sequence of length @{L} with elemenents from given @{universe}
     */
    template<class _T>
    vector<_T> getRandomSequence( vector<_T> & universe, int L ){
        UniformIntGenerator gen(0,universe.size()-1);
        vector<_T> res;
        res.reserve( L );
        for( int i=0; i<L; i++ ) res.push_back( universe[ gen.rand() ] );
        return res;
    }




    /**
     * Function iterates over all partitions of the form (A_1, A_2, ... , A_t) where t is the size of @{sets} - it will be 0 <= A_i < sets[i].
     * Whenever an element d from set at index ind is added or removed to currently created sequence, we call function fun(seq, ind, s, added), where added is true,
     * if an elements was added and false if it was removed, and s is the index of element hosen from A[ind]. This enables us to quickly update
     * data. (e.g. when we calculate all possible products, we modif current product after an element is added or removed).
     * @param sets
     * @param fun
     */
    void allPartitions( VI & sets, function< void( VI&, int ind, int d, bool added ) > fun );


}


#endif //ALGORITHMSPROJECT_COMBINATORICUTILS_H
