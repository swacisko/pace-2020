/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_SEPARATORMINIMIZER_H
#define ALGORITHMSPROJECT_SEPARATORMINIMIZER_H

#include "../Separator.h"

class SeparatorMinimizer{
public:

    virtual Separator minimizeSeparator( Separator sep ) = 0;

    function<bool(const Separator&,const Separator&)> *sepEval;

    static void createSeparatorGraphDataForSeparator(Separator &sep, VVPII &sepGraph, VI &nodeWeights, VI &edgeWeights);

protected:
    VVPII sepGraph; // graph in form  of edges a -> ( b, weight(b) )
    VVI gr; // gr[i][j] is just sepGraph[i][j].first
    VVI grW; // grW[i][j] is weight of edge i -> gr[i][j], that is sepGraph[i][j].second
    VI nodeWeights; // nodeWeights[i] is number of nodes in component represented by i in sepGraph, For i < sep.nodes.size() that is 1
    VI edgeWeights; // edgeWeights[i] is number of edges in component represented by i in sepGraph. For i < sep.nodes.size() that is 0
};

#endif //ALGORITHMSPROJECT_SEPARATORMINIMIZER_H
