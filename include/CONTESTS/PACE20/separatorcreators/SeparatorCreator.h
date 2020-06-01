//
// Created by sylwester on 3/25/20.
//

#ifndef ALGORITHMSPROJECT_SEPARATORCREATOR_H
#define ALGORITHMSPROJECT_SEPARATORCREATOR_H

#include <CONTESTS/PACE20/separatorminimizers/SeparatorMinimizer.h>
#include "Makros.h"

class SeparatorCreator{
public:


    virtual vector<Separator> createSeparators(VVI& V, int repeats=1) = 0;

    

};

#endif //ALGORITHMSPROJECT_SEPARATORCREATOR_H
