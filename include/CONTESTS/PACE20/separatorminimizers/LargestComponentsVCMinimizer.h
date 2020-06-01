//
// Created by sylwester on 3/29/20.
//

#ifndef ALGORITHMSPROJECT_LARGESTCOMPONENTSVCMINIMIZER_H
#define ALGORITHMSPROJECT_LARGESTCOMPONENTSVCMINIMIZER_H

#include "SeparatorMinimizer.h"

/**
 * This minimizers takes first the union X of certain number of largest connected components of V \ sep.nodes, and finds VC of
 * bipartite graph (sep.nodes, N( sep.nodes ) \cap X)
 */
class LargestComponentsVCMinimizer : public SeparatorMinimizer{

public:

    Separator minimizeSeparator(Separator sep);
    Separator minimizeSeparator(Separator sep, double sizeBalance );

    static void test();

    double sizeBalance = 0.25;

};

#endif //ALGORITHMSPROJECT_LARGESTCOMPONENTSVCMINIMIZER_H
