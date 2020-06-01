//
// Created by sylwester on 3/16/20.
//

#include <CONTESTS/PACE20/depthtreecreators/DepthTreeCreator.h>

#include "CONTESTS/PACE20/depthtreecreators/DepthTreeCreator.h"

LL binom(int N, int K){
    LL res = 1;
    if( K > N-K ) K = N-K;
    for( LL i = 1; i<=K; i++ ){
        res *= (N-K+i);
        res /= i;
    }
    return res;
}

VVI DepthTreeCreator::getRandomSources( VI quantities ) {
    VVI res;

    set<VI> sets;

    int N = V->size();
    LL maxSets = 0;
    for(int i=1; i<quantities.size(); i++){
        if( maxSets < Constants::INF ){
            maxSets = binom(N,i);
            maxSets = min( 1ll*Constants::INF, maxSets );
        }
        quantities[i] = min( quantities[i], (int)maxSets );
    }

    for(int q=1; q<quantities.size(); q++){

        for( int i=0; i<quantities[q]; i++ ){
            VI sources = getRandomSources(q);
            sort(ALL(sources));
            if( sets.count( sources ) ){
                i--;
                continue;
            }

            sets.insert(sources);
            res.push_back(sources);
        }
    }

    return res;
}

