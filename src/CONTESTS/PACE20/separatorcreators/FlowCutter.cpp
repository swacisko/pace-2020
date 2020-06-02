/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/separatorcreators/FlowCutter.h>
#include <CONTESTS/PACE20/separatorcreators/ComponentExpansionSeparatorCreator.h>
#include <graphs/GraphUtils.h>
#include <graphs/GraphInducer.h>
#include <graphs/GraphReader.h>
#include <utils/StandardUtils.h>
#include <graphs/graphtraversals/BFS.h>
#include <combinatorics/CombinatoricUtils.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <CONTESTS/PACE20/separatorminimizers/TotalMinimizer.h>
#include <CONTESTS/PACE20/DTKernelizer.h>
#include <graphs/landmarks/LandmarkCreator.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <utils/TimeMeasurer.h>
#include "graphs/VertexCover/BipartiteGraphVertexCover.h"


FlowCutter::FlowCutter(SeparatorEvaluator eval) {
    sepEval = eval;
}

VI FlowCutter::getVertexCoverOfSTBarrier(VVI& V) {
    bool debug = false;

    VI src, trg;
    for( int s : sources ){
        if( s < V.size() ){
            for( int d : V[s] ){
                if( isTarget[d] ){
                    src.push_back(s);
                    break;
                }
            }
        }
    }
    for( int t : targets ){
        if( t < V.size() ){
            for( int d : V[t] ){
                if( isSource[d] ){
                    trg.push_back(t);
                    break;
                }
            }
        }
    }

    if(debug){
        DEBUG(src);
        DEBUG(trg);
    }

    VI nodes = src;
    StandardUtils::append( nodes,trg );

    InducedGraph g = GraphInducer::induce( V, nodes );
    VB bipartition(g.V.size(),false);
    for( int s : src ) bipartition[ g.perm[s] ] = true;
    VI vc =  BipartiteGraphVertexCover::getVertexCoverOfBipartiteGraph(g.V,bipartition);
    for(int& d : vc) d = g.nodes[d];

    if(debug) DEBUG(vc);

//    DEBUG(src.size());
//    DEBUG(trg.size());
//    DEBUG(vc.size());
//    DEBUG(uf->flowValue());

    return vc;
}


