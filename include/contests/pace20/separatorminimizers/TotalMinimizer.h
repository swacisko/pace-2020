/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_TOTALMINIMIZER_H
#define ALGORITHMSPROJECT_TOTALMINIMIZER_H

#include "SeparatorMinimizer.h"


class TotalMinimizer : public SeparatorMinimizer{
public:

    TotalMinimizer( SeparatorEvaluator eval );

    Separator minimizeSeparator(Separator sep);

    /**
     * If true, then it will break minimization process after just one round of minimizations.
     * This can be used to iterative minimization of separators instead of total minimization of just a few separators.
     */
    bool oneRoundMinimization = false;

    static void test();

//    SeparatorEvaluator sepEval;

};

#endif //ALGORITHMSPROJECT_TOTALMINIMIZER_H
