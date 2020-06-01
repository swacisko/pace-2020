//
// Created by sylwester on 8/27/19.
//

#ifndef ALGORITHMSPROJECT_RANDOMGENERATORS_H
#define ALGORITHMSPROJECT_RANDOMGENERATORS_H

#include "Makros.h"


namespace RandomNumberGenerators{

    const int DEFAULT_SEED = 171;

    /**
     *
     * @param min
     * @param max
     * @param seed seed of the mersenne_twister_engine
     * @return a pair containing uniform distribution generator and mersenne_twister_engine. Called p.first( p.second ) returns random integer from closed interval [min,max] with uniform probability distribution
     */
    static pair< std::uniform_int_distribution<long long>, std::mt19937_64  > getRandomUniformIntegerGenerator( LL minVal, LL maxVal, int seed = DEFAULT_SEED ){
        std::mt19937_64 rng;
        rng.seed(seed);
        std::uniform_int_distribution<long long> unif(minVal, maxVal);
        return {unif,rng};
    }

}



class UniformIntGenerator{
public:
    static int lastSeed;

    UniformIntGenerator( LL minVal, LL maxVal, int seed = RandomNumberGenerators::DEFAULT_SEED ){
        unif = std::uniform_int_distribution<long long>(minVal, maxVal);
        rng.seed(lastSeed);
        lastSeed = (lastSeed + 1) % 1'000'000'007;
    }

    long long rand(){
        return unif(rng);
    }

    long long nextInt( long long U ){ return rand() % U; }

private:
    std::mt19937_64 rng;
    std::uniform_int_distribution<long long> unif;
};


class UniformDoubleGenerator{
public:
    static int lastSeed;
    UniformDoubleGenerator( double minVal, double maxVal, int seed = RandomNumberGenerators::DEFAULT_SEED ){
        rng.seed(lastSeed);
        lastSeed = (lastSeed + 1) % 1'000'000'007;
        unif = std::uniform_real_distribution<double>(minVal, maxVal);
    }

    double rand(){
        return unif(rng);
    }

private:
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> unif;
};

#endif //ALGORITHMSPROJECT_RANDOMGENERATORS_H