vector<Separator> FlowCutter::getSeparatorsForSourcesAndTargets(VVI &V, VVI &expV, VI S, VI T) {
    bool debug = false;

    vector<Separator> seps;


    getFlowCutterExpansionOrder( expV, S, T );

    if( Pace20Params::tle ) return seps;



     {
        if(debug) cerr << endl << "Using VC barrier:" << endl;
        VI vc = getVertexCoverOfSTBarrier(V);

        if( !vc.empty() ) {
            Separator res(V, vc);
            res.createSeparatorStats();
            if (debug) {
                DEBUG(res);
            }

            seps.push_back(res);
        }
    }

    {

        if(debug) cerr << "Using expansion order:" << endl;


        VI orderExpV = expansionOrder;

        VB inExpVOrder = StandardUtils::toVB(expV.size(), orderExpV);
        for (int i = 0; i < expV.size(); i++) if (!inExpVOrder[i]) orderExpV.push_back(i);


        VI orderV;
        VB inOrder(V.size(), false);
        for (int p : orderExpV) {
            if (p >= V.size()) p -= V.size();
            if (!inOrder[p]) {
                inOrder[p] = true;
                orderV.push_back(p);
            }
        }


        ComponentExpansionSeparatorCreator ceCr(sepEval);

        VVI ordersToCheck = {orderV}; // orderV
        reverse(ALL(orderV));
        ordersToCheck.push_back(orderV); // reversed orderV


        { // some other adding orders
            VI src, trg;
            for( int s : sources ){
                if( s < V.size() ){
                    for( int d : V[s] ){
                        if( isTarget[d] ){
                            src.push_back(s);
                            break;
                        }
                    }
                }
            }
            for( int t : targets ){
                if( t < V.size() ){
                    for( int d : V[t] ){
                        if( isSource[d] ){
                            trg.push_back(t);
                            break;
                        }
                    }
                }
            }

            // now we add order: source-adding-order + reversed-target-adding-order
            orderV = src;
            reverse(ALL(trg));
            StandardUtils::append(orderV,trg);
            VB w = StandardUtils::toVB(V.size(),orderV);
            for( int i=0; i<V.size(); i++ ) if( !w[i] ) orderV.push_back(i);
            ordersToCheck.push_back(orderV);
            reverse(ALL(trg));


            // and now target-adding-order + reverse-source-adding-order
            orderV = trg;
            reverse(ALL(src));
            StandardUtils::append(orderV,src);
            w = StandardUtils::toVB(V.size(),orderV);
            for( int i=0; i<V.size(); i++ ) if( !w[i] ) orderV.push_back(i);
            ordersToCheck.push_back(orderV);
            reverse(ALL(src));


            // here we add order: source-adding-order + target-adding-order
            orderV = src;
            StandardUtils::append(orderV,trg);
            w = StandardUtils::toVB(V.size(),orderV);
            for( int i=0; i<V.size(); i++ ) if( !w[i] ) orderV.push_back(i);
            ordersToCheck.push_back(orderV);

            // here we add order target-adding-order + source-adding-order
            orderV = trg;
            StandardUtils::append(orderV,src);
            w = StandardUtils::toVB(V.size(),orderV);
            for( int i=0; i<V.size(); i++ ) if( !w[i] ) orderV.push_back(i);
            ordersToCheck.push_back(orderV);

        }

        // orders:
        // orderV, reversedV, src+revTrg, src+trg, trg+src
        auto getOrderName = [](int cnt){
            if( cnt == 0 ) return "orderV";
            else if(cnt == 1) return "reversedORderV";
            else if(cnt == 2) return "src+revTrg";
            else if(cnt == 3) return "trg+revSrc";
            else if(cnt == 4) return "src+trg";
            else if(cnt == 5) return "trg+src";
        };

        for( int o=0; o<ordersToCheck.size(); o++ ) {
            if( Pace20Params::tle ) return seps;

            VI ord = ordersToCheck[o];
            if(debug) cerr << "checking order #" << getOrderName(o) << endl;

            VI optOrderV = ceCr.optimizeOrderByTraversingFirstSmallestComponents(V, ord);
            Separator res = ceCr.getBestSeparatorForExpansionOrder(V, optOrderV);
            res.updatePointers(V);

            if (debug) {
                DEBUG(res);
            }

            /*TotalMinimizer totMin(sepEval);
            res = totMin.minimizeSeparator(res);

            if (debug) {
                DEBUG(res);
            }*/

            seps.push_back(res);
        }


    }

    if (debug) {
        ENDL(5);
    }

    return seps;
}




vector<Separator> FlowCutter::createSeparators(VVI &V, int repeats) {
    bool debug = false;

    bestResultSoFarSeparatorNodesSize = 2 * V.size();

    VVI expV = getExpansionGraph(V);
    usingExpansionGraph = false;

    // random nodes for sources and targets
    VI subset = CombinatoricUtils::getRandomSubset( V.size(),  min( (int)V.size() - ( (int)V.size() & 1 ), 2*repeats ) );
    VPII sourceTargets;
    for( int i=0; i<subset.size(); i+=2 ) sourceTargets.emplace_back( subset[i], subset[i+1] );



//    cerr << "Creating landmarks" << endl;
    LandmarkCreator lcr;
    int LANDMARKS = min( (int)V.size()-1, max( 50, 2*repeats )  );
    VI landmarks = lcr.getLandmarks( V, 0, LANDMARKS, 0 );
    random_shuffle(ALL(landmarks));
    sourceTargets.clear();
    for( int i=1; i< min( 2*repeats, LANDMARKS ) ; i+=2 ) sourceTargets.emplace_back( landmarks[i-1], landmarks[i] );
//    cerr << "\tlandmarks created" << endl;



    vector<Separator> seps;
    for( int i=0; i<sourceTargets.size(); i++ ){
        if( Pace20Params::tle ) break;

        PII x = sourceTargets[i];
        int s = x.first;
        int t = x.second;

        VI sources = {s};
        VI targets = { (int)V.size()+t};

//        { // #TEST
//            s = rand() % V.size();
//            t = rand() % V.size();
//            sources = {s};
//            targets = {t + (int)V.size()};
//        }


        auto iterSeps = getSeparatorsForSourcesAndTargets( V, expV, sources, targets );
        StandardUtils::append( seps, iterSeps );

        auto bestSepIt = min_element( ALL(seps), sepEval );
        bestResultSoFarSeparatorNodesSize = bestSepIt->stats.size;

        if( debug ) DEBUG(bestResultSoFarSeparatorNodesSize);

    }

    for(auto& sp : seps) sp.updatePointers(V);

//    exit(1);

    return seps;

}

