/*
 * Copyright (c) 2020, Sylwester Swat
*/
#ifndef ALGORITHMSPROJECT_FLOWCUTTERMINIMIZER_H
#define ALGORITHMSPROJECT_FLOWCUTTERMINIMIZER_H

#include "SeparatorMinimizer.h"

class FlowCutterMinimizer : public SeparatorMinimizer{
public:

    FlowCutterMinimizer( SeparatorEvaluator eval ){ sepEval = &eval; }

    Separator minimizeSeparator( Separator sep );

    static void test();

    void setDistances( int left, int right ){ leftDistance = left; rightDistance = right; }

    static const int DISTANCE_MINIMIZATION = 1;
    static const int FURTHEST_POINT_MINIMIZATION = 2;
    int MINIMIZATION_MODE = DISTANCE_MINIMIZATION;
//    int MINIMIZATION_MODE = FURTHEST_POINT_MINIMIZATION;

private:

    /**
     * This mode works as follows: Take the set X of largest connected components of V \ S that have at least @balance fraction of total graph edges.
     * Then take all vertices that are at distance leftDistance from S in X, and all vertices that are at distance rightDistance from S in Y, where Y is the
     * set of remaining connected components. Then find separator using flow-cutter between those two sets of vertices
     * @param V
     * @param sep
     * @param comps
     * @param largestCompsNodes
     * @return
     */
    Separator getDistanceMinimizedSeparator(VVI *V, Separator &sep, VVI &comps, VI& edgesInComp);

    /**
     * Works as follows:
     * Let X and Y be the two largest connected components of G \ S. Take 1/3 of furthest vertices from S in those components, then find separator using flow-cutter
     * with sources in one set and targets in the other
     * @param sep
     * @return
     */
    Separator getFurthestPointMinimizedSeparator(Separator &sep, VVI &comps);

    int leftDistance = 2; // distance from separator in the largest component
    int rightDistance = 1; // distance from separator in the rest but the largest component
};

#endif //ALGORITHMSPROJECT_FLOWCUTTERMINIMIZER_H
