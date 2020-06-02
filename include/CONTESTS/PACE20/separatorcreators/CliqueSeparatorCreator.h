/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_CLIQUESEPARATORCREATOR_H
#define ALGORITHMSPROJECT_CLIQUESEPARATORCREATOR_H

#include "SeparatorCreator.h"

class CliqueSeparatorCreator : public SeparatorCreator{
public:

    vector<Separator> createSeparators(VVI& V, int repeats = 1);


    static void test();

};

#endif //ALGORITHMSPROJECT_CLIQUESEPARATORCREATOR_H
