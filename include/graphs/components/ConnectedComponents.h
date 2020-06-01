//
// Created by sylwester on 3/16/20.
//

#ifndef ALGORITHMSPROJECT_CONNECTEDCOMPONENTS_H
#define ALGORITHMSPROJECT_CONNECTEDCOMPONENTS_H

#include "Makros.h"

namespace ConnectedComponents{

    extern void dfs(VVI& V, int &num, int &par, VB& was, VVI & comps);
    /**
     *
     * @param V
     * @param sep
     * @return connected components of given graph V
     */
    extern VVI getConnectedComponents( VVI & V );

    /**
     *
     * @param V
     * @param sep
     * @return connected components of graph V \ S
     */
    extern VVI getConnectedComponents( VVI &V, VI & S );

    /**
     *  CAUTION!
     *  Function modifies was array. Each visited node in the same component as v is marked true.
     * @param V
     * @param was
     * @return nodes is a connected component of grapoh V induced by nodes for which was[v] = true. This way we can easily adapt to many uses.
     */
    extern VI getConnectedComponentForNode(VVI &V, int v, VB &was);

    /**
     *
     * @param V
     * @param W
     * @param v
     * @param was
     * @return sum of weights of edges in induced by was graph V[was] in component containing v
     */
    extern int countEdgeWeightsInComponent( VVI& V, VVI& W, int v, VB& was );

}

#endif //ALGORITHMSPROJECT_CONNECTEDCOMPONENTS_H
