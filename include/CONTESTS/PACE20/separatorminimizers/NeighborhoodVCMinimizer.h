//
// Created by sylwester on 3/16/20.
//

#ifndef ALGORITHMSPROJECT_NEIGHBORHOODVCMINIMIZER_H
#define ALGORITHMSPROJECT_NEIGHBORHOODVCMINIMIZER_H

#include "../Separator.h"
#include "SeparatorMinimizer.h"

/**
 * Function responsible for creating new (possibly better) separators based on given separator.
 * This class takes a separator S, then takes all components C from G \ S, finds smallest t, such that
 * |C_1| + ... + |C_t| >= (1 - Pace20Params::balance) * |G|
 * Then it finds minimal vertex cover in bipartite graph (S, N(S) \cap (C_1 \cup ... \cup C_t) ).
 * This way all components C_i for i <= t will remain disjoint, whereas other may connect. But still the new separator will be balanced.
 */
class NeighborhoodVCMinimizer : public SeparatorMinimizer{
public:


    Separator minimizeSeparator( Separator sep );


    static void test();

private:

};

#endif //ALGORITHMSPROJECT_NEIGHBORHOODVCMINIMIZER_H
