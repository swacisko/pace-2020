/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_CENTROIDDECOMPOSITION_H
#define ALGORITHMSPROJECT_CENTROIDDECOMPOSITION_H

#include "Makros.h"

namespace Centroids{

    /**
     *
     * @param V
     * @param v
     * @param was
     * @param subtreeSize must be additional array, to quickly acces values in dfs instead of creating the whole new array or using hash table.
     * @return centroid of tree that is induced by component containing v in graph V \ X, where X is set of all set nodes in was.
     */
    extern int getCentroid( VVI& V, int v, VB& was, VI& subtreeSize );

    extern int getCentroid(VVI& V);

    extern VI getCentroidDecomposition(VVI& V);



    extern void test();

}

#endif //ALGORITHMSPROJECT_CENTROIDDECOMPOSITION_H
