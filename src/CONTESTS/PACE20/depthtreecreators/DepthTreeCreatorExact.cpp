/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <CONTESTS/PACE20/depthtreecreators/DepthTreeCreatorExact.h>
#include <CONTESTS/PACE20/depthtreecreators/DepthTreeCreatorLarge.h>
#include <graphs/GraphReader.h>
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include <graphs/trees/Tree.h>
#include <utils/StandardUtils.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <CONTESTS/PACE20/separatorminimizers/SeparatorMinimizer.h>
#include <graphs/components/BridgesAndArtPoints.h>
#include "graphs/cliques/CliqueUtils.h"
#include "graphs/GraphInducer.h"
#include "combinatorics/sequences/combinations/Combinations.h"

VVI DepthTreeCreatorExact::degreeLowerBound;
queue<LL> DepthTreeCreatorExact::fSQueue;
unordered_map<LL,int> DepthTreeCreatorExact::fS;
VLL DepthTreeCreatorExact::origHashes;

DepthTreeCreatorExact::DepthTreeCreatorExact(VVI &V, int k, int recDpth) : DepthTreeCreator(V, recDpth), bestTree( DepthTree(V) ) {
    preprocessDegreeLowerBound();
    K = k;
    bestTree.root = -1;
    bestTree.height = Constants::INF;
    originalGraphRemapper = VI(V.size()); iota(ALL(originalGraphRemapper),0);
    createOriginalGraphHashes();
}

bool DepthTreeCreatorExact::existsDepthTree() {
    if( isForbiddenGraph() ){
        logSpacing(); cerr << "-----> Graph in FS: " << originalGraphRemapper << "  " << endl;
        return false;
    }

    if( checkLowerBounds() == false ) return false;
    else if( bestTreeFound ){ addGraphToFS(); return true; }

    if( checkGraphClass() ){ // if graph is of one of the classes
        if( bestTreeFound ) return true; // if tree of height <= K was found
        else{
            addGraphToFS();
            return false; // otherwise no such tree exists
        }
    }

    createBranchingNodes();
    createArtPointsAndUpdateBranchingPoints();

    createMinimalSubsets();

    if( branch() ) return true;
    else{
        addGraphToFS();
        return false;
    }
}

bool DepthTreeCreatorExact::checkGraphClass() {
    if( K == 1 ){ // single node
        if( V->size() == 1 ) {
            bestTree.root = 0;
            bestTree.height = 1;
            bestTree.par = {{0, -1}};
            bestTreeFound = true;
        }
        return true;
    }



    int N = V->size();
    int E = GraphUtils::countEdges(*V);



    if( E == ( (N * (N-1)) / 2  ) ){ // a clique
//        logSpacing(); cerr << "Graph is a clique" << endl;
        if( N <= K) {
            bestTree.root = 0;
            bestTree.height = V->size();
            for (int i = 0; i < V->size(); i++) bestTree.par[i] = i - 1;
            bestTreeFound = true;
        }
        return true;
    }






    if( E == N-1 ){ // tree
//        logSpacing(); cerr << "Graph is a tree" << endl;
    }


    { // clique with single nodes attached
        VI nodes(N);
        iota(ALL(nodes), 0);
        sort(ALL(nodes),  [=](int a, int b) { return (*V)[a].size() < (*V)[b].size(); }); // sorting by non-descending degrees

//        DEBUG(*V);
//        DEBUG(nodes);

        VB inIs(N, false);
        int isDegSum = 0;
        int isSize = 0;
        for (int p : nodes) {
            bool add = true;
            for (int d : (*V)[p])
                if (inIs[d]) {
                    add = false;
                    break;
                }
            if (add) {
                inIs[p] = true;
                isSize++;
                isDegSum += (*V)[p].size();
            }
        }
        bool isCliqueWithNodesAttached = false; // this should be set true, if there exists clique Q such that V \ Q is independent set. Then add nodes from clique in order of non-ascending degrees
        int clqSize = N - isSize; // if there is given decomposition, then clique has size N - isSize
        if (isDegSum + (clqSize * (clqSize - 1)) / 2 == E) isCliqueWithNodesAttached = true;

//        DEBUG(N); DEBUG(E);DEBUG(isSize);DEBUG(isDegSum);DEBUG(clqSize);

        if (isCliqueWithNodesAttached) {
//            logSpacing(); cerr << "Graph is a clique with nodes attached" << endl;
//            DEBUG(*V); DEBUG(clqSize);

            VI clq;
            for (int i = 0; i < N; i++) if (!inIs[i]) clq.push_back(i);
            sort(ALL(clq), [=](int a, int b) { return (*V)[a].size() > (*V)[b].size(); }); // sorting clique by largest degree first, then attaching is to lowest node

//            DEBUG(clq);

            int H = (  ( (*V)[clq.back()].size() == clqSize-1 )   ? clqSize : clqSize+1  );

//            DEBUG(H); DEBUG(K);

            if( H > K ){
//                logSpacing(); cerr << "clique with nodes attached fails" << endl;
                bestTreeFound = false;
                return true;
            }

            bestTree.root = clq[0];
            bestTree.height = H;

            VI indInClq(N,-1);
            for(int i=0; i<clq.size(); i++){
                bestTree.par[ clq[i] ] = ( i == 0 ? -1 : clq[i-1] );
                indInClq[clq[i]] = i;
            }

            for( int i=0; i<N; i++ ){
                if(inIs[i]){
                    int m = -1;
                    for( int d : (*V)[i] ) m = max( m, indInClq[d] );
                    bestTree.par[i] = clq[m];
                }
            }

            return true;
        }
    }


    return false;

}

