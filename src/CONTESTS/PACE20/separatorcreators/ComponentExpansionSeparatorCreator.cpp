/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <CONTESTS/PACE20/separatorcreators/ComponentExpansionSeparatorCreator.h>
#include <graphs/GraphUtils.h>
#include <datastructures/FAU.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <graphs/GraphReader.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <graphs/generators/GraphGenerator.h>
#include "datastructures/Heap.h"
#include "graphs/graphtraversals/BFS.h"
#include "graphs/components/ConnectedComponents.h"
#include <graphs/GraphInducer.h>
#include <utils/StandardUtils.h>
#include "graphs/VertexCover/BipartiteGraphVertexCover.h"
#include "combinatorics/CombinatoricUtils.h"


ComponentExpansionSeparatorCreator::ComponentExpansionSeparatorCreator(function<bool(const Separator &, const Separator &)> eval) {
    sepEval = eval;
}

vector<Separator> ComponentExpansionSeparatorCreator::createSeparators(VVI &V, int repeats) {
    int N = V.size();
    repeats = min( repeats,N );

    VI repeatsSources = CombinatoricUtils::getRandomSubset( N-1, repeats );

    vector<Separator> seps;
    for( int r = 0; r < repeats; r++ ){
//        int a = rand()%N;
        int a = repeatsSources[r];
        int b = repeatsSources[ (r+1)%repeats ];
        int c = repeatsSources[ (r+2)%repeats ];

//        set<int> zb;
//        if( r % 3 == 0 ) zb = {a};
//        else if(r%3==1) zb = {a,b};
//        else zb = {a,b,c};

        set<int> zb = {a,b,c};




        bool useIterativeExpansion = false;

        if(useIterativeExpansion) {
            auto sep = getIterativeSepForStartingNodes(V, VI(ALL(zb)));
            seps.push_back(sep);
        }
        else {

            VVI orders = getExpansionOrdersForNodes(V, VI(ALL(zb)));
//            VVI orders = getExpansionOrdersForNode(V,a);

            vector<Separator> newSeps;

            for (int i = 0; i < orders.size(); i++) {
                if (Pace20Params::tle) break;
                Separator sep = getBestSeparatorForExpansionOrder(V, orders[i]);


                newSeps.push_back(sep);
            }


            if (Pace20Params::tle) break;
            bool improveSeparator = true;
            if (improveSeparator) {
                sort(ALL(newSeps), sepEval);
                Separator sep = newSeps[0];

                if (ORDERS_TO_OPTIMIZE & TIGHTEST_NODE_ORDER) {
                    seps.push_back(getBestSeparatorForExpansionOrder(V, optimizeOrderByTraversingFirstSmallestComponents(V, getTightestNeighborOrder(V, sep.nodes))));
                }

                if (ORDERS_TO_OPTIMIZE & LEAST_NEIGHBORS_ORDER) {
                    seps.push_back(getBestSeparatorForExpansionOrder(V, optimizeOrderByTraversingFirstSmallestComponents(V, getLeastExteriorNeighborhoodOrder(V, sep.nodes))));
                }

            }


            seps.insert(seps.end(), ALL(newSeps));
        }

//        ENDL(1);

        if( Pace20Params::tle ) break;
    }

//    exit(1);
    for(auto& sp : seps) sp.updatePointers(V);

    return seps;
}



