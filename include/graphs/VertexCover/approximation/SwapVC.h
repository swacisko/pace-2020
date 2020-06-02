/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_SWAPVC_H
#define ALGORITHMSPROJECT_SWAPVC_H

#include <frameworks/StochasticApproximation.h>
#include <graphs/GraphInducer.h>
#include <graphs/matching/MaxMatchBipartite.h>
#include "../VCUtils.h"


/**
 * Parameters for SwapVC solver.
 * It is advised to use default parameters and do not change them. (   however, only advised, not neccessary :)   )
 */
class SwapVCParams{
public:

    /**
     * If true, then i will use permuting mis, if false, i will in each iteration just find a new independent set Ix.
     */
    bool usePermMISIterations = false;

    /**
     * If true, then permuting mis approach will be used. It seems to be generally much faster and better than coloring.
     */
    bool useNextIterationPermutingMis = true;

    /**
     * If true, then local iterations will be used - i will not induce the graph in each iteration, but modify its structure based on the swapCandidate. It seems however, that
     * even though this method performs more candidate checks than usual permutating mis, it slowlier finds solutions of the same quality (it perhaps get somehow magically stuck in local minimums).
     */
    bool useLocalIterations = false;

    /**
     * If true, then SwapVC will in each iteratino perform one iteration of coloring approach and one iteration of permutating mis approach.
     * CAUTION - if useLocalIterations will be set, then there will be just one coloring and the rest of local iterations.
     */
    bool useBothColoringAndPermutatingMis = false;



    /**
     * If true, then i will look for hall equalizers.
     * CAUTION - If one want NOT TO USE AT ALL hall equalizers, then must set useHallEqualizerOnlyAfterIteration to false as well !!
     */
    bool useHallEqualizers = true;

    /**
     * If true, then hall equalizers will be sought only after each iteration. If false, then for each MIS Ix will be check for hall equalizers, what can take much more time
     * If this value is set to true, then automatically useHallEqualizers variable will be set to true at the end of each iteration.
     *
     * This may not be entirely true - in permuting mis we weill check the last \ceil( 25% * perm.size() ) candidates for hall equalizer. :)
     *
     * CAUTION - due to previous line if this is true, then setting manuallly useHallEqualizers to false will not prevent from looking for equalizers, but will reduce its frequency to once per iteration.
     *
     */
    bool useHallEqualizerOnlyAfterIteration = false;

    /**
     * If true, then i will take 'random' equalizer and swap it, instead of taking an equalizer that minimizes or maximizes weights.
     */
    bool useRandomEqualizerSwaps = true;

    /**
     * If true, then we will look for hall equalizer T \subset Ix with |T| = | N(T) \cup S | and sum_of_degrees(T) < sum_of_degrees( N(T) \cup S ). If false, then there will be ">" instead if "<".
     *
     * In case of MinWeighedVC, we will minimize or maximize the weight of the MIS!!!.
     * !!!
     * CAUTION - if we want to minimize the weight of the vertex cover, then we should set this value to false!!
     * !!!
     */
    bool minimizeWeightOfMIS = true;

    /*
     * If true, then the run of each algorithm will consist of @fluctuatingDegreesMaximizationFrequency phases, after each phase, value of @minimizeWeightOfMIS will be changed to the opposite.
     */
    bool useFluctuatingDegreesMaximization = false;

    /**
     * Number of changes of value @minimizeDegreeOfEqualizer to the opposite.
     */
    int fluctuatingDegreesMaximizationFrequency = 11;

    /**
     * If true, then in each iteration i will try to find a hall violator using SmallHallViolatorFinder.
     */
    bool useSmallHallViolatorFinder = true;

    /**
     * maximal degree that will be subjected to Small Hall Violator method.
     */
    int smallHallViolatorMaxDegree = Constants::INF;

    /**
     * If true, then small hall violator will accept equalizers.
     *
     * Note, that in LARGE graphs this may reduce significantly size of the found violator.
     */
    bool useSmallHallViolatorEqualizers = true;

    /**
     * If true, then i will use kernelization for graph V before local search.
     */
    bool useKernelization = false;
};


class SwapVC : public StochasticApproximation {
public:

    SwapVC( VVI & V );

    virtual ~SwapVC();

    /**
     * Function used to test.
     */
    static void test();

    SwapVCParams& getParams(){ return params; }

    /**
     * Function sets graph structure. This must be invoked before @{run} method naturally (it must not be invoked if proper graph is passed via constructor).
     * @param V
     */
    void setV( VVI & V ){ this->V = V; }