void DepthTreeCreatorExact::createArtPointsAndUpdateBranchingPoints() {
    artPoints = BridgesAndArtPoints::getBridgesAndArtPoints(*V).first;
    sort(ALL(artPoints));
    VI diff;
    set_difference( ALL(branchingNodes), ALL(artPoints), back_inserter(diff) );

//    DEBUG(branchingNodes);
//    DEBUG(artPoints);

    VB inOldBrN(V->size(),false);
    for(int p : branchingNodes) inOldBrN[p] = true;

    const int MAX_ART_POINTS = 10;
    if( artPoints.size() > MAX_ART_POINTS ){
        vector<Separator> seps;
        seps.reserve(artPoints.size());
        for( int i=0; i<artPoints.size(); i++ ){
            seps.emplace_back( *V, VI( 1, artPoints[i] ) );
            seps.back().createSeparatorStats();
        }

        sort( ALL(seps), SeparatorEvaluators::sepEvalToUse );
        while( seps.size() > MAX_ART_POINTS ) seps.pop_back();
        artPoints.clear();
        for( int i=0; i<seps.size(); i++ ) artPoints.push_back( seps[i].nodes[0] );

        partition( ALL(artPoints), [&inOldBrN](int a){ return inOldBrN[a]; } );
    }
    branchingNodes = diff;
    logSpacing(); cerr << "branchingNodes.size(): " << branchingNodes.size() << "  "; DEBUG(branchingNodes);

//    partition( ALL(artPoints), [&inOldBrN](int a){ return inOldBrN[a]; } );



    logSpacing(); DEBUG(artPoints);
}


bool DepthTreeCreatorExact::branch() {
//    cerr << "Branching!" << endl;
    auto fun = [=]( VI subset ){
        long long mask = 0;
        for(int p : subset) mask |= (1ll << p);
        if( minimalSubsets[mask] == false ) return false;

        for(int& p : subset) p = branchingNodes[p];
//        DEBUG(mask);
        if( branch(subset) ) return true;
        else return false;
    };

//    DEBUG(artPoints)
    for( int ap : artPoints ){
        if( branch( {ap} ) ) return true;
    }


//    DEBUG(branchingNodes);
    int B = branchingNodes.size();
    for( int k=1; k<=B; k++ ){
        VI subs(k);
        iota(ALL(subs),0);
        if( fun(subs) ) return true;
        while( Combinations::next_combination( subs,B-1 ) ){
            if( fun(subs) ) return true;
        }
    }


    return false;

}