Separator ComponentExpansionSeparatorCreator::getBestSeparatorForExpansionOrder(VVI& V, VI order) {
    bool debug = false;

    int N = V.size();




    CompBorder borderAndComp = calculateInternalSizes(V, order); // borderSize[i] is the number of nodes on the border of component induced by nodes ord[0] + ord[1] + ... + ord[i]
    VI borderSize = borderAndComp.borderSize;
    VI compEdges = borderAndComp.compEdges;

    if(debug){
        DEBUG(borderSize);
        DEBUG(compEdges);
    }

    pair<VI,VI> compSizes = calculateExternalSizes(V, order);

    if(debug){
        DEBUG(compSizes.first);
        DEBUG(compSizes.second);
    }


    VVI mods = borderAndComp.modifications;

    set<int> initNodes;
    for(int p : mods[0]) if( initNodes.count(p) ) initNodes.erase(p); else initNodes.insert(p);
    assert(initNodes.size()==1);

    Separator sep(V, VI(ALL(initNodes)) );
    sep.V = &V;
    sep.stats.originalGraphSize = V.size();
    sep.stats.originalGraphEdges = GraphUtils::countEdges(V);
    sep.createSeparatorStats();
    Separator bestSep = sep;
    bestSep.updatePointers(V);



    if(debug){
        cerr << "Adding init nodes: " << initNodes << endl;
    }

    set<int> curNodes = initNodes;

    int bestInd = 0;
    for( int i=1; i<N-1; i++ ){



        sep.stats.size = borderSize[i];

//        sep.stats.maxCompSize = max( i+1-borderSize[i], compSizes.first[i] );
//        sep.stats.maxCompEdges = max( compEdges[i], compSizes.second[i] );

        sep.stats.maxCompSize = max( borderAndComp.maxInternalCompSize[i], compSizes.first[i] );
        sep.stats.maxCompEdges = max( borderAndComp.maxInternalCompEdges[i], compSizes.second[i] );

        if(debug){
            cerr << "Adding " << order[i] << endl;
            cerr << "\tborderSize: " << borderSize[i] << endl;

            cerr << "\tall internal nodes: " << i+1-borderSize[i] << endl;
            cerr << "\tall internal edges: " << compEdges[i] << endl;
            cerr << "\tmax internal nodes: " << borderAndComp.maxInternalCompSize[i] << endl;
            cerr << "\tmax internal edges: " << borderAndComp.maxInternalCompEdges[i] << endl;

            cerr << "\tmax external nodes: " << compSizes.first[i+1] << endl;
            cerr << "\tmax external edges: " << compSizes.second[i+1] << endl;

            for( int p : mods[i] ) if( curNodes.count(p) ) curNodes.erase(p); else curNodes.insert(p);
            DEBUG(curNodes);
            ENDL(2);
        }


        bool testCorrectness = false;
        if(testCorrectness) {
            cerr << "\rTESTING! SLOW CHECK IN EXP_CREATOR" << flush;

            if (!debug) for (int p : mods[i]) if (curNodes.count(p)) curNodes.erase(p); else curNodes.insert(p);
            Separator testSep(V, VI(ALL(curNodes)));
            testSep.createSeparatorStats();
            if (testSep.stats.maxCompSize != sep.stats.maxCompSize ||
                testSep.stats.maxCompEdges != sep.stats.maxCompEdges) {

                cerr << "DIFFERENCE! assessed sep is different than it really is! " << endl;
                DEBUG(sep);
                DEBUG(testSep);
                exit(1);
            }
        }

        if( sepEval(sep,bestSep) ){
//            cerr << "\t\tFOUND BETTER SEPARATOR! i = " << i << endl;
//            cerr << "\t\t"; DEBUG(bestSep);
//            cerr << "\t\t"; DEBUG(sep);
//            ENDL(1);

            bestSep = sep;
            bestInd = i;

        }

    }

    VB inBorder(N,false);

    for( int i=0; i<=bestInd; i++ ){
        for( int p : mods[i] ){

            inBorder[p] = !inBorder[p];
        }
    }

    if(debug){
        DEBUG(bestInd);
        DEBUG(inBorder);
    }

//    Separator res(V, VI( ALL(inBorder) ) );

    Separator res(V, StandardUtils::toVI(inBorder) );
    res.createSeparatorStats();

    return res;

}



