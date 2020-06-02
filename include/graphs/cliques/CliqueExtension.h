/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_CLIQUEEXTENSION_H
#define ALGORITHMSPROJECT_CLIQUEEXTENSION_H

#include "Makros.h"

namespace CliqueExtension{

    /**
     * Adds nodes from V to clq until clq forms a maximal clique. Nodes are added in random order
     * @param V
     * @param clq
     * @return
     */
    extern VI maximizeCliqueRandom( VVI& V, VI& clq );

    /**
     * Adds nodes frm V to clq until clq is a maximal clique.
     * First we take A = clq and B = N(clq) \ A
     * Then in each step we select from B node x that maximizes number of neighbors in B, and move x to A.
     * Time:
     * 1. time for inducing graph V[ N(clq) ], that is of order O( |N(N(clq))| )
     * 2. At most O( clq.size() ] operations for each iteration.
     * Terminates faster as soon as it detects that only 1 clique remains.
     *
     * @param V
     * @param clq
     * @return
     */
    extern VI maximizeCliqueGreedy(VVI& V, VI clq);

    /**
     * For each node v calls maximizeCliqueGreedy(V, {v}) and returns the largest clique found
     * @param V
     * @return
     */
    extern VI findMaximalNodeCliqueExtension(VVI& V);


    extern void test();

}

#endif //ALGORITHMSPROJECT_CLIQUEEXTENSION_H
