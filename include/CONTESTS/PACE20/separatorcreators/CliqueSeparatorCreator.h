//
// Created by sylwester on 4/6/20.
//

#ifndef ALGORITHMSPROJECT_CLIQUESEPARATORCREATOR_H
#define ALGORITHMSPROJECT_CLIQUESEPARATORCREATOR_H

#include "SeparatorCreator.h"

class CliqueSeparatorCreator : public SeparatorCreator{
public:

    vector<Separator> createSeparators(VVI& V, int repeats = 1);


    static void test();

};

#endif //ALGORITHMSPROJECT_CLIQUESEPARATORCREATOR_H