pair<VI, VI> ComponentExpansionSeparatorCreator::calculateExternalSizes(VVI &V, VI order) {
    int N = V.size();
    FAU fau(N);
    VI ord = order;

    VB inSep(N,true);
    Separator sep(V,ord );
    sep.stats.size = ord.size();

    VI edgesInComp(N,0);
    VB was(N,false);

    VI maxCompSize(N,0), maxCompEdges(N,0);

    while( ord.size() > 1 ){

        int p = ord.back();
        ord.pop_back();

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


        edgesInComp[ fau.Find(p) ] = edgesInCompNeighbors + neighInV;

        sep.stats.size--;
        sep.stats.maxCompSize = max( sep.stats.maxCompSize, fau.countNodesInComponent(p) );
        sep.stats.maxCompEdges = max( sep.stats.maxCompEdges, edgesInComp[ fau.Find(p) ] );

        maxCompSize[ord.size()-1] = sep.stats.maxCompSize;
        maxCompEdges[ord.size()-1] = sep.stats.maxCompEdges;
    }

    return { maxCompSize, maxCompEdges };

}

ComponentExpansionSeparatorCreator::CompBorder ComponentExpansionSeparatorCreator::calculateInternalSizes(VVI &V,
                                                                                                          VI order) {
    bool debug = false;


    int N = V.size();
    VI borderSizeNodes(N,0);
    VI inCompEdges(N,0);
    VI degOutside(N,0);

    VI maxInternalCompSize(N,0);
    VI maxInternalCompEdges(N,0);
    VI internalCompSize(N,0); // internalCompSize[ fau.Find(i) ] is the number of nodes in the same component as i in given stage
    VI internalCompEdges(N,0); // internalCompSize[ fau.Find(i) ] is the number of edges in the same component as i in given stage

    for( int i=0; i<N; i++ ) degOutside[i] = V[i].size();
    VB was(N,false);
    VB isInBorder(N,false);

    int inBorderNodes = 0;
    int inEdges = 0; // edges in component without border
    int currentMaxInternalSize = 0;
    int currentMaxInternalEdges = 0;
    FAU fau(N);

    VVI modifications; modifications.reserve(N);

    VI internalComps; internalComps.reserve(N);
    VB isInternalComp(N,false);

    for( int i=0; i<N; i++ ){
        int p = order[i];

        if(debug){
            cerr << "Adding " << p << " to border" << endl;
        }

        modifications.push_back( VI() );
        modifications.back().push_back(p);

        inBorderNodes++;
        was[p] = true;
        isInBorder[p] = true;

        auto removeFromBorder = [=,&modifications, &isInBorder,&currentMaxInternalSize, &currentMaxInternalEdges, &fau,
                                 &inBorderNodes, &was, &inEdges, &V, &internalCompSize, &internalCompEdges,    &internalComps, &isInternalComp](int d){
            inBorderNodes--;
            modifications.back().push_back(d);
            isInBorder[d] = false;
            if(debug){
                cerr << "removing " << d << " from border" << endl;
            }

//            unordered_set<int> internalComps;
            int additionalEdges = 0;
            for( int t : V[d] ){
                if( was[t] && !isInBorder[t] ){
                    inEdges++;

                    int tmp = fau.Find(t);
                    if( !isInternalComp[tmp] ){
                        internalComps.push_back(tmp);
                        isInternalComp[tmp] = true;
                    }
                    additionalEdges++;
                }
            }

            int totalEdges = additionalEdges;
            int totalSize = 1;
            for(int t : internalComps){
                totalEdges += internalCompEdges[ fau.Find(t) ];
                totalSize += internalCompSize[ fau.Find(t) ];
            }

            for(int t : internalComps) fau.Union(t,d);

            for(int t : internalComps) isInternalComp[t] = false;
            internalComps.clear();


            internalCompSize[fau.Find(d)] = totalSize;
            internalCompEdges[fau.Find(d)] = totalEdges;
            currentMaxInternalSize = max( currentMaxInternalSize, totalSize );
            currentMaxInternalEdges = max( currentMaxInternalEdges, totalEdges );

            if(debug){
                cerr << "internalCompEdges in component containing " << d << ": " << internalCompEdges[fau.Find(d)] << endl;
                cerr << "internalCompSize in component containing " << d << ": " << internalCompSize[fau.Find(d)] << endl;
                DEBUG(inEdges);
                DEBUG(inBorderNodes);
                DEBUG(currentMaxInternalSize);
                DEBUG(currentMaxInternalEdges);
                ENDL(1);
            }
        };

        for( int d : V[p] ){
            if( was[d] ){
                degOutside[d]--;
                if( degOutside[d] == 0 ){
                    removeFromBorder(d);
                }
            }else{
                degOutside[d]--;
            }
        }



        bool allNeighborsAdded = true;
        for( int d : V[p] ) if( !was[d] ) allNeighborsAdded = false;

        if( allNeighborsAdded ){
            removeFromBorder(p);
        }

        borderSizeNodes[i] = inBorderNodes;
        inCompEdges[i] = inEdges;

        maxInternalCompSize[i] = currentMaxInternalSize;
        maxInternalCompEdges[i] = currentMaxInternalEdges;

    }


    CompBorder cb;
//    cb.borderSize = borderSizeNodes;
//    cb.compEdges = inCompEdges;
//    cb.modifications = modifications;
//    cb.maxInternalCompSize = maxInternalCompSize;
//    cb.maxInternalCompEdges = maxInternalCompEdges;

    // #TEST testing swapping instead of rewriting arrays
    swap( cb.borderSize, borderSizeNodes);
    swap( cb.compEdges, inCompEdges);
    swap( cb.modifications, modifications);
    swap( cb.maxInternalCompSize, maxInternalCompSize);
    swap( cb.maxInternalCompEdges, maxInternalCompEdges);

    return cb;
}