VVI FlowCutter::getExpansionGraph(VVI &V) {
    int N = V.size();
    VVI expV(2*N);
    for( int i=0; i<N; i++ ){
        expV[N+i].push_back(i);
        for(int d : V[i]) expV[i].push_back(N+d);
    }
    return expV;
}


VI FlowCutter::getFlowCutterExpansionOrder(VVI &V, VI src, VI ends) {
    bool debug = false;

    this->V = V;
    N = V.size();
    revV = GraphUtils::reverseGraph(V);

    sources = src;
    isSource = StandardUtils::toVB(N,sources);
    sourceBorder = sources;

    targets = ends;
    isTarget = StandardUtils::toVB(N,targets);
    targetBorder = targets;

    isSourceReachable = StandardUtils::toVB( N,sources );
    isTargetReachable = StandardUtils::toVB( N,targets );

    dstFromSources = dstFromTargets = VI(N,0);
    calculateDistances();

    inExpansionOrder = VB(N,false);
    expansionOrder.clear();
    addToExpansionOrder(sources);
    addToExpansionOrder(targets);

    canBeAugmented = false;

    if( uf != nullptr ) delete uf;
//    uf = new UnitFlow(V);
    uf = new UnitFlow2(V);
//    uf->setUseDfsAugmentation(true); // seems that dfs augmentation usually works a tiny bit faster than layer bfs block flow creation

    for( int s : sources ) uf->addSource(s);
    for(int t : targets) uf->addTarget(t);
    uf->augmentFlow();

    fGrow();
    bGrow();


    if(debug){
        uf->writeSaturatedEdges();
        DEBUG(sources);
        DEBUG(targets);
        DEBUG(sourceReachable.size());
        DEBUG(sourceReachable);
        DEBUG(targetReachable.size());
        DEBUG(targetReachable);
        DEBUG(sourceBorder);
        DEBUG(targetBorder);
        ENDL(2);
    }


    int lastPNSourceSize = -1;
    int lastPNTargetSize = -1;

    bool canExpandSources = true;
    bool canExpandTargets = true;

//    cerr << "before" << endl;
//    DEBUG( sources.size() );
//    DEBUG( targets.size() );

    int augmentingTimes = 0;

    auto augmentStep = [=, &augmentingTimes,&lastPNSourceSize, &lastPNTargetSize, &canExpandSources, &canExpandTargets](){
        if( Pace20Params::tle ) return;

        augmentingTimes++;

//        cerr << "AUGMENTING FLOW, value: " << uf->flowValue() << ", sources.size() + targets.size() = "
//             << sources.size() + targets.size() << " / " << V.size() << " = expV.size()" << endl;

        if(debug){

            DEBUG(sources);
            DEBUG(targets);
        }


//        for( int i=0; i<N; i++ ) if( isSource[i] && isTarget[i] ) ERROR( "", "node both in sources and targets" ); // LINE JUST FOR TESTING!! REMOVE IT LATER


        canBeAugmented = false;
        uf->augmentFlow();

        if( debug ) cerr << "Flow augmented" << endl;

        /*sourceReachable = sources;
        targetReachable = targets;

        isSource = StandardUtils::toVB(N,sources);
        isTarget = StandardUtils::toVB(N,targets);

        isSourceReachable = StandardUtils::toVB( N,sources );
        isTargetReachable = StandardUtils::toVB( N,targets );*/


        for( int s : sourceReachable ) isSourceReachable[s] = false; sourceReachable = sources;
        for( int s : sources ) isSourceReachable[s] = true;

        for(int t : targetReachable ) isTargetReachable[t] = false; targetReachable = targets;
        for(int t : targets ) isTargetReachable[t] = true;



        fGrow();

//        for( int i=0; i<N; i++ ) if( isSourceReachable[i] && isTargetReachable[i] ) ERROR( "", "node after augmenting is both source and target reachable" ); // LINE JUST FOR TESTING!! REMOVE IT LATER

        if(debug) DEBUG(sourceReachable);



        bGrow();

//        for( int i=0; i<N; i++ ) if( isSourceReachable[i] && isTargetReachable[i] ) ERROR( "", "node after augmenting is both source and target reachable" ); // LINE JUST FOR TESTING!! REMOVE IT LATER

        if(debug) DEBUG(targetReachable);



        lastPNSourceSize = sources.size();
        lastPNTargetSize = targets.size();

        if(debug){
            cerr << "flow augmented, now:" << endl;
            DEBUG(sources); DEBUG(targets);
            DEBUG(sourceReachable.size()); DEBUG(sourceReachable);
            DEBUG(targetReachable.size()); DEBUG(targetReachable);
            DEBUG(sourceBorder); DEBUG(targetBorder);
            uf->writeSaturatedEdges(); ENDL(5);
        }

//        calculateDistances();

    };

    auto expandSourcesStep = [=, &lastPNSourceSize, &lastPNTargetSize,&canExpandSources, &canExpandTargets](){
        if( Pace20Params::tle ) return;

        if(debug){
            cerr << "Expanding sources" << endl;
        }
        // adding to sources all nodes that are source-reachable but are not yet in sources
        for( int i=lastPNSourceSize; i<sourceReachable.size(); i++ ){
            int p = sourceReachable[i];

            if(debug){
                cerr << "expanding sources by node " << p << endl;
            }

            addToSources( {p} );
        }
        if(debug){
            DEBUG(sources);
            DEBUG(targets);
            DEBUG(sourceReachable.size());
            DEBUG(sourceReachable);
            DEBUG(targetReachable.size());
            DEBUG(targetReachable);
            DEBUG(sourceBorder);
            DEBUG(targetBorder);
        }

        int pN = getPiercingSourceNode();

        if( isTarget[pN] ){
            canExpandSources = false;
//            continue;
            return;
        }

        addToSources( {pN} );
        lastPNSourceSize = sourceReachable.size();

        if(debug){
            cerr << "Forward-growing source-reachable nodes starting from node " << pN << endl;
        }

        fGrow(pN);

        if(debug){
            cerr << "after growing: " << endl;
            DEBUG(sourceReachable.size());
            DEBUG(sourceReachable);
            DEBUG(sourceBorder);
            DEBUG(targetBorder);
            ENDL(3);
        }
    };

    auto expandTargetsStep = [=, &lastPNSourceSize, &lastPNTargetSize,&canExpandSources, &canExpandTargets](){
        if( Pace20Params::tle ) return;

        if(debug){
            cerr << "Expanding targets" << endl;
        }

        // adding to targets all nodes that are target-reachable, but are not yet in targets
        for( int i=lastPNTargetSize; i < targetReachable.size(); i++ ){
            int p = targetReachable[i];

            if(debug){
                cerr << "axpanding targets by node " << p << endl;
            }

            addToTargets({p});
        }

        if(debug){
            DEBUG(sources);
            DEBUG(targets);
            DEBUG(sourceReachable.size());
            DEBUG(sourceReachable);
            DEBUG(targetReachable.size());
            DEBUG(targetReachable);
            DEBUG(sourceBorder);
            DEBUG(targetBorder);
        }

        int pN = getPiercingTargetNode();

        if( isSource[pN] ){
            canExpandTargets = false;
//            continue;
            return;
        }

        addToTargets({pN});
        lastPNTargetSize = targetReachable.size();

        if(debug){
            cerr << "Backward-growing target-reachable nodes starting from node " << pN << endl;
        }

        bGrow(pN);

        if(debug){
            cerr << "after growing: " << endl;
            DEBUG(targetReachable.size());
            DEBUG(targetReachable);
            DEBUG(sourceBorder);
            DEBUG(targetBorder);
            ENDL(3);
        }
    };


    while( sources.size() + targets.size() < N ){
        if( Pace20Params::tle ) break;

        if( uf->flowValue() > max( (double)15, bestResultSoFarSeparatorNodesSize * maximalDeviationFromBestResultFactor) ) break;


        if( canBeAugmented ){

            augmentStep();

        }else{
//            for( int i=0; i<N; i++ ) if( isSource[i] && isTarget[i] ) ERROR( "", "node both in sources and targets" ); // LINE JUST FOR TESTING!! REMOVE IT LATER

            if( !canExpandSources && !canExpandTargets ) break; // all sources and targets already form a separator, each piercing node is either a source or target




            bool expandSources;
            if( !canExpandTargets ) expandSources = true;
            else if( !canExpandSources ) expandSources = false;
//            else expandSources = ( sourceReachable.size() <= targetReachable.size() ); // original version
            else{
                vector<PiercingNode> pNodes = getAllPiercingNodes();
                VI augSrc, nonaugSrc, augTrg, nonaugTrg;
                for( auto pn : pNodes){
                    if( pn.sourcePN && pn.augmenting ) augSrc.push_back(pn.v);
                    else if( pn.sourcePN ) nonaugSrc.push_back(pn.v);
                    else if( pn.augmenting ) augTrg.push_back(pn.v);
                    else nonaugTrg.push_back(pn.v);
                }

                const double SCALE = balanceParameter;
                const double SCALE2 = 0.1;

                bool unbalance = ( sources.size() + targets.size() > SCALE2 * V.size() );

                // if there are only nonaugmenting source nodes and
                if( unbalance && !nonaugSrc.empty() && nonaugTrg.empty() && sourceReachable.size() < SCALE *  targetReachable.size() ) canExpandSources = true;

                // if there are only nonaugmenting target nodes, and target reachable nodes are not too large
                else if( unbalance && nonaugSrc.empty() && !nonaugTrg.empty() && targetReachable.size() < SCALE * sourceReachable.size() ) canExpandSources = false;

                // if all piercing nodes are augmenting or all are nonaugmenting, then we expand the smaller component
                else expandSources = ( sourceReachable.size() <= targetReachable.size() ); // original version
            }



            if( expandSources ){ // original version

                expandSourcesStep();

            }else{
                expandTargetsStep();

            }


        }
    }

    if(debug){
        cerr << "Finished!" << endl;
        DEBUG(sources);
        DEBUG(targets);
        DEBUG(expansionOrder);
    }



    return expansionOrder;


}

