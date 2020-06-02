/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_NUMBERTHEORYUTILS_H
#define ALGORITHMSPROJECT_NUMBERTHEORYUTILS_H

#include "Makros.h"

namespace NumberTheoryUtils{


    /**
     *
     * @param a base of  power
     * @param p exponent of power
     * @param mod modulo
     * @return value of a^p modulo mod
     */
    extern LL expMod(long long a, long long p, long long mod);


    /**
     * Function finds greatest common divisor of integers @{a} and @{b}. Also calculates values @{l} and @{k} such that   a*l + b*k = l
     * @param a
     * @param b
     * @param l
     * @param k
     * @return graetest common divisor of a and b
     */
    extern int extendedEuclidean(LL a, LL b, LL &l, LL &k);

    /**
     * Function calculates modular inverse of a modulo m. It means that a * res == 1 (mod m)
     * @param a
     * @param m
     * @return
     */
    extern int modularInverse(LL a, LL m);

    /**
     * Calculates inverse mod P for all i <= n. P must be PRIME.
     * @param n
     * @return array of size n+1, with res[i] being a modular inverse of i mod P, or -1 if  it does not exist, where P is a PRIME number.
     */
    extern VLL modularInverseArray(int n, LL P);

    /**
     * Function creates and returns a vector of coefficients with property   N = res[0] + res[1] * B + res[2] * B^2 + ... + res.back() * B^(res.size()-1)
     * @param N
     * @return
     */
    VLL convertToBase( LL N, LL B );

    /**
     * Converts number given by coefficients in V n base B to decimal long long value.
     * @param V
     * @param B
     * @return
     */
    LL convertToDecimalLL(VLL V, LL B);




    extern void test();
}



#endif //ALGORITHMSPROJECT_NUMBERTHEORYUTILS_H