VVI ComponentExpansionSeparatorCreator::getExpansionOrdersForNodes(VVI &V, VI sources) {
    VVI res;

    if( ORDERS_TO_CREATE & TIGHTEST_NODE_ORDER ){
        VI tightnessOrder = getTightestNeighborOrder(V, sources );
        res.push_back( tightnessOrder );
    }

    if( ORDERS_TO_CREATE & BFS_ORDER ){
        VI bfsOrder = getBFSOrder( V, sources );
        res.push_back( bfsOrder );
    }

    if( ORDERS_TO_CREATE & BFS_VC_ORDER ){
        VI bfsVCorder = getBFSLayerVCOrder( V, sources );
        res.push_back( bfsVCorder );
    }

    if( ORDERS_TO_CREATE & LEAST_NEIGHBORS_ORDER ){
        VI leastExtNeighOrder = getLeastExteriorNeighborhoodOrder(V,sources);
        res.push_back(leastExtNeighOrder);
    }


    // adding optimized orders
    int T = res.size();
    for( int i=0; i<T; i++ ){
        res.push_back( optimizeOrderByTraversingFirstSmallestComponents( V,res[i] ) );
    }


//    while( res.size() > T ) res.erase( res.begin() ); // keeping only optimized orders

    return res;
}



Separator ComponentExpansionSeparatorCreator::getIterativeSepForStartingNodes(VVI &V, VI sources) {
    cerr << "Iterative expansion" << endl;

    VI tightnessOrder = getTightestNeighborOrder(V, sources );
    auto sep = getBestSeparatorForExpansionOrder( V, tightnessOrder );
    sep.updatePointers(V);

//    cerr << "After tightness order: " << sep << endl;
//
//    VI leastExtNeighOrder = getLeastExteriorNeighborhoodOrder(V,sep.nodes);
//    sep = getBestSeparatorForExpansionOrder( V, leastExtNeighOrder );
//    sep.updatePointers(V);
//
//    cerr << "After least neighbors order: " << sep << endl;
//
//    VI bfsOrder = getBFSOrder( V, sep.nodes );
//    sep = getBestSeparatorForExpansionOrder( V, bfsOrder );
//    sep.updatePointers(V);
//
//    cerr << "After bfs order: " << sep << endl;

//    for(int i=0; i<7; i++) {
    while(1){
        if( Pace20Params::tle ) break;

        VI toOpt = optimizeOrderByTraversingFirstSmallestComponents(V, getTightestNeighborOrder(V, sep.nodes));
        auto newsep = getBestSeparatorForExpansionOrder(V, toOpt);
        newsep.updatePointers(V);
        cerr << "After optimized tightness order: " << newsep << endl;
        if( sepEval(newsep,sep) ) swap(sep,newsep); else break;


        VI lenoOpt = optimizeOrderByTraversingFirstSmallestComponents(V, getLeastExteriorNeighborhoodOrder(V, sep.nodes));
        newsep = getBestSeparatorForExpansionOrder(V, lenoOpt);
        newsep.updatePointers(V);
        cerr << "After optimized least neighbor order: " << newsep << endl;
        if( sepEval(newsep,sep) ) swap(sep,newsep); else break;


        VI bfsoOpt = optimizeOrderByTraversingFirstSmallestComponents(V, getBFSLayerVCOrder(V, sep.nodes));
        newsep = getBestSeparatorForExpansionOrder(V, bfsoOpt);
        newsep.updatePointers(V);
        cerr << "After optimized bfsLayerVC order: " << newsep << endl;
        if( sepEval(newsep,sep) ) swap(sep,newsep);

    }

    ENDL(1);

    return sep;
}