void FlowCutter::fGrow(int s) {
    bool debug = false;

    VI reachable;
    if( s == -1 ) reachable = uf->getSourceReachableNodes();
    else reachable = uf->getSourceReachableNodes( s, isSourceReachable );

    if(debug){
        cerr << "fGrow  "; DEBUG(reachable);
    }

    sourceReachable.insert( sourceReachable.end(), ALL(reachable) );
    for(int d : reachable){
        isSourceReachable[d] = true;
        if( isTargetReachable[d] ) canBeAugmented = true;
    }

}

void FlowCutter::bGrow(int t) {
    bool debug = false;

    VI reachable;
    if( t == -1 ) reachable = uf->getTargetReachableNodes();
    else reachable = uf->getTargetReachableNodes( t, isTargetReachable );

    if(debug){
        cerr << "bGrow  "; DEBUG(reachable);
    }

    targetReachable.insert( targetReachable.end(), ALL(reachable) );
    for(int d : reachable){
        isTargetReachable[d] = true;
        if( isSourceReachable[d] ) canBeAugmented = true;
    }
}

int FlowCutter::getDistanceValueFrom(VI &dst1, VI &dst2, int p) {
    if( PN_STRATEGY == MAXIMIZE_HIPERBOLIC_DISTANCE ) return dst1[p] - dst2[p];
    else if( PN_STRATEGY == MAXIMIZE_DISTANCE ) return dst1[p];

    cerr << "PN_STRATEGY unknown, terminating" << endl;
    exit(1);
}


