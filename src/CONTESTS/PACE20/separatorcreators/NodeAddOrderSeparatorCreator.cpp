/*
 * Copyright (c) 2020, Sylwester Swat
*/


#include <graphs/GraphReader.h>
#include "CONTESTS/PACE20/separatorcreators/NodeAddOrderSeparatorCreator.h"
#include <graphs/GraphUtils.h>
#include <combinatorics/CombinatoricUtils.h>
#include <datastructures/FAU.h>


vector<Separator> NodeAddOrderSeparatorCreator::createSeparators(VVI &V, int repeats) {
    int N = V.size();
    cerr << "NodeAddOrderSeparatorCreator NOT IMPLEMENTED YET" << endl; exit(1);

    VI perm = CombinatoricUtils::getRandomPermutation(N);

    FAU fau(N);
    VI edgesInComponent(N,0);





    vector<Separator> res;

    exit(3);
    return res;
}

void NodeAddOrderSeparatorCreator::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );



    exit(1);
}