    virtual void run() override;

protected:

    /**
     * This vector contains for each node number of iteration in which it was for the last time in Ix.
     * This way i can 'force' nodes that have small score values to appear from time to time in Ix anyway by adding iterationsDone - lastTimeVisit[i] to score[i]
     * during filling to random maximal independent set in nextIterationPermutatingMIS
     */
    VI lastTimeVisit;

    SwapVCParams params;

    Solution *getInitialSolution() override;

    void nextIteration() override;

    bool updateBestSolution(Solution *sol) override;

    void makeInitialWork() override;

    /**
     * This is one of the variants for iteration. Set X is colored with at most \Delta colors, and each color in turn is extended to maximal independent set.
     */
    void nextIterationWithColoring();


    /**
     * This is second variant of nextIteration. Here we take a random permutation of nodes in X and in turn:
     * add next node to X and remove all its neighbors, then fill to maximal independent set.
     */
    void nextIterationWithPermutingMis();

    /**
     * This function is used to mark, that i want to look for MinWeighedVC not just MinVC.
     * @tparam _T
     * @param val true if i want to use weights other than 1
     * @param weights weights assigned to nodes
     */
    template<class _T>
    void setFindMinWeighedVC( bool val, vector<_T> & weights ){ findMinWeighedVC = val; nodeWeights = VD( V.size() ); for( int i=0; i<V.size(); i++ ) nodeWeights[i] = weights[i]; }

    /**
     * The structure of the graph for which to find a vertex cover.
     */
    VVI V;

    /**
     * Set S is the independent set (upper) in swapvc algorithm, X is the vertex cover (bottom part).
     * Ix is the independent set chosen from X as a candidate for swap.
     * All sets S,X and Ix must have ids of nodes in original graph, not induced.
     */
    VI S,X, Ix;

    InducedGraph /*SInduced,*/ XInduced, /*IxInduced,*/ SIxInduced;

    /**
     * Function responsible for creating induced graphs by set S,X/
     */
    void createInducedX();

    /**
     *  Function responsible for creating induced graphs by set Ix and ( s \cup Ix ).
     *  This function is used in each iteration for each independent set Ix.
     */
    void createInducedSIx();
    /**
     *
     * @return random maximal independent set from X, that contains given nodes
     */
    VI getRandomMISFromXWithNodes( VI & nodes );

    /**
     * This is the main part of the algorithm. Function finds a candidate for swap, that is a subset T \subset Ix , such that changing T with N(T) gives better solution
     * with regard to some criterion (here smaller or with better weight - e.g. measured as sum of outdegrees of S).
     * @return swap candidate. If there is no swap candidate, returned vector is empty.
     */
    VI findSwapCandidate();

    /**
     * Function is used when there is no hall violator found in graph SIx.
     * Function looks for subset T of X s.t. T is MIS in X, |N(T) \cap S| = |T| and w(T) > w( N(T) ), where w is some weight function.
     * This does not guarantee that such a set will be found if exists, but is PROBABLY just a heuristic (maybe it is exact, but i cannot yet prove it).
     * @param matching matching in graph given by the strucutre.
     * @param nodeDegrees is just a vector containing degrees IN THE ORIGINAL GRAPH of i-th node in structure. This is to enable finding hall equalizer in induced graphs by just passing mapped degrees.
     * CAUTION here - if the structure is induced then for example nodeDegrees[7] may be a degree of node 132 in the original graph - nodeDegrees must be properly mapped before passing to this function.
     * @param mapper - this is used to be able to select nodes that are in the S set. It is impossible only based on bipartition, because nodes in X \ Ix are marked with bipartition=true, just as nodes in S.
     * When using Coloring version, we need to pass a mapper. In permutating mis we do not need to pass enything.
     * @return hall equalizer set in given structure, that is a swap candidate for @applySwapCandidate function
     */
    VI findHallEqualizer(VVI &structure, VI &matching, VB &bipartition, VI &nodeDegrees, VI mapper = VI());


    /**
     * Function finds and returns nodes that should not be in the equalizer graph. These nodes are exactly those in X, that can be obtained via alternating paths with start point
     * in an unsaturated node in S
     * @param mapper - this is used to be able to select nodes that are in the S set. It is impossible only based on bipartition, because nodes in X \ Ix are marked with bipartition=true, just as nodes in S.
     * @return
     */
    VI getNodesToRemoveFromEqualizerGraph(VVI &structure, VB &bipartition, VI &matching, VI &mapper);


