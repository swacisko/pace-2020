/*
 * Copyright (c) 2020, Sylwester Swat
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