VVI ComponentExpansionSeparatorCreator::getExpansionOrdersForNode(VVI &V, int v) {
    return getExpansionOrdersForNodes( V, {v} );
}

VI ComponentExpansionSeparatorCreator::getBFSOrder(VVI &V, VI sources) {
    VVI layers = BFS::getBfsLayers( V, sources );
    VI order;
    for( VI& v : layers ) order.insert( order.end(), ALL(v) );
    return order;
}

VI ComponentExpansionSeparatorCreator::getBFSLayerVCOrder(VVI &V, VI sources) {
    VVI layers = BFS::getBfsLayers(V, sources);

//    DEBUG(layers);

    if( layers.size() == 1 ) return layers[0];

    int L = layers.size();

    VI inLayer(V.size()); // inLayer[d] is index of layer in which node d is
    for( int i=0; i<L; i++ ){
        for(int d : layers[i]) inLayer[d] = i;
    }

    VVI vertexCovers;

    for( int i=1; i<L; i++ ){

        VPII interLayerEdges;
        for( int d : layers[i-1] ){
            for( int t : V[d] ){
                if( inLayer[t] == i ){
                    interLayerEdges.emplace_back(d,t);
                }
            }
        }

        InducedGraph gr = GraphInducer::induce(V,interLayerEdges);

        VB bipartition(gr.V.size(),true);
        for( int j=0; j<gr.nodes.size(); j++ ){
            if( inLayer[ gr.nodes[j] ] == i-1 ) bipartition[j] = false;
        }

        auto vc = BipartiteGraphVertexCover::getVertexCoverOfBipartiteGraph(gr.V,bipartition);
        for(int& d : vc) d = gr.nodes[d]; // remapping back to graph V

        vertexCovers.push_back(vc);
    }

//    DEBUG(vertexCovers);

    VB inVC(V.size(),false);
    for( int i=0; i<vertexCovers.size(); i++ ){
        for( int d : vertexCovers[i] ){
            if( inLayer[d] == i+1 ) inVC[d] = true;
        }
    }

    VI order(V.size(),0);
    iota(ALL(order),0);

    sort( ALL(order), [&inVC, &inLayer ]( int a, int b ){
        if( inLayer[a] != inLayer[b] ) return inLayer[a] < inLayer[b];
        else if( inVC[a] != inVC[b] ) return inVC[a] == true;
        else return a < b;
    } );

//    DEBUG(order);

    return order;

}