bool DepthTreeCreatorExact::branch( VI sep ) {
    if( recDepth <= 2 ){
//        logSpacing(); cerr << "Branching on " << sep << endl << endl;
        logSpacing(); cerr << "Branching on original nodes "; for(int p : sep) cerr << originalGraphRemapper[p] << " "; cerr << endl << endl;
    }
//    return false;

    VVI comps = ConnectedComponents::getConnectedComponents(*V,sep);
    sort( ALL(comps), [](auto& v1, auto& v2){return v1.size() > v2.size();} );

    vector<InducedGraph> subgraphs;
    subgraphs.reserve( comps.size() );
    vector< DepthTreeCreatorExact > creators;

    for( int i=0; i<comps.size(); i++ ){
        subgraphs.push_back( GraphInducer::induce(*V,comps[i]) );
        creators.push_back( DepthTreeCreatorExact( subgraphs.back().V, K - sep.size(), recDepth+1 ) );
        for( int j=0; j<comps[i].size(); j++ ) creators.back().originalGraphRemapper[j] = originalGraphRemapper[ subgraphs.back().nodes[j] ];
        if( creators.back().checkLowerBounds() == false ) return false;
    }

    for( int i=0; i<comps.size(); i++ ){
        if( creators[i].existsDepthTree() == false ) return false;
    }

    bestTree = DepthTree(*V);
    bestTree.root = sep[0];
    bestTree.height = -1;
    int par = -1;
    for( int i=0; i<sep.size(); i++ ){
        bestTree.par[ sep[i] ] = par;
        par = sep[i];
    }

    for( int i=0; i<comps.size(); i++ ){
        DepthTree* subt = &creators[i].getBestTree();
        InducedGraph *g = &subgraphs[i];

        for( PII p :subt->par ){
            int a = p.first;
            a = g->nodes[a];

            int b = p.second;
            if( b == -1 ) b = sep.back();
            else b = g->nodes[b];

            bestTree.par[a] = b;
        }

        bestTree.height = max( bestTree.height, (int)sep.size() + subt->height );
    }

    return true;

}


void DepthTreeCreatorExact::createMinimalSubsets() {
    int B = branchingNodes.size();

    if( B >= 30 ) DEBUG(branchingNodes);

    createComponentGraph();
    minimalSubsets = VB( 1 + (1ll)<<B, true );

    // returns true if V \ subset is connected, false otherwise
    auto checkSubsetForConnectivity = [=,&B]( VI subset ){

        for(int& p : subset) p += C;
        int cnt = 0;
        VB was(componentGraph.size(),false);
        VB inSubset = StandardUtils::toVB(componentGraph.size(),subset);

        function< void(int) > dfs = [=,&dfs,&cnt,&was, &inSubset](int num){
//            DEBUG(num);
            was[num] = true;
            cnt++;
            for( int d : componentGraph[num] ){
                if( !was[d] ){
                    if( !inSubset[d] || d < C ) dfs(d);
                }
            }
        };

        for( int i=0; i<componentGraph.size(); i++ ){
            if( !inSubset[i] ){
                dfs(i);
                break;
            }
        }

//        assert( cnt <= (int)(componentGraph.size() - subset.size()) );
        if( cnt > (int)(componentGraph.size() - subset.size()) ){
            DEBUG(*V);
            DEBUG(subset);
            DEBUG(cnt);
            cerr << "After removing from V subset " << subset << " graph is " << ( cnt == componentGraph.size() - subset.size() ? "" : "not" ) << " connected" << endl;
            ENDL(1);
            DEBUG(dtrees);

            exit(1);
        }


        return cnt == componentGraph.size() - subset.size();
    };

    long long testCnt = 1;
    minimalSubsets[0] = false;

    function< void(int) > markAllSupermasks = [=,&markAllSupermasks, &B, &testCnt]( long long mask ){
        if( mask > ( 1ll << B ) || minimalSubsets[mask] == false ) return;
        minimalSubsets[mask] = false;
        testCnt++;

//        cerr << "\tmarking subset as not minimal: "; for( int i=0; i<B; i++ ) if( mask & (1ll << i) ) cerr << branchingNodes[i];
//        cerr << endl;

        for( int i=0; i <= B; i++ ) markAllSupermasks( mask | (1ll << i) );
    };

    auto fun = [=,&checkSubsetForConnectivity,&B, &markAllSupermasks, &testCnt](VI subset){
//        cerr << "in fun, subset = " << subset << endl;
        long long mask = 0;
        for(int p : subset) mask |= (1ll << p);

        if( minimalSubsets[mask] && checkSubsetForConnectivity(subset) == false ){

//            for(int& p : subset) p = branchingNodes[p];
//            cerr << "subset " << subset << " is minimal" << endl;

            for( int i=0; i<=B; i++ ) if( (mask & (1ll << i)) == 0 ) markAllSupermasks( mask | (1ll << i) );
        }else{
            if( minimalSubsets[mask] ) testCnt++;
            minimalSubsets[mask] = false;
//            DEBUG(subset);
//            for(int& p : subset) p = branchingNodes[p];
//            cerr << "subset " << subset << " does not disconnect graph" << endl;
        }
    };



    for( int k=1; k<=B; k++ ){
//        DEBUG(k);
        for( int i = 1; i < (1ll << B); i++ ){
            if( minimalSubsets[i] && __builtin_popcount(i) == k ){
                VI subs;
                for( int j=0; (1ll<<j) <= i; j++ ) if( i&(1ll<<j) ) subs.push_back(j);
                fun(subs);
            }
        }
    }


//    for( int k = 1; k <= B; k++ ){
//        DEBUG(k);
//        processAllSubsets( B-1,k,fun );
//    }




//    DEBUG( (1ll << B) );
//    DEBUG(testCnt);
    logSpacing(); cerr << "There are " << ( ( 1ll << B ) - testCnt ) << " minimal subsets to branch on" << endl;

}


