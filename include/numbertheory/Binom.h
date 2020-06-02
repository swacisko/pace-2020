/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/
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
