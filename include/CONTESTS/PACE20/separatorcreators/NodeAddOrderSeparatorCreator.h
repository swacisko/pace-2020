//
// Created by sylwester on 4/5/20.
//

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