void DepthTreeCreatorExact::processAllSubsets(int N, int K, function<void(VI &)> fun) {
    VI subs(K);
    iota(ALL(subs),0);
    fun(subs);
    while( Combinations::next_combination( subs,N ) ) fun(subs);
}



void DepthTreeCreatorExact::createComponentGraph() {

    VVI comps = ConnectedComponents::getConnectedComponents(*V, branchingNodes);
    C = comps.size();
    int B = branchingNodes.size();
    componentGraph = VVI( B+C );

    VI inComp(V->size(),-1);
    for( int i=0; i<comps.size(); i++ ) for(int p : comps[i]) inComp[p] = i;
    for( int i=0; i<B; i++ ) inComp[ branchingNodes[i] ] = C+i;

    for( int i=C; i<B+C; i++ ){
        unordered_set<int> neigh;
        int p = branchingNodes[i-C];
        for( int d : (*V)[p] ) neigh.insert( inComp[d] );
        for( int p : neigh ){
            if( p >= C && p > i ) continue;
            componentGraph[i].push_back(p);
            componentGraph[p].push_back(i);
        }
    }

//    DEBUG(C);
//    DEBUG(componentGraph);

}

void DepthTreeCreatorExact::createDominationGraph() {

}

void DepthTreeCreatorExact::preprocessDegreeLowerBound() {
    if( degreeLowerBound.empty() ){

//        cerr << "degreeLowerBound not testes yet!" << endl;

        int maxN = 505;
        degreeLowerBound = VVI(maxN);
        for( int i=1; i<maxN; i++ ){
            degreeLowerBound[i] = VI(i);
            degreeLowerBound[i][0] = 1; // isolated nodes
            if( i >= 2 ) degreeLowerBound[i][1] = 2; // pairwise disjoint edges
        }
        degreeLowerBound[1][0] = 1;

        for( int i=3; i<maxN; i++ ){
            for( int k=2; k<i; k++ ){
                int n = i;
                while( n > 0 ){
                    degreeLowerBound[i][k]++;
                    n = (int)ceil( (double)(n-1) / k );
                }
            }
        }

//        cerr << "initial values of degreeLowerBound" << endl;
//        for( int i=0; i<=16; i++ ){
//            cerr << i << ": ";
//            for( int k=0; k<degreeLowerBound[i].size(); k++ ){
//                cerr << degreeLowerBound[i][k] << " ";
//            }
//            cerr << endl;
//        }
    }
}

DepthTree DepthTreeCreatorExact::getDTCLLowerBound() {
    SeparatorEvaluators::sepEvalToUse = SeparatorEvaluators::estimatedDepthTreeEdgePlusNode;

    int reps = 3;

    for( int i=0; i<reps;i++ ){
        if( i == 0 ){
            SeparatorEvaluators::nodeScaleFactor = 0.95; SeparatorEvaluators::edgeScaleFactor = 0.05;
            Pace20Params::minimizeNodesIteration = true;
        }
        else if(i==1){
            SeparatorEvaluators::nodeScaleFactor = 0.05; SeparatorEvaluators::edgeScaleFactor = 0.95;
            Pace20Params::minimizeNodesIteration = false;
        }else{
            SeparatorEvaluators::nodeScaleFactor = 0.5; SeparatorEvaluators::edgeScaleFactor = 0.5;
        }

        DepthTreeCreatorLarge dtcl(*V, 1); // recDepth is set to 1 just to disable logs
        dtcl.MINIMIZE_SEPARATORS = false;
        dtcl.USE_KERNELIZATION = false;
        dtcl.SEPARATOR_CREATORS_MODE = DepthTreeCreatorLarge::COMP_EXP_CREATOR;

        DepthTree dtree = dtcl.getDepthTree();
//        DEBUG(dtree);
        dtrees.push_back(dtree);
    }
    int ind = -1, m = 500;
    for(int i=0; i<dtrees.size(); i++){
        if( dtrees[i].height < m ){
            m = dtrees[i].height;
            ind = i;
        }
    }
    return dtrees[ind];
}

