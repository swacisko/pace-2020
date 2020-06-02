/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <numbertheory/NumberTheoryUtils.h>
#include <numbertheory/Binom.h>

namespace Binom{


    LL binom(int N, int K){
        LL res = 1;
        if( K > N-K ) K = N-K;
        for( LL i = 1; i<=K; i++ ){
            res *= (N-K+i);
            res /= i;
        }
        return res;
    }


}