/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/
#ifndef ALGORITHMSPROJECT_CLIQUEUTILS_H
#define ALGORITHMSPROJECT_CLIQUEUTILS_H

#include "Makros.h"

namespace CliqueUtils{

    /**
     * Works in undiurected graphs
     * @param V
     * @param clq
     * @return true if clq induces a clique in graph V
     */
    extern bool isClique( VVI& V, VI& clq);


    /**
     * Function adds edges so that after adding @{nodes} induces a clique. Works for undirected graphs.
     * @param V
     * @param nodes
     */
    extern void fillToClique( VVI& V, VI nodes );

}


#endif //ALGORITHMSPROJECT_CLIQUEUTILS_H