bool DepthTreeCreatorExact::getPathLowerBound() {
    int N = V->size();
    VB was(N,false);

    VVI V2 = *V;

    VVI tree(N); // dfs tree
    for( int i=0; i<N; i++ ) tree[i].reserve( V2[i].size() );

    function< void(int) > dfs = [&V2,&dfs,&tree,&was](int num){
        was[num] = true;
        for( int d : V2[num] ){
            if( !was[d] ){
                tree[num].push_back(d);
                tree[d].push_back(num);
                dfs(d);
            }
        }
    };

    auto getDiameter = [&tree](){
        function<void(int,int,int, int&, int&)> getFurthest = [&tree,&getFurthest](int num, int par, int dst, int& furthest, int& furthestDst){
            if( dst > furthestDst ){
                furthest = num;
                furthestDst = dst;
            }

            for( int d : tree[num] ){
                if(d != par){
                    getFurthest( d,num,dst+1,furthest, furthestDst );
                }
            }
        };

        int furthest = -1, furthestDst = -1;
        getFurthest( 0,0,0,furthest,furthestDst );

        int a = furthest;
        furthest = furthestDst = -1;
        getFurthest( a, a, 0, furthest, furthestDst );

        return furthestDst;
    };

    int reps = 3;
    while(reps--){

        fill(ALL(was),false);
        for(int i=0; i<N; i++){
            random_shuffle(ALL(V2[i]));
            tree[i].clear();
        }

        int a = rand() % N;
        dfs(a);

//        DEBUG(tree);
        int diam = getDiameter() + 1;
//        DEBUG(diam);

        int lB = (int)ceil( log2(diam+1) );
//        DEBUG(lB);

        if( lB > K ){
//            DEBUG(diam);
            return false;
        }
    }

    return true;

}

DepthTree DepthTreeCreatorExact::getDepthTree() {
    int K = V->size();
    DepthTree bestTree(*V);

    DepthTreeCreatorExact creator(*V,K,0);
    while( creator.existsDepthTree() ){
        bestTree = creator.getBestTree();

        cerr << "Found tree of height " << bestTree.height << " <= " << K << ",  now looking for height " << bestTree.height-1 << endl;

        K = bestTree.height-1;
        creator = DepthTreeCreatorExact(*V,K,0);
        creator.addDtree(bestTree);

        fS.clear();
        queue<LL>().swap( fSQueue );
    }
    return bestTree;
}


bool DepthTreeCreatorExact::checkLowerBounds() {
    if( boundsChecked ) return true;

    { // degree lower bound
        int N = V->size();
        int maxDeg = max_element(ALL((*V)), [](VI &v1, VI &v2) { return v1.size() < v2.size(); })->size();
        if (getDegreeLowerBound(N, maxDeg) > K){
//            logSpacing(); cerr << "degree lower bound fails, K = " << K << "   V->size() = " << V->size() << "   maxDeg = " << maxDeg << endl;
            return false;
        }
    }

    { // path lower bound
        if( getPathLowerBound() == false ){
//            logSpacing(); cerr << "path lower bound fails, K = " << K << endl;
            return false;
        }
    }

    { // DTCL lowerBound - heuristic
        auto dtree = getDTCLLowerBound();

        double factor = (double)11 / 10;
        int err;
        if( V->size() > 100 ) err = 3;
        else if( V->size() > 50 ) err = 2;
        else err = 1;

        if( dtree.height > (int)ceil( factor*K ) + err ){
//            logSpacing(); cerr << "DTCL lower bound fails, K = " << K << "   dtree.height = " << dtree.height << endl;
            return false;
        }
        else if( dtree.height <= K ){
            swap(bestTree, dtree);
//            cerr << "Found dtree of height " << dtree.height << " <= " << K << endl;

            bestTreeFound = true; // #TEST
            return true;
        }
    }

    boundsChecked = true;
    return true;
}