int FlowCutter::getPiercingSourceNode() {
    bool debug = false;
    if(debug){
        cerr << "Getting piercing source node" << endl;
    }

    VI piercingSourceNodes;
    for( int i = (int)sourceBorder.size()-1; i>=0; i-- ){
        int p = sourceBorder[i];

        bool hasOutNeigh = false;
        for( int d : V[p] ){
            if( !isSource[d] ){
                hasOutNeigh = true;
                piercingSourceNodes.push_back(d);
            }
        }

        if( !hasOutNeigh ){ // if p is inside source, i can remove it from border
            swap( sourceBorder[i], sourceBorder.back() );
            sourceBorder.pop_back();

            uf->removeSource(p);
        }
    }

    if(debug){
        DEBUG(sourceBorder);
        DEBUG(piercingSourceNodes);
        cerr << "distances of piercing source nodes from targets: "; for( int p : piercingSourceNodes ) cerr << dstFromTargets[p] << " "; cerr << endl;
    }


    VI augmenting, nonAugmenting;

    if(debug){
        cerr << "returning non-augmenting piercing source node or any other if all are non-augmenting" << endl;
    }

    int pN = -1;
    for( int d : piercingSourceNodes ){
        if( isTargetReachable[d] == false ){
            pN = d;
            nonAugmenting.push_back(d);
        }
        else{
            augmenting.push_back(d);
        }
    }

    if(debug){
        DEBUG(augmenting);
        DEBUG(nonAugmenting);
    }

    auto comp = [=](int a, int b){
        if( isTarget[a] != isTarget[b] ) return (isTarget[a] == true); // if a is target and b is not, then  b is LARGER than a, that is better, since we maximize values
        else if( getDistanceValueFrom( dstFromTargets, dstFromSources,a ) != getDistanceValueFrom( dstFromTargets, dstFromSources,a ) ){
            return getDistanceValueFrom( dstFromTargets, dstFromSources,a ) < getDistanceValueFrom( dstFromTargets, dstFromSources,a );
        }else if( dstFromTargets[a] != dstFromTargets[b] ) return dstFromTargets[a] < dstFromTargets[b];
        else return a < b;
    };

    auto getBestNode = [=, &comp]( VI& nodes ){
        if( nodes.empty() ) return -1;
        return *max_element( ALL(nodes), comp );
    };

    pN = getBestNode(nonAugmenting);

    string msg = "non-augmenting";
    if( pN == -1 ){
        pN = getBestNode(augmenting);
        msg = "augmenting";
    }

    if( isTarget[pN] ){
        if(debug) cerr << "AllPiercingNodes are  in target!" << endl;

    }
    if(debug) cerr << "returning " << msg << " " << pN << endl;

    return pN;
}

