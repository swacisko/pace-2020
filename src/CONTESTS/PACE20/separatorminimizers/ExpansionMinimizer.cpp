//
// Created by sylwester on 4/10/20.
//

#include <CONTESTS/PACE20/separatorminimizers/ExpansionMinimizer.h>
#include "CONTESTS/PACE20/separatorcreators/ComponentExpansionSeparatorCreator.h"
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include <graphs/GraphReader.h>
#include <graphs/trees/Tree.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include "graphs/GraphInducer.h"

Separator ExpansionMinimizer::minimizeSeparator(Separator sep) {
//    ComponentExpansionSeparatorCreator ceCr( SeparatorEvaluators::estimatedDepthTreeEdge );
    ComponentExpansionSeparatorCreator ceCr( SeparatorEvaluators::sepEvalToUse );

    ceCr.setOrdersToCreate( ceCr.TIGHTEST_NODE_ORDER + ceCr.LEAST_NEIGHBORS_ORDER );
//    ceCr.setOrdersToOptimize( ceCr.TIGHTEST_NODE_ORDER + ceCr.LEAST_NEIGHBORS_ORDER );

    VVI* V = sep.V;
    VVI orders = ceCr.getExpansionOrdersForNodes( *V, sep.nodes );

    // #TEST testing minimizing only with optimized orders
    orders.erase( orders.begin() + orders.size() / 2 );


    Separator bestSep = sep;
    for(int i=0; i<orders.size(); i++){
        if( Pace20Params::tle ) break;
        Separator sep = ceCr.getBestSeparatorForExpansionOrder( *V, orders[i] );

        if( ceCr.sepEval( sep, bestSep ) ){
            bestSep = sep;
        }
    }

    return bestSep;
}

void ExpansionMinimizer::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    VI nodes = { 1,18 };
    Separator sep(V, nodes);
    sep.createSeparatorStats();

    ExpansionMinimizer minim;
    auto res = minim.minimizeSeparator(sep);

    DEBUG(sep);
    DEBUG(res);

//    res = minim.minimizeSeparator(res);

    exit(1);
}