void DepthTreeCreatorExact::createBranchingNodes() {
    branchingNodes.clear();
    int N = V->size();
    VB was(N,false);


    unordered_set<int> sepNodes;

    bool addSecondStretch = true;

    int cnt = 0;
    for(auto& dt : dtrees){
        auto strStr = dt.getStretchStructure();
        auto data = strStr.second;
        VI sep = data[0].sepNodes;
        sepNodes.insert( ALL(sep) );

        if(addSecondStretch){
            VVI tree = strStr.first;
            for( int i=0; i<tree.size(); i++ ) sort( ALL(tree[i]), [&data](int a, int b){ return data[a].height > data[b].height; } );
            if( tree[0].size() > 0 ) sepNodes.insert( ALL( data[ tree[0][0] ].sepNodes ) );
        }

//        if(++cnt == 2) break;
    }




    branchingNodes = VI(ALL(sepNodes));

    bool addNeighbors = true;

    if(addNeighbors) {
        unordered_set<int> neighbors;
        for (int p : sepNodes) {
            for (int d : (*V)[p]) neighbors.insert(d);
        }

        for (int p : sepNodes) neighbors.erase(p);

        int THR = 25;
        for (int p : neighbors) {
            if (branchingNodes.size() >= THR) break;
            else branchingNodes.push_back(p);
        }

    }


    sort(ALL(branchingNodes));

}


void DepthTreeCreatorExact::test(){
    Pace20Params::quickAndWeakTreeCreation = false;

    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
//    string filename = "exact_101.gr";
//    fstream str(filename);
//    VVI V = GraphReader::readGraphDIMACSWunweighed(str,false);

    Pace20Params::inputGraphSize = V.size();
    Pace20Params::inputGraphEdges = GraphUtils::countEdges(V);

    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    int K = V.size();
    DepthTreeCreatorExact dtce(V,K,0);

//    dtce.checkLowerBounds();
//    dtce.createBranchingNodes();
//    dtce.createMinimalSubsets();

    auto bestTree = dtce.getDepthTree();


//    if( dtce.existsDepthTree() ){
//        cerr << "Tree of height " << K << " exists: " << dtce.getBestTree() << endl;
//    }else{
//        cerr << "Tree of height " << K << " does not (most probably) exist" << endl;
//    }
    cerr << "FINISHED" << endl;
    DEBUG(bestTree);

    bestTree.write();

    exit(0);
}

void DepthTreeCreatorExact::createOriginalGraphHashes() {
    if( origHashes.empty() ){
        origHashes = VLL(500);
        LL M = (1ll << 60);
        UniformIntGenerator gen(0, M);
        for(int i=0; i<500; i++) origHashes[i] = gen.rand();
    }
}

bool DepthTreeCreatorExact::isForbiddenGraph() {
    LL hash = getGraphHash();
    auto it = fS.find(hash);
    if( it != fS.end() && it->second >= K ) return true;
    else return false;
}

LL DepthTreeCreatorExact::getGraphHash() {
    LL hash = 0;
    for( int i=0; i<V->size(); i++ ) hash ^= origHashes[ originalGraphRemapper[i] ];
    return hash;
}

void DepthTreeCreatorExact::addGraphToFS() {
    LL hash = getGraphHash();

    fSQueue.push(hash);
    if( fSQueue.size() > MAX_FS_SIZE ){
        LL rem = fSQueue.front();
        fSQueue.pop();
        fS.erase(rem);
    }

    fS.insert( {hash,K} );
}



/*

 // clique with nodes attached
p td 8 14
0 1
0 2
0 3
0 4
0 5
0 7
1 2
1 3
1 5
1 6
2 3
2 6
3 6
3 7

p td 7 11
0 1
0 2
0 3
0 4
0 5
1 2
1 3
1 5
1 6
2 3
2 6


 // clique
 p td 5 10
 0 1
 0 2
 0 3
 0 4
 1 2
 1 3
 1 4
 2 3
 2 4
 3 4


 // tree
 p td 11 10
 0 1
 0 8
 1 2
 1 3
 1 4
 2 9
 3 10
 4 5
 5 6
 6 7

*/