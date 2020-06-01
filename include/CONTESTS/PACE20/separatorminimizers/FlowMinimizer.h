//
// Created by sylwester on 4/4/20.
//

#ifndef ALGORITHMSPROJECT_FLOWMINIMIZER_H
#define ALGORITHMSPROJECT_FLOWMINIMIZER_H

#include "SeparatorMinimizer.h"

class FlowMinimizer : public SeparatorMinimizer{
public:


    Separator minimizeSeparator( Separator sep );


    static void test();

private:



};

#endif //ALGORITHMSPROJECT_FLOWMINIMIZER_H