int FlowCutter::getPiercingTargetNode() {
    bool debug = false;
    if(debug) cerr << "Getting piercing target node" << endl;

    VI piercingTargetNodes;

    for( int i = (int)targetBorder.size()-1; i>=0; i-- ){
        int t = targetBorder[i];

        bool hasOutNeigh = false;
        for( int d : revV[t] ){
            if( !isTarget[d] ){
                hasOutNeigh = true;
                piercingTargetNodes.push_back(d);
            }
        }

        if( !hasOutNeigh ){ // if p is inside source, i can remove it from border
            swap( targetBorder[i], targetBorder.back() );
            targetBorder.pop_back();

            uf->removeTarget(t);
        }
    }

    if(debug){
        DEBUG(targetBorder);
        DEBUG(piercingTargetNodes);
        cerr << "distances of piercing target nodes from sources: "; for( int p : piercingTargetNodes ) cerr << dstFromSources[p] << " "; cerr << endl;
    }

    VI augmenting, nonAugmenting;

    if(debug) cerr << "returning non-augmenting piercing target node or any other if all are non-augmenting" << endl;


    int pN = -1;
    for( int d : piercingTargetNodes ){
        if( isSourceReachable[d] == false ){
            pN = d;
            nonAugmenting.push_back(d);
        }
        else{
            augmenting.push_back(d);
        }
    }

    if(debug){
        DEBUG(augmenting);
        DEBUG(nonAugmenting);
    }

    auto comp = [=](int a, int b){
        if( isSource[a] != isSource[b] ) return (isSource[a] == true); // if a is source and b is not, then  b is LARGER than a, that is better, since we maximize values
        else if( getDistanceValueFrom( dstFromSources, dstFromTargets,a ) != getDistanceValueFrom( dstFromSources, dstFromTargets,a ) ){
            return getDistanceValueFrom( dstFromSources, dstFromTargets,a ) < getDistanceValueFrom( dstFromSources, dstFromTargets,a );
        }else if( dstFromSources[a] != dstFromSources[b] ) return dstFromSources[a] < dstFromSources[b];
        else return a < b;
    };

    auto getBestNode = [=, &comp]( VI& nodes ){
        if( nodes.empty() ) return -1;
        return *max_element( ALL(nodes), comp );
    };

    pN = getBestNode(nonAugmenting);

    string msg = "non-augmenting";
    if( pN == -1 ){
        pN = getBestNode(augmenting);
        msg = "augmenting";
    }

    if( isSource[pN] ){
        if(debug) cerr << "AllPiercingNodes are in sources!" << endl;
    }

    if(debug) cerr << "returning " << msg << " " << pN << endl;

    return pN;
}

