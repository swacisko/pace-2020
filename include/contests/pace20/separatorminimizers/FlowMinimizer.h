/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_FLOWMINIMIZER_H
#define ALGORITHMSPROJECT_FLOWMINIMIZER_H

#include "SeparatorMinimizer.h"

class FlowMinimizer : public SeparatorMinimizer{
public:


    Separator minimizeSeparator( Separator sep );


    static void test();

private:



};

#endif //ALGORITHMSPROJECT_FLOWMINIMIZER_H
