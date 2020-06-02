/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_SEPARATORCREATOR_H
#define ALGORITHMSPROJECT_SEPARATORCREATOR_H

#include <CONTESTS/PACE20/separatorminimizers/SeparatorMinimizer.h>
#include "Makros.h"

class SeparatorCreator{
public:


    virtual vector<Separator> createSeparators(VVI& V, int repeats=1) = 0;

    

};

#endif //ALGORITHMSPROJECT_SEPARATORCREATOR_H
