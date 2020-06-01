//
// Created by sylwester on 3/25/20.
//

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
