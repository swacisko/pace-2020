//
// Created by sylwester on 4/5/20.
//

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
