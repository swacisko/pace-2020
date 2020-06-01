//
// Created by sylwester on 4/10/20.
//

#ifndef ALGORITHMSPROJECT_BFSMINIMIZER_H
#define ALGORITHMSPROJECT_BFSMINIMIZER_H

#include "SeparatorMinimizer.h"

class BFSMinimizer : public SeparatorMinimizer{
public:

    Separator minimizeSeparator(Separator sep);


    static void test();

};

#endif //ALGORITHMSPROJECT_BFSMINIMIZER_H