vector<FlowCutter::PiercingNode> FlowCutter::getAllPiercingNodes() {

    vector<PiercingNode> pNodes;

    // NOW SOURCE NODES
    VI piercingSourceNodes;
    for( int i = (int)sourceBorder.size()-1; i>=0; i-- ){
        int p = sourceBorder[i];
        bool hasOutNeigh = false;
        for( int d : V[p] ){
            if( !isSource[d] ){
                hasOutNeigh = true;
                piercingSourceNodes.push_back(d);
            }
        }
        if( !hasOutNeigh ){ // if p is inside source, i can remove it from border
            swap( sourceBorder[i], sourceBorder.back() );
            sourceBorder.pop_back();
            uf->removeSource(p);
        }
    }
    for( int d : piercingSourceNodes ){
        if( isTargetReachable[d] == false ) pNodes.emplace_back( d,true,false ); // nonaugmenting source node d
        else pNodes.emplace_back( d,true,true ); // augmenting source node d
    }



    // NOW TARGETS
    VI piercingTargetNodes;
    for( int i = (int)targetBorder.size()-1; i>=0; i-- ){
        int t = targetBorder[i];
        bool hasOutNeigh = false;
        for( int d : revV[t] ){
            if( !isTarget[d] ){
                hasOutNeigh = true;
                piercingTargetNodes.push_back(d);
            }
        }
        if( !hasOutNeigh ){ // if p is inside source, i can remove it from border
            swap( targetBorder[i], targetBorder.back() );
            targetBorder.pop_back();
            uf->removeTarget(t);
        }
    }
    for( int d : piercingTargetNodes ){
        if( isSourceReachable[d] == false ) pNodes.emplace_back( d,false,false ); // nonaugmenting target node d
        else pNodes.emplace_back( d,false,true ); // augmenting source node d
    }

    return pNodes;
}


