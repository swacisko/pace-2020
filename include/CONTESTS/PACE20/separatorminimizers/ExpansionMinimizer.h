/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_EXPANSIONMINIMIZER_H
#define ALGORITHMSPROJECT_EXPANSIONMINIMIZER_H

#include "SeparatorMinimizer.h"

class ExpansionMinimizer : public SeparatorMinimizer{
public:


    Separator minimizeSeparator( Separator sep );


    static void test();


};

#endif //ALGORITHMSPROJECT_EXPANSIONMINIMIZER_H
