/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/separatorcreators/RemovalOrderSeparatorCreator.h>
#include <datastructures/FAU.h>
#include <graphs/GraphUtils.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <graphs/GraphReader.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <CONTESTS/PACE20/separatorminimizers/GreedyNodeEdgeMinimizer.h>


RemovalOrderSeparatorCreator::RemovalOrderSeparatorCreator(VVI &V) {
    this->V = V;
}

Separator
RemovalOrderSeparatorCreator::getBestSeparatorForRemovalOrder(VI order, function<bool(Separator &, Separator &)> eval) {
    int N = V.size();
    FAU fau(N);
    VI ord = order;


    VB inSep(N,true);
//    inSep[ ord.back() ] = false;
//    ord.pop_back();
    Separator sep(V,ord );
    sep.stats.size = ord.size();
    sep.stats.originalGraphEdges = GraphUtils::countEdges(V);
    sep.stats.originalGraphSize = V.size();

    Separator bestSep = sep;
    int bestSepIndex = order.size(); // bestSepIndex   is the number of nodes in the separator

    VI edgesInComp(N,0);
    VB was(N,false);

//        DEBUG(ord.size());
    while( ord.size() > 1 ){

        int p = ord.back();
        ord.pop_back();

//        DEBUG(ord.size());
//        DEBUG(p);

        inSep[p] = false;

        int neighInV = 0;
        int edgesInCompNeighbors = 0;

        for( int d : V[p] ){
            if( inSep[d] ) continue;
            if( !was[ fau.Find(d) ] ){ // I need to consider each component only once
                edgesInCompNeighbors += edgesInComp[ fau.Find(d) ];
                was[ fau.Find(d) ] = true;
            }
            neighInV++;
        }

        for( int d : V[p] ){
            if( inSep[d] ) continue;
            was[ fau.Find(d) ] = false; // clearing was
        }

        for( int d : V[p] ){
            if( inSep[d] ) continue;
            fau.Union(p,d);
        }

//        DEBUG(edgesInCompNeighbors);
//        DEBUG(neighInV);

        edgesInComp[ fau.Find(p) ] = edgesInCompNeighbors + neighInV;

        // updating stats
        sep.stats.size--;
        sep.stats.maxCompSize = max( sep.stats.maxCompSize, fau.countNodesInComponent(p) );
        sep.stats.maxCompEdges = max( sep.stats.maxCompEdges, edgesInComp[ fau.Find(p) ] );

//        DEBUG(sep.stats);
//        ENDL(2);

        int sizeTHR = min( Pace20Params::maxSeparatorSizeForGNEMinimizer, (int)order.size()-1 );
        if(   ( eval(sep,bestSep) && ord.size() < sizeTHR )    || ord.size() == sizeTHR ){
            bestSep = sep;
            bestSepIndex = ord.size();

//            cerr << "Found better separator: " << sep.stats << endl;
        }


    }

//    DEBUG(bestSep.stats);
    bestSep.nodes.clear();
    for(int i=0; i<bestSepIndex; i++) bestSep.nodes.push_back(order[i]);
    bestSep.createSeparatorStats();

//    DEBUG(bestSep.stats);

//
//    exit(1);
    return bestSep;


}

VI RemovalOrderSeparatorCreator::getGreatestDegreeOrder(VVI &V) {
    VI order(V.size());
    iota(ALL(order),0);
    random_shuffle(ALL(order));
    stable_sort( ALL(order),[&V](int a, int b){ return V[a].size() > V[b].size(); } );
    return order;
}

VI RemovalOrderSeparatorCreator::getGreatestDegreeIterativeOrder(VVI &V) {
    int N = V.size();
    VI order(N);
    iota(ALL(order),0);
    VI deg(N,0);
    for( int i=0; i<N; i++ ) deg[i] = V[i].size();


    auto comp = [&V,&deg](int a, int b){
        if( deg[a] != deg[b] ) return deg[a] > deg[b];
        else if( V[a].size() != V[b].size() ) return V[a].size() > V[b].size();
        else return a < b;
    };

    set<int, decltype(comp) > zb( ALL(order) ,comp);

//    DEBUG(order);
//    for( int it : zb ) cerr << it << "  "; cerr << endl;

    order.clear();

    VB is(N,true);

    while( order.size() < V.size() ){

        int p = *zb.begin();
        zb.erase( zb.begin() );
        is[p] = false;
        deg[p] = 0;
        order.push_back(p);

//        DEBUG(p);
//        DEBUG(deg);

        VI neigh;
        for( int d : V[p] ){
            if(is[d]){
                neigh.push_back(d);
                zb.erase(d);
            }
        }

        for( int d : neigh ){
            deg[d]--;
            zb.insert(d);
        }

//        DEBUG(deg);
//        ENDL(2);

    }

//    DEBUG(order);
    return order;
}

void RemovalOrderSeparatorCreator::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    RemovalOrderSeparatorCreator roCr(V);
//    VI order = roCr.getGreatestDegreeIterativeOrder(*V);
    VI order = roCr.getGreatestDegreeOrder(V);
    Separator sep = roCr.getBestSeparatorForRemovalOrder( order, SeparatorEvaluators::estimatedDepthTreeMinNodeEdge );

    DEBUG(sep.stats);
    GreedyNodeEdgeMinimizer minim(GreedyNodeEdgeMinimizer::MINIMIZE_EDGES);
    sep = minim.minimizeSeparator(sep);
    DEBUG(sep.stats);


    exit(1);



}
