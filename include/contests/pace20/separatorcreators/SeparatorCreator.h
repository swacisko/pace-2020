/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_SEPARATORCREATOR_H
#define ALGORITHMSPROJECT_SEPARATORCREATOR_H

#include <contests/pace20/separatorminimizers/SeparatorMinimizer.h>
#include "Makros.h"
#include "contests/experiments/Config.h"

class SeparatorCreator{
public:
    virtual ~SeparatorCreator() = default;

    SeparatorCreator(Config c) : cnf(c){}

    virtual vector<Separator> createSeparators(VVI& V, int repeats=1) = 0;

    
    Config cnf;
};

#endif //ALGORITHMSPROJECT_SEPARATORCREATOR_H