VI ComponentExpansionSeparatorCreator::getTightestNeighborOrder(VVI &V, VI sources) {
    int N = V.size();

    VI order = sources;
    VB was(N,false);

    for(int d : sources) was[d] = true;

    auto comp = [](int a, int b){ return a > b; };
    Heap<int> heap( comp );

    for(int i=0; i<N; i++){
//        int w = (GraphUtils::containsEdge( V, i,v ) ? 1 : 0);
//        if(was[i]) continue;
        int w = 0;
        for(int d : V[i]){
            if( was[d] ) w++;
        }

        if( was[i] ) heap.push_back(0);
        else heap.push_back(w);
    }

//    heap.removeFromHeap(v);
    for(int p : order) heap.removeFromHeap(p);

//    cerr << "starting in " << order << endl;

    while( !heap.empty() ){

        int p = heap.top().indInItems;
//        int tightness = heap.top().val;
        heap.extract_min();

//        cerr << "adding " << p << " to order with tightness " << tightness << endl;
        order.push_back(p);

        was[p] = true;
        for(int d : V[p]){
            if(!was[d]){
                heap.set( d, heap[d]+1 );
            }
        }
    }

//    DEBUG(order);

    return order;


}


VI ComponentExpansionSeparatorCreator::getLeastExteriorNeighborhoodOrder(VVI &V, VI sources) {
    int N = V.size();
    int INF = Constants::INF;

    VI order = sources;
    VB was(N,false);

    for(int d : sources) was[d] = true;

    auto comp = [](int a, int b){ return a < b; };
    Heap<int> heap(comp);

    for(int i=0; i<N; i++){
//        int w = (GraphUtils::containsEdge( V, i,v ) ? 1 : 0);
//        if(was[i]) continue;
        int w = 0;
        for(int d : V[i]){
            if( !was[d] ) w++;
        }

        if( was[i] ) heap.push_back(w);
        else heap.push_back(INF);
    }

//    for(int i=0; i<N; i++) if( !was[i] ) heap.removeFromHeap(i);


    VI toAdd;

    VI inBorder(N,false);
    for(int d : sources) inBorder[d] = true;

    while( !heap.empty() ){

        int p;
        int val;

        if( toAdd.empty() ){
            p = heap.top().indInItems;
            val = heap.top().val;
            heap.extract_min();

//            cerr << "\tremoving " << p << " from border and adding " << val << " neighbors to border" << endl;
            for( int d : V[p] ){
                if( !was[d] ){
                    toAdd.push_back(d);
//                    cerr << "\t\tadding " << d << endl;
                }
            }

            was[p] = true;
            inBorder[p] = false;

//            ENDL(1);

            continue;
        }
        else {
            p = toAdd.back();
            toAdd.pop_back();

            int neighInExt = 0;
            for (int d : V[p]) if (!was[d]) neighInExt++;

//            cerr << "adding " << p << " to border and setting neighbors to " << neighInExt << endl;
            heap.set(p, neighInExt);

            order.push_back(p);

            was[p] = true;
            inBorder[p] = true;

            for (int d : V[p]) {
                if (was[d] && inBorder[d] ) {
                    heap.set(d, heap[d] - 1);
                }
            }

//            ENDL(1);
        }
    }

//    DEBUG(order);

    return order;


}



