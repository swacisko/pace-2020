//
// Created by sylwester on 4/10/20.
//

#ifndef ALGORITHMSPROJECT_EXPANSIONMINIMIZER_H
#define ALGORITHMSPROJECT_EXPANSIONMINIMIZER_H

#include "SeparatorMinimizer.h"

class ExpansionMinimizer : public SeparatorMinimizer{
public:


    Separator minimizeSeparator( Separator sep );


    static void test();


};

#endif //ALGORITHMSPROJECT_EXPANSIONMINIMIZER_H
