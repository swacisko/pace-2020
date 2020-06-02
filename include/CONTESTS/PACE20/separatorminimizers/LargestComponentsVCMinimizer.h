/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

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