void FlowCutter::addToExpansionOrder(VI nodes) {
    for( int d : nodes ){
        if( !inExpansionOrder[d] ){
            expansionOrder.push_back(d);
            inExpansionOrder[d] = true;
        }
    }
}

void FlowCutter::calculateDistances() {
    bool debug = false;

    fill( ALL(dstFromSources), 0 );
    fill( ALL(dstFromTargets), 0 );

    auto layers = BFS::getBfsLayers( V, sources );
    for( int i=1; i<layers.size(); i++ ){
        for(int d : layers[i]) dstFromSources[d] = i;
    }

    layers = BFS::getBfsLayers( revV, targets );
    for( int i=1; i<layers.size(); i++ ){
        for(int d : layers[i]) dstFromTargets[d] = i;
    }

    if(debug){
        DEBUG(dstFromSources);
        DEBUG(dstFromTargets);
    }

}


void FlowCutter::addToSources(VI S) {
    for( int s : S ){
//        if( isTargetReachable[s] ) canBeAugmented = true;
        if( !isSource[s] ){
            sources.push_back(s);
            isSource[s] = true;
            sourceBorder.push_back(s);

            uf->addSource(s);
            addToExpansionOrder({s});
        }

        if( !isSourceReachable[s] ){
            sourceReachable.push_back(s);
            isSourceReachable[s] = true;
        }

        /*if( usingExpansionGraph && s > N/2 ){ // this is to instantaneously add both nodes in expansion graph to sources
            s -= N/2;
            if( !isSource[s] ){
                sources.push_back(s);
                isSource[s] = true;
                sourceBorder.push_back(s);

                uf->addSource(s);
                addToExpansionOrder({s});
            }

            if( !isSourceReachable[s] ){
                sourceReachable.push_back(s);
                isSourceReachable[s] = true;
            }
        }*/




    }
}

void FlowCutter::addToTargets(VI T) {
    for( int t : T ){
//        if( isSourceReachable[t] ) canBeAugmented = true;
        if( !isTarget[t] ){
            targets.push_back(t);
            isTarget[t] = true;
            targetBorder.push_back(t);

            uf->addTarget(t);
            addToExpansionOrder({t});
        }

        if( !isTargetReachable[t] ){
            targetReachable.push_back(t);
            isTargetReachable[t] = true;
        }


       /* if( usingExpansionGraph && t > N/2 ){ // this is to instantaneously add both nodes in expansion graph to sources
            t -= N/2;
            if( !isTarget[t] ){
                targets.push_back(t);
                isTarget[t] = true;
                targetBorder.push_back(t);

                uf->addTarget(t);
                addToExpansionOrder({t});
            }

            if( !isTargetReachable[t] ){
                targetReachable.push_back(t);
                isTargetReachable[t] = true;
            }
        }*/


    }

}






void FlowCutter::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    VVI initKernV; // this is initially kernelized V - V after subgraph kernelization, before deg3 kernelization, since deg3 kernelization may yield different results depending on nodes in IS
    DTKernelizer initKernelizer(V);
    V = initKernelizer.getKernelizedGraphSubgraphs(); // harder kernelization

    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

//    DEBUG( FlowCutter::getExpansionGraph(V) );


    TimeMeasurer::startMeasurement("FC");

    FlowCutter fc( SeparatorEvaluators::sepEvalToUse );
    auto seps = fc.createSeparators(V,10);
//    for( auto& sp : seps ) DEBUG(sp);
//    fc.getFlowCutterExpansionOrder( V, {0}, {15} );

    TimeMeasurer::stopMeasurement("FC");
    TimeMeasurer::writeAllMeasurements();


    exit(1);
}








