//
// Created by sylwester on 9/2/19.
//

#ifndef ALGORITHMSPROJECT_PARTITIONSVC_H
#define ALGORITHMSPROJECT_PARTITIONSVC_H


#include <frameworks/StochasticApproximation.h>
#include "SwapVC.h"


class PartitionSVCParams{
public:

    /**
     * With this probability each node in S will be added to S1
     */
    double alpha = 0.75;

    /**
     * When a subset S1 is fixed, then we create set W and induced graph G[ s1 cup W ]. For this graph we will use @{iterationsPerSubgraph} iterations of SwapVC to find possibly best
     *
     */
    int iterationsPerSubgraph = 100;

    /**
     * Max run time accepted for a subgraph in miliseconds.
     */
    long long maxRunTimeForSubgraph = Constants::INF;

    /**
     * Number of iterations done by SwapVC for creating initial solution.
     */
    int initialSolutionIterations = 1000;

    int initialSolutionMaxRunTime = 10000;

    /**
     * If in some iteration a soultion is found for the subgraph, that has a result not worse than @{maxDeviationFromBestResult} from it current result, it is swapped.
     * This way we do worsen current solution, but it may help to get out of local optimums.
     */
    int maxDeviationFromBestResult = 5;


    /**
     * If true, then each sugbrahph G[W cup S1) will be  initialized with known vertex cover W (mapped to induced graph).
     * Otherwise it will not be initialized.
     *
     * PLEASE NOTE that if this is set to true, then we will never get a worse solution after PartitionSVC nextIteration(), since there will always be a solution of size W in GW.
     *
     * Pros: good solution for a subgraph G[W cup S1] is INSTATNLY FOUND.
     * Cons: if no better solution for G[W cup S1] is found, then best solution is exactly W, so no equalizer-rotation is done.
     *      This could be overcome, by taking currentSolution for G[W cup S1] if it is the same size as best solution so far.
     */
    bool setInitialSolutionForSubgraph = false;


    /**
     * After each of @{localOptimumViolationFrequency} ierations, there will be @{localOptimumViolationDegree} random nodes from S moved to X, then S will be filled randomly to maximal
     * independent set
     * This is to try to break local optimums.
     */
    int localOptimumViolationDegree = 3;

    /**
     * After each of @{localOptimumViolationFrequency} ierations, there will be @{localOptimumViolationDegree} random nodes from S moved to X, then S will be filled randomly to maximal
     * independent set
     * This is to try to break local optimums.
     */
    int localOptimumViolationFrequency = 500;

};


/**
 * Class responsible for partition S algorithm.
 * In each iteration o subset S1 of currently known set S is taken.
 * Then set W of all nodes v in X that have   ( N(v) cap S )  is subset of S1   is calculated and vertex cover for G[ W cup S1 ] is determined.
 * Based on that vertex cover, a new vertex cover is set for currentSolution.
 */
class PartitionSVC : public SwapVC{
public:

    PartitionSVC( VVI & V );


    static void test();


    void run() override { SwapVC::run(); }

    PartitionSVCParams &getSvcParams();



    Solution *getInitialSolution() override;

    void makeInitialWork() override;

    void nextIteration() override;

protected:

    /**
     * Function forces getting out of local optimum.
     * It removes nodes from S and adds them to X, then S is filled randomly to maximal
     * independent set
     */
    void forceGettingOutOfLocalOptimum();


    /**
     * Function creates and returns random set S1m that is subset of S with probability params.alpha for each node
     * @return
     */
    VI getRandomSetS1();

    /**
     * Calculates set W with neighbors that have all neighbors from S in S1. This is done by the rule W = X \ N( S \ S1 )
     * @param S1
     * @return set W with neighbors that have all neighbors from S in S1
     */
    VI getSetWForS1( VI & S1 );


    PartitionSVCParams svcParams;


};

#endif //ALGORITHMSPROJECT_PARTITIONSVC_H