    /**
     * Function assigns a weight to each set of nodes. Each of these sets represents a connected component in the SCC graph used to find hall equalizer.
     * Depending on what we want to achieve we may maximize or minimize (or something else) sum of degrees in the set S.
     * By default (as implemented in this function) each component will have the sum of weights of nodes it contains.
     * By default (as implemented) weight of a single node in the original graph is a difference between its degree and the degree of node matched with it.
     * @param comps
     * @weights
     */
    VD assignStronglyConnectedComponentWeights(VVI &comps, VI &nodeDegrees, VI &matching);

    /**
     * Function takes vertices that are in swapCandidate and puts them into S and removes from S the set N( swapCandidates ).
     * @param swapCandidate
     * @param idsInSIx if true then swapCandidate must have ids of nodes in SIx (it will be faster to get neighborhood of swapCandidate when intersected with S).
     * @param SIxStructure This is either SIxInduced.V if idsInSIx is set to true (for next iteration coloring version) or SIxStructure if idsInSIx is set to false (in permuting mis version)
     */
    void applySwapCandidate(VI swapCandidate, bool idsInSIx, VVI &SIxStructure);






    //********************************************************************************************** PERMUTING MIS

    /**
     * This is equivalent for @findSwapCandidate function, but when using PERMUTING MIS technique, we do not need to create SIx induced each time, we can just modify it quickly
     * @return
     */
    VI findSwapCandidatePermutingMIS(VVI &SIxStructure, MaxMatchBipartite &matcher, VI &matching, VB &bipartition,
                                     VI &addedNodes);

    /**
     * Function adds node v to SIxStructure and removes its neighbors from SIxStructure.
     * @return nodes that were removed from Ix are returned, they will be needed for updating degreeInX values
     */
    VI moveNodeToSIxStructure(VVI &SIxStructure, int v, VI &degreeInIx, VB &inS, VB &inIx);

    /**
     * Function used to make Ix a MIS not only IS after removing nodes.
     * @param pivotNode it is just p in perm, but since funcion is used multiple times, it is here called pivotNode. By default there is no pivotNode, so it is set to -1
     */
    void fillIxToMIS(VVI &SIxStructure, VI &removedNodes, VI &addedNodes, VI &matching, VI &degreeInIx, VB &inS,
                     VB &inIx, VB &bipartition, int pivotNode = -1);

    /**
     * This is used to quickly update data after applying a swap candidate - we do not need to restart everything.
     * @param swapCandidate this must be a swap candidate with ids in ORIGINAL graph G (not like in applySwapCandidate with ids in SIx)
     */
    void modifyDataAfterApplyingSwapCandidate(VVI &SIxStructure, VI &swapCandidate, VB &inIx, VB &inS, VI &degreeInIx,
                                              VI &matching, VB &bipartition);
    /**
     * Function used in permuting_mis_local_iteration_viariant. It shuffles perm of considered nodes, sets local variable i to -1, increases localIterations counter
     * @return true if i should continue - if time limit and maximum number of iterations were not reached yet
     */
    bool startNextLocalIteration( VI &perm, int& i, int& localIterations );

    // ************************************************************************************  END OF PERMUTING MIS






    /**
     * This is just value for comparing two nextIterations methods: 'coloring' and 'permuting mis'. It equals the total number of Ix subsets of X checked by the algorithm.
     */
    int candidatesChecked;

    /**
     *
     * @return sum of degrees of nodes in S.
     */
    int getSumOfDegreesOfNodesInS();




    //**********************************************************   WEIGHED MinVC

    /**
     * If true, then i will look for a minimum weighed vertex cover, not a vertex cover with minimal number of nodes.
     * If false, then i will look for minimum vertex cover (with regard to number of nodes).
     */
    bool findMinWeighedVC;

    /**
     * Vector containing weight of nodes.
     * If @{findMinWeighedVC} is false, then this will be VD( V.size() ,1)
     */
    VD nodeWeights;

    /**
     *
     * @return weight of vertex cover X
     */
    double getWeightOfX(){ return getWeightOf(X); }

    /**
     *
     * @param nodes
     * @return weight of nodes in given vector
     */
    double getWeightOf( VI & nodes ){ double res = 0; for(int d : nodes) res += nodeWeights[d]; return res; }

    //**********************************************************  END OF WEIGHED MinVC


};


#endif //ALGORITHMSPROJECT_SWAPVC_H