VI ComponentExpansionSeparatorCreator::optimizeOrderByTraversingFirstSmallestComponents(VVI &V, VI order){
    bool debug = false;

    int N = V.size();
    int firstInOrder = order[0];

   /* if( order.size() != N ){
        DEBUG(order.size());
        DEBUG( ConnectedComponents::getConnectedComponents(V).size() );
    }*/
    assert( order.size() == N );

    VVI traverseGraph(N);

    VI edgesInComp(N,0);
    VI firstInComp(N,0); // first in comp[ fau.Find(i) ] is the first node in @{order} in component that contains i, in given phase of the algorithm
    VI inOrder(N,0);
    for( int i=0; i<order.size(); i++ ) inOrder[ order[i] ] = i;

    FAU fau(N);

    if(debug)DEBUG(order);

    while( !order.empty() ){
        int p = order.back();
        order.pop_back();

        if(debug)cerr << "Adding " << p << endl;

        set<int> neigh;
        int edges = 0;
        for( int d : V[p] ){
            if( inOrder[d] > inOrder[p] ){
                int t = fau.Find(d);
                neigh.insert( t );
                edges++;
            }
        }

        if(debug) {cerr << "neigh: "; for(int t : neigh) cerr << t << " "; cerr << endl;}

        for( int d : neigh ) edges += edgesInComp[d];

        if(debug)DEBUG(edges);

        VI neighOrder(ALL(neigh));

        sort( ALL(neighOrder), [&edgesInComp]( int a, int b ){ return edgesInComp[a] < edgesInComp[b]; } );

        if(debug){
            DEBUG(neighOrder);
            cerr << "first in orders: "; for(int t : neighOrder) cerr << firstInComp[t] << " "; cerr << endl;
        }

        for( int d : neighOrder ) traverseGraph[p].push_back( firstInComp[d] );

        for( int d : neighOrder ) fau.Union(p,d);

        firstInComp[ fau.Find(p) ] = p;
        edgesInComp[ fau.Find(p) ] = edges;

        if(debug)ENDL(1);
    }

    if(debug) DEBUG(traverseGraph);

    VI newOrder;

    function< void(int) > restoreNewOrder = [&traverseGraph, &newOrder, &restoreNewOrder](int v){
        for( int d : traverseGraph[v] ){
            newOrder.push_back(d);
            restoreNewOrder(d);
        }
    };

//    newOrder.push_back( order[0] );
//    restoreNewOrder( order[0] );
    newOrder.push_back( firstInOrder );
    restoreNewOrder( firstInOrder );

    if(debug){
        DEBUG(newOrder);
        exit(1);
    }

    return newOrder;

}





void ComponentExpansionSeparatorCreator::test() {

    bool measureTime = false;


    if( !measureTime ) {
        VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
        DEBUG(V.size());
        DEBUG( GraphUtils::countEdges(V) );

        for(int i=0 ;i<V.size(); i++) random_shuffle(ALL(V[i]));

        VI order = VI(V.size());
        iota(ALL(order),0);
        swap( order[1], order[2] );

        ComponentExpansionSeparatorCreator ceCr( SeparatorEvaluators::estimatedDepthTreeEdge );

        order = ceCr.getLeastExteriorNeighborhoodOrder( V, {0} );
        order = ceCr.optimizeOrderByTraversingFirstSmallestComponents(V,order);
        DEBUG(order); exit(1);



        order = {12, 1, 0, 2, 3, 13, 7, 17, 11, 14, 15, 16, 9, 10, 8, 4, 5, 6, 19, 20, 18};
//        ceCr.getBestSeparatorForExpansionOrder( V, order );
        Separator res = ceCr.getBestSeparatorForExpansionOrder( V, ceCr.optimizeOrderByTraversingFirstSmallestComponents(V,order) );
        DEBUG(res);

//        Separator sep(V, {12} ); sep.createSeparatorStats();
//        DEBUG( sep );
    }
    else {

        TimeMeasurer::startMeasurement("ComponentExpansion");

        int T = 1'000;
        int cnt = T;
        while (T--) {
            cerr << "\r#" << cnt - T << " / " << cnt << flush;
            int N = 200;
            int M = 5*N;
            VVI V = GraphGenerator::getRandomGraph(N, M);
            if (ConnectedComponents::getConnectedComponents(V).size() > 1) continue;
            ComponentExpansionSeparatorCreator ceCr(SeparatorEvaluators::estimatedDepthTreeEdge);
            ceCr.createSeparators(V, 5);

        }

        cerr << "TEST PASSED!" << endl;
        TimeMeasurer::stopMeasurement("ComponentExpansion");

        TimeMeasurer::writeAllMeasurements();
    }

    exit(1);
}







