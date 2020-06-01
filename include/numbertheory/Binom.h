//
// Created by sylwester on 10/3/19.
//

#ifndef ALGORITHMSPROJECT_BINOM_H
#define ALGORITHMSPROJECT_BINOM_H

#include "Makros.h"
#include "Constants.h"


namespace Binom{

    /**
     * Calculates value of (N choose K). Works in O(K). Be carefull for precision - it grows really fast.
     * @param n
     * @param k
     * @return value of ( n choose k ) for integers n and k
     */
    LL binom(int N, int K);


}

#endif //ALGORITHMSPROJECT_BINOM_H
