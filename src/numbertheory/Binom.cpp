//
// Created by sylwester on 10/3/19.
//

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