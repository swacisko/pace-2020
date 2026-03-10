/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <contests/pace20/separatorminimizers/BFSMinimizer.h>
#include <contests/pace20/Pace20Params.h>
#include <contests/pace20/SeparatorEvaluators.h>
#include <graphs/GraphUtils.h>
#include <contests/pace20/separatorcreators/BFSSeparatorCreator.h>
#include "graphs/components/ConnectedComponents.h"
#include "datastructures/Heap.h"
#include "graphs/GraphReader.h"
#include "utils/StandardUtils.h"

Separator BFSMinimizer::minimizeSeparator(Separator sep) {
    VVI* V = sep.V;
    BFSSeparatorCreator bfsCr( *V,cnf );
    auto seps = bfsCr.createLayerSeparators( sep.nodes );

    SeparatorEvaluator sepEval = SeparatorEvaluators::sepEvalToUse;

    sort(ALL(seps), sepEval);

    return seps[0];
}

void BFSMinimizer::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    VI nodes = { 13 };
    Separator sep(V, nodes);
    sep.createSeparatorStats();

    Config cnf{};
    BFSMinimizer minim(cnf);
    auto res = minim.minimizeSeparator(sep);

    DEBUG(sep);
    DEBUG(res);

    exit(1);
}

