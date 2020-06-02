/*
 * Copyright (c) 2020, Sylwester Swat
*/





#include <CONTESTS/PACE20/separatorcreators/CliqueSeparatorCreator.h>
#include <graphs/cliques/CliqueExtension.h>
#include <CONTESTS/PACE20/separatorminimizers/GreedyNodeEdgeMinimizer.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>


vector<Separator> CliqueSeparatorCreator::createSeparators(VVI &V, int repeats) {

    /*{
        Separator sep( V, CliqueExtension::findMaximalNodeCliqueExtension(V) );
        sep.createSeparatorStats();
//        DEBUG(sep);
        return{sep};
    }*/

    vector<Separator> res;

     // CLIQUE SEP CREATOR
    VB was(V.size(),false);
    for(int k=0; k<V.size(); k++){
        if(was[k])continue;
        VI clq = CliqueExtension::maximizeCliqueGreedy( V, {k} );

        for(int p : clq) was[p] = true;
        if( clq.size() < 5 ) continue;

//            DEBUG(clq.size());
        for( int i=(int)clq.size()-1; i>=0; i-- ){
            if( V[clq[i]].size() == clq.size()-1 ){
                swap( clq[i], clq.back() );
                clq.pop_back();
            }
        }
//            DEBUG(clq.size());
        Separator clqSep(V, clq);
        clqSep.createSeparatorStats();
//        if( clqSep.stats.numberOfComponents == 1 ) continue;

//            DEBUG(clqSep);


        res.push_back(clqSep);
        GreedyNodeEdgeMinimizer minimizer( Pace20Params::minimizeNodesIteration ? GreedyNodeEdgeMinimizer::MINIMIZE_NODES : GreedyNodeEdgeMinimizer::MINIMIZE_EDGES );
        minimizer.sepEval = &SeparatorEvaluators::sepEvalToUse;
        clqSep = minimizer.minimizeSeparator(clqSep);

//            DEBUG(clqSep);
//            ENDL(1);

        res.push_back(clqSep);
//            ENDL(1);
    }

//    exit(1);
    return res;
}

void CliqueSeparatorCreator::test() {


    exit(1);
}

