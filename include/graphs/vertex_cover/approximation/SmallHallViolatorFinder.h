/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_SMALLHALLVIOLATORFINDER_H
#define ALGORITHMSPROJECT_SMALLHALLVIOLATORFINDER_H

#include "Makros.h"


class SwapVC;

/**
 * Class responsible for finding a small Hall Violator. This is just a heuristic and works as follows.
 * For each node v in X with d <= parameter neighbors in the set S we take set   W subset X with property, that all nodes in W have do not have a neighbor in S \ ( N(v) cap S ).
 * if d == 1 this is done efficiently in time O( W * E_W ), where E_W is the number of edges in the induced graphs G[W]
 *
 *
 *
 */
class SmallHallViolatorFinder{
public:

    /**
     * Constructor
     * @param V structure of the whole graph
     * @param S independent set in that graph
     */
    SmallHallViolatorFinder( VVI & V, VI & S );


    /**
     * Function checks each node in X degree in S not greater than @{centerNodeMaxDegree} using @{findCenterNodeHallViolator} function and based on the results returns violator or equalizer.
     * @param centerNodeMaxDegree
     * @return
     */
    VI findSwapCandidate( int centerNodeMaxDegree, bool admitHallEqualizers = false );




    /**
     * Function looks for the hall violator with center in given centerNode in X. Center that is take N(v) cap S and find set W... etc as in the class description.
     * @param v
     * @param admitHallEqualizer if true, then i may return an equalizer if violator does not exist.
     * @return
     */
    VI findCenterNodeHallViolator(int centerNode, bool admitHallEqualizer = false);

    /**
     * Function is a specializetion of @{findCenterNodeHallViolator} for center nodes with just one neighbor in the set S - is it then done quickly, not using
     * @param centerNode
     * @param admitHallEqualizer
     * @return
     */
    VI findDeg1CenterNodeHallViolator(int centerNode, bool admitHallEqualizer = false);

private:

    VVI V;
    VI S,X;
    VB inS,inX;


    /**
     * This is a helper vector to find quickly sets W. It stores number of neighbors of nodes in X that are in S / S1. If this qill be equal 0, then given node will be added to W.
     */
    VI degreeInS2;

    /**
     * Set of hashes of subsets in S that were considered. This is to avoid checking the same subset in S multiple times (if two center nodes give the same neighborhood in S).
     */
    unordered_set<LL> subsetsOfSConsidered;

    /**
     *
     * @param subsetOfS
     * @return hash of given subset to store in @{subsetsOfSConsidered}
     */
    LL getHash( VI & subsetOfS );


    /**
     * Array keeps true, if a node in x can be considered for the set W in some call of @{findCenterNodeHallViolator} function.
     * This must be used, because considering multiple center nodes hall violator we might have violated the condition of the union of violators being and independent set in X.
     * Hence, a node will be added to W if and only if it is not a neighbors of a node added earlier to hall violator (or equalizer if admissible).
     */
    VB admissibleNodesInXForIndependentSet;


    /**
     * Function creates neccessary initial data, such as sets X, inS, inX etc
     */
    void makeInitialStuff();

    /**
     * Function finds set W for given S1 subset S. It uses degreeInS2 values to do it efficiently
     * @param centerNode
     * @return set W of nodes v in X that do not have a neighbor in S2
     */
    VI findSetW( VI &neighInS );


    SwapVC *vcFinder;
};

#endif //ALGORITHMSPROJECT_SMALLHALLVIOLATORFINDER_H
