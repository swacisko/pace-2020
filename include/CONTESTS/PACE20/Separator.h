//
// Created by sylwester on 3/16/20.
//

#ifndef ALGORITHMSPROJECT_SEPARATOR_H
#define ALGORITHMSPROJECT_SEPARATOR_H

#include "Makros.h"
#include "SeparatorStats.h"


class Separator{
public:
    Separator( VVI & V, VI nodes );

    VVI* V;
    VI nodes;

    /**
     * Creates all stats for given separator. This is time-consuming - uses O( V + E ) time to traverse whole graph V
     */
    void createSeparatorStats();

    void updatePointers(VVI & V);

    SeparatorStats stats;

    Separator& operator=( const Separator& oth );


    friend ostream& operator<<(ostream& str, Separator& sep);

};

using SeparatorEvaluator = function< bool(const Separator&, const Separator&) >;

#endif //ALGORITHMSPROJECT_SEPARATOR_H
