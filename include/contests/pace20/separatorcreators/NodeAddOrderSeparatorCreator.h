/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_NODEADDORDERSEPARATORCREATOR_H
#define ALGORITHMSPROJECT_NODEADDORDERSEPARATORCREATOR_H

#include "SeparatorCreator.h"

/**
 * We add nodes in given order, as long as adding a new node does not violate the balance of components
 */
class NodeAddOrderSeparatorCreator : public SeparatorCreator{
public:

    vector<Separator> createSeparators(VVI& V, int repeats=1);

    static void test();

};

#endif //ALGORITHMSPROJECT_NODEADDORDERSEPARATORCREATOR_H
