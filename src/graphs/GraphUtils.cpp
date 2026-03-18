//
// Created by sylwester on 8/8/19.
//


#include "../../include/graphs/GraphUtils.h"
#include "graphs/trees/Tree.h"
#include "graphs/components/ConnectedComponents.h"

#include "combinatorics/CombinatoricUtils.h"
#include "graphs/vertex_cover/approximation/PartitionSVC.h"
#include "graphs/vertex_cover/SolutionVC.h"
#include "../../include/utils/StandardUtils.h"
// #include <CONTESTS/PACE22/Utils.h>
// #include <graphs/treewidth/pace.h>
#include "contests/pace20/SeparatorEvaluators.h"
#include "contests/pace20/Pace20Params.h"
#include "graphs/cliques/CliqueExtension.h"
#include "graphs/components/BridgesAndArtPoints.h"
#include "contests/pace20/separatorcreators/FlowCutter.h"
#include "contests/pace20/separatorcreators/ComponentExpansionSeparatorCreator.h"


VI GraphUtils::getComplimentaryNodes( VVI & V, VI & nodes ){
    VB inNodes( V.size(),false );
    for(auto p : nodes) inNodes[p] = true;
    VI res;
    for( int i=0; i<V.size(); i++ ){
        if( !inNodes[i] ) res.push_back(i);
    }
    return res;
}

bool GraphUtils::isBipartite(VVI &V) {
    int N = V.size();
    constexpr int inf = 1e9;
    VI dst(N,inf);

    auto bfs = [&](int beg) {
        deque<int> que;
        que.push_back(beg);
        dst[beg] = 0;
        while(!que.empty()) {
            int v = que.front();
            que.pop_front();
            for(int d : V[v]) {
                if(dst[d] == dst[v]) return false;
                if(dst[d] == inf) {
                    dst[d] = dst[v]+1;
                    que.push_back(d);
                }
            }
        }
        return true;
    };

    for( int i=0; i<N; i++ ) if(dst[i] == inf) if( !bfs(i) ) return false;
    return true;
}

bool GraphUtils::isCycle(VVI& V) {
    int u = 0;
    int beg = u;
    if ( V[u].size() != 2 ) return false;
    int v = V[u][0];
    int visited = 1;

    while ( v != beg && V[v].size() == 2 ) {
        visited++;
        int w = ( V[v][0] == u ? V[v][1] : V[v][0] );
        u = v;
        v = w;
    }

    return (v == beg) && (visited == V.size());
}

bool GraphUtils::hasCycle(VVI &V) {
    int N = V.size();
    VB was(N);
    function<bool(int,int)> dfs = [&](int num, int par) {
        was[num] = true;
        for ( int d : V[num] ) if (d != par && was[d]) return true;
        // we can mark all neighbors as visited, because if at some point a neighbor of a node on path is reachable,
        // then that node on a path is also reachable, hence we will have a cycle
        for ( int d : V[num] ) was[d] = true;
        for ( int d : V[num] ) if ( d != par && dfs(d,num) ) return true;
        return false;
    };
    for ( int i=0; i<N; i++ ) if ( !was[i] ) if ( dfs(i,i) ) return true;
    return false;
}

VI GraphUtils::getRandomColoring( VVI & V ){
    VI color(V.size(),-1);
    VI perm = CombinatoricUtils::getRandomPermutation( V.size() );

//    cerr << "Radnom permutation: " << perm << endl;

    VB colorTaken(V.size()+1);
    for( int p : perm ){
        for( int d : V[p] ){ // marking colors taken by neighbors
            int c = color[d];
            if( c != -1 ) colorTaken[c] = true;
        }

        for( int i=0; i<colorTaken.size(); i++ ){ // choosing first free color and assigning that
            if( !colorTaken[i] ){
                color[p] = i;
//                cerr << "assigning color " << i << " to " << p << endl;
                break;
            }
        }

        for( int d : V[p] ){ // resetting taken colors
            int c = color[d];
            if( c != -1 ) colorTaken[c] = false;
        }
    }
    return color;
}

VPII GraphUtils::getGraphEdges( VVI & V, bool directed ){
    if(directed) return getDirectedGraphEdges(V);

    VPII res;
    res.reserve( countEdges(V) );
    for( int i=0; i<V.size(); i++ ){
        for( int d : V[i] ){
            if(d>i) res.push_back( {i,d} );
        }
    }
    return res;
}

VPII GraphUtils::getDirectedGraphEdges( VVI & V ){
    VPII res;
    res.reserve( countEdges(V) );
    for( int i=0; i<V.size(); i++ ){
        for( int d : V[i] ){
            res.push_back( {i,d} );
        }
    }
    return res;
}

VI GraphUtils::getNeighborhood(VVI &V, VI &S, bool useSet) {
    VI res;
    if( useSet ){
        unordered_set<int> zb;
        for( int p : S ){
            for(int d : V[p]){
                zb.insert(d);
            }
        }
        res = VI(ALL(zb));
    }else{
        VB was(V.size(),false);
        for( int p : S ){
            for(int d : V[p]){
                if( !was[d] ){
                    was[d] = true;
                    res.push_back(d);
                }
            }
        }
    }
    return res;
}

VI GraphUtils::getNeighborhoodOfAInB(VVI &V, VI &A, VI &B, bool useSet) {
    unordered_set<int> inBSet;
    VB inB;
    if( useSet ){
        inBSet.insert( ALL(B) );
    }else{
        inB = VB(V.size(),false);
        for(int d : B) inB[d] = true;
    }

    function< bool (int d) > inBFunction = [&inBSet, &inB, &useSet]( int d ) -> bool {
        if( useSet ) return (inBSet.count(d) > 0);
        else return inB[d];
    };

    VI res;
    if( useSet ){
        unordered_set<int> zb;
        for( int p : A ){
            for(int d : V[p]){
                if( inBFunction(d) ) zb.insert(d);
            }
        }
        res = VI(ALL(zb));
    }else{
        VB was(V.size(),false);
        for( int p : A ){
            for(int d : V[p]){
                if( inBFunction(d) && !was[d] ){
                    was[d] = true;
                    res.push_back(d);
                }
            }
        }
    }
    return res;
}


VVI GraphUtils::transposeGraph(VVI &v) {
    VVI g( SIZE(v) );
    REP( i,SIZE(v) )	REP( k,SIZE(v[i]) )	g[ v[i][k] ].PB(i);
    return g;
}


void GraphUtils::addEdge(VVI &V, int a, int b, bool directed) {
    V[a].push_back(b);
    if( !directed ) V[b].push_back(a);
}

bool GraphUtils::addEdgeSafe(VVI &V, int a, int b, bool directed) {
    auto exists = StandardUtils::find(V[a],b);
    if(!exists) V[a].push_back(b);

    bool x = false;
    if( !directed ) x = addEdgeSafe( V, b,a,true );

    return (!exists || x);
}

void GraphUtils::removeEdge(VVI &V, int a, int b, bool directed) {
    auto rem = [ &V ](int a, int b){
        for( int i=(int)V[a].size()-1; i>=0; i-- ){
            if( V[a][i] == b ){
                swap(V[a][i], V[a].back() );
                V[a].pop_back();
                break;
            }
        }
    };


    rem(a,b);
    if(!directed) rem(b,a);

}

void GraphUtils::removeNeighborsOfNode(VVI &V, int a, unordered_set<int> neighbors) {
    for( int i=(int)V[a].size()-1; i>=0; i-- ){
        if( neighbors.count( V[a][i] ) > 0 ){
            swap(V[a][i], V[a].back() );
            V[a].pop_back();
        }
    }
}

void GraphUtils::removeNodeFromGraph(VVI &V, int a) {
    for( int d : V[a] ) removeEdge( V,d,a,true );
    V[a].clear();
}

int GraphUtils::countNodesWithDegree(VVI &V, int d, int D) {
    int res = 0;
    for( auto & v : V ){
        if(v.size() >= d && v.size() <= D) res++;
    }
    return res;
}

bool GraphUtils::containsEdge(VVI &V, int a, int b) {
    return find( ALL(V[a]),b ) != V[a].end();
}

int GraphUtils::countEdges(VVI &V, const bool directed) {
    int res = 0;
    for(auto& v : V) res += v.size();
    if(!directed) return res >> 1;
    else return res;
}

VVI GraphUtils::sortNodeNeighborhoods( VVI & V ){
    VVI V2(V.size());
    for(int i=0; i<V.size(); i++) V2[i].reserve(V[i].size());
    for( int i=0; i<V.size(); i++ ){
        for( int d : V[i] ) V2[d].push_back(i);
    }
    return V2;
}


VVI GraphUtils::getComplimentaryGraph(VVI &V) {
    VVI V2(V.size());
    for( int i=0; i<V.size(); i++ ){
        VI tmp = V[i];
        tmp.push_back(i);
        sort(ALL(tmp));
        V2[i] = getComplimentaryNodes( V,tmp );
    }

    bool correct = true;
    for( int i=0; i<V.size(); i++ ){
        VB was(V.size(),false);
        for( int d : V[i] ) was[d] = true;
        for(int d : V2[i]) was[d] = true;
        if( V[i].size() + V2[i].size() != V.size() -1 ) correct = false;
        for(int k=0; k<V.size(); k++) if( k != i && !was[k]) correct = false;
    }

    assert(correct);

    return V2;


//    VVI V2 = V;
//    return makeCompliment(V2, CombinatoricUtils::getRandomPermutation(V.size()) );
}

void GraphUtils::removeNodesFromGraph(VVI &V, VI nodes) {

    unordered_set<int> nd(ALL(nodes));
    VI neigh = getNeighborhood( V,nodes,true );
    for( int t : neigh ){
        if( nd.count(t) ) continue;

        for( int i = (int)V[t].size()-1; i>=0; i-- ){
            if( nd.count(V[t][i]) ){
                swap( V[t][i], V[t].back() );
                V[t].pop_back();
            }
        }
    }

    for( int d : nodes ) V[d].clear();
}

void GraphUtils::removeNodesFromGraph(VVI &V, VVI &W, VI nodes) {
    unordered_set<int> nd(ALL(nodes));
    VI neigh = getNeighborhood( V,nodes,true );
    for( int t : neigh ){
        if( nd.count(t) ) continue;

        for( int i = (int)V[t].size()-1; i>=0; i-- ){
            if( nd.count(V[t][i]) ){
                swap( V[t][i], V[t].back() );
                V[t].pop_back();

                swap( W[t][i], W[t].back() );
                W[t].pop_back();
            }
        }
    }

    for( int d : nodes ){
        V[d].clear();
        W[d].clear();
    }
}

void GraphUtils::writeGraphHumanReadable(VVI &V) {
    clog << "****" << endl;
    for( int i=0; i<V.size(); i++ ){
        clog << i << ": ";
        VI neigh = V[i];
        sort(ALL(neigh));
        for(int d : neigh) clog << d << "  ";
        clog << endl;
    }
}

void GraphUtils::mergeNodeToNode(VVI &V, int a, int b) {
//    assert( containsEdge( V,a,a ) == false );
//    assert( containsEdge( V,b,b ) == false );

    VI Na = V[a];
    Na.push_back(a);

    VI Nb = V[b];
    Nb.push_back(b);


    sort(ALL(Na));
    sort(ALL(Nb));

//    DEBUG(VI({a,b}));
//    DEBUG(Na);
//    DEBUG(Nb);

    VI diff;
    set_difference( ALL(Na), ALL(Nb), back_inserter(diff) ); // diff is Na \ Nb

//    DEBUG(diff);

    removeNodeFromGraph(V,a);
    for( int d : diff ){

//        assert( containsEdge( V,d,b ) == false );
        V[d].push_back(b);

//        assert( containsEdge( V,b,d ) == false );
        V[b].push_back(d);
    }

//    cerr << "GraphUtils::mergeNodeToNode not tested yet" << endl;
//    exit(1);
}

void GraphUtils::contractEdge(VVI &V, int a, int b) {
    mergeNodeToNode(V,a,b);
}

#ifndef QUICK_BUILD
void GraphUtils::writeBasicGraphStatistics(VVI &V) {
    clog << "V.size(): " << V.size() << endl;
    clog << "|E(V)|: " << countEdges(V) << endl;
    clog << "average degree: " << ( (double)2 * countEdges(V) ) / V.size() << endl;
    clog << "largest degree: " << max_element( ALL(V), []( VI& v, VI& w ){ return v.size() < w.size(); } )->size() << endl;
    int m = min( (int)V.size()-1,3 );

    VVI comps = ConnectedComponents::getConnectedComponents(V);
    clog << "There are " << comps.size() << " connected components" << endl;
    sort( ALL(comps), [](VI& v1, VI& v2){ return v1.size() > v2.size(); } );
    m = min( (int)comps.size(),7 );
    clog << "first " << m << " largest component sizes: "; for( int i=0; i<m; i++ ) clog << comps[i].size() << " "; clog << endl;
    clog << "first " << m << " smallest component sizes: "; for( int i=(int)comps.size()-m; i< comps.size(); i++ ) clog << comps[i].size() << " "; clog << endl;

    m = min((int)V.size(),11);
    VI degs(V.size(),0);
    for( VI& v : V ) degs[v.size()]++;
    for(int i=0; i<m; i++) clog << "nodes with deg " << i << ": " << degs[i] << endl;
    clog << endl;

//    cerr << "isTree: " << Tree::isTree(V) << endl;

    // auto artsAndBridges = BridgesAndArtPoints::getBridgesAndArtPoints(V);
    // clog << "There are " << artsAndBridges.first.size() << " articulation points and " << artsAndBridges.second.size() << " bridges" << endl;
    // clog << endl;

    bool is_bipartite = isBipartite(V);
    DEBUG(is_bipartite);


//    { // clique size by greedy extension
//        ENDL(1);
//        if(V.size() < 3'000){
//            VVI compV = getComplimentaryGraph(V);
//            VI vc = Utils::getUpperBoundByVCOnSuperPIGraph(compV, 3000);
//            VI compVmis = CombinatoricUtils::getFullSetDifference( V.size(), vc );
//            VI clq = compVmis;
//            clog << "maximal clique found using FastVC on complimentary graph: " << clq.size() << endl;
//        }else{
//            VI clq = CliqueExtension::findMaximalNodeCliqueExtension(V, true);
//            clog << "maximal clique found via sparse greedy extension: " << clq.size() << endl;
//        }
//    }

//    { // VC and MIS sizes
//        ENDL(1);
//        VI vc = Utils::getUpperBoundByVCOnSuperPIGraph(V, 3000);
//        clog << "Estimated VC size (run FastVC for 3s): " << vc.size() << "  ->  mis: " << V.size() - vc.size() << endl;
//    }

//    { // treewidth
//        TREEWIDTH trw;
//        int max_cnt = 8;
//        volatile sig_atomic_t tle = 0;
//        TreewidthDecomposition decomp = trw.main( V, max_cnt, tle );
//        ENDL(1);
//        clog << "Found treewidth decomposition of size: " << decomp.getWidth() << endl;
//    }
//
//    { // separator
//        ENDL(5);
////
//        VVI comps = ConnectedComponents::getConnectedComponents(V);
//        VI cmp = *max_element(ALL(comps), [](auto & a, auto & b){ return a.size() < b.size(); });
//        InducedGraph g = GraphInducer::induce(V,cmp);
//        DEBUG(g.V.size());
//        DEBUG(GraphUtils::countEdges(g.V));
//
//        FlowCutter fc( SeparatorEvaluators::sepEvalToUse );
//        fc.setBalanceParameter(5);
//        vector<Separator> separators = fc.createSeparators(g.V, 5);
//        clog << "Separators found using 5 iterations of FlowCutter:" << endl;
//        for(auto sep : separators){
//            DEBUG(sep.stats);
//            InducedGraph g = GraphInducer::induce(V, sep.nodes);
//            clog << "\tGraph induced by sep has " << countEdges(g.V) << " edges" << endl;
//        }
//
////        ComponentExpansionSeparatorCreator exp(SeparatorEvaluators::sepEvalToUse);
////        vector<Separator> separators = exp.createSeparators(g.V,5);
////        clog << "Separators found using 5 iterations of ComponentExpansionSeparatorCreator:" << endl;
////        for(auto sep : separators) DEBUG(sep.stats);
//    }

}
#endif // QUICKBUILD

void GraphUtils::removeEdges(VVI &V, VPII &edges, VB & helper, bool directed) {
    int N = V.size();

    auto job = [&]() {
        sort(ALL(edges));
        int p = 0;
        while(p < N) {
            int q = p+1;
            while(q < N && edges[p].first == edges[q].first) q++;
            {
                int a = edges[p].first;
                for( int j=p; j<q; j++ ) helper[ edges[j].second ] = true;
                REMCVAL( helper, V[a] );
                for( int j=p; j<q; j++ ) helper[ edges[j].second ] = false;
            }
            p = q;
        }
    };

    job();

    if(!directed) {
        for( auto & [a,b] : edges ) swap(a,b);
        job();
    }

    assert(false && "This version of GraphUtils::removeEdges was not tested yet");
}

void GraphUtils::removeEdges(VVI &V, VPII &edges, bool directed) {

//    cerr << "removeEdges not tested yet" << endl;

    if( directed == false ){
        int E = edges.size();
        for( int i=0; i<E; i++ ) edges.emplace_back( edges[i].second, edges[i].first ); // adding reverse edges to remove
    }

    sort( ALL(edges) );

    for( int i=0; i<edges.size(); i++ ){
        int p = i;
        unordered_set<int> toRemove;
        while( p < edges.size() && edges[p].first == edges[i].first ){
            toRemove.insert( edges[p].second );
            p++;
        }

        int t = edges[i].first;
        for( int k=(int)V[t].size()-1; k>=0; k-- ){
            int d = V[t][k];
            if( toRemove.count(d) ){
//                cerr << "Removing edge " << t << " -> " << V[t][k] << endl;
                swap( V[t][k], V[t].back() );
                V[t].pop_back();
            }
        }

        i = p-1;
    }

}

bool GraphUtils::isConnected(VVI &V) {

    // below is the equivalent to the code above, but does not use recursion (and hence has no issues with small
    // stack size in IDE connected to WSL).
    int N = V.size();
    VB was(N);
    VI neigh; neigh.reserve(N);
    neigh.push_back(0);
    was[0] = true;
    int cnt = 0;
    for (int i=0; i<neigh.size(); i++) {
        int v = neigh[i];
        cnt++;
        for ( int d : V[v] ) if ( !was[d] ) {
            was[d] = true;
            neigh.push_back(d);
        }
    }
    return cnt == N;
}

VI GraphUtils::getNeighborhoodExclusive(VVI &V, VI &A, VB &helper) {
    VI res;
    for( int a : A ) helper[a] = true;
    for( int a : A ){
        for( int w : V[a] ){
            if(!helper[w]){
                res.push_back(w);
                helper[w] = true;
            }
        }
    }

    for( int a : A ) helper[a] = false;
    for(int d : res) helper[d] = false;
    return res;
}

int GraphUtils::regularity(VVI V) {
    int k = V[0].size();
    for(int i=1; i<V.size(); i++) if( V[i].size() != k ) return -1;
    return k;
}

VVI GraphUtils::makeSimple(VVI V) {
    for( int i=0; i<V.size(); i++ ){
        sort(ALL(V[i]));
        V[i].resize(unique(ALL(V[i])) - V[i].begin()); // removing parallel edges

        for( int j=(int)V[i].size()-1; j>=0; j--){ // removing loops
            if( V[i][j] == i ){
                swap(V[i][j], V[i].back());
                V[i].pop_back();
            }
        }
    }
    return V;
}

VVI GraphUtils::getGraphForEdges(VPII edges, bool directed) {
    int N = 0;
    for(auto & [a,b] : edges) N = max(N, max(a,b));
    VVI V(N+1);
    for( auto & [a,b] : edges ) addEdge(V,a,b,directed);
    return V;
}

VVI GraphUtils::remapGraph(VVI V, VI perm) {
    int N = V.size();
    VVI H(N);
    for( int i=0; i<N; i++ ){
        H[perm[i]].reserve(V[i].size());
        for(int d : V[i]) H[perm[i]].push_back(perm[d]);
    }

    return H;
}

bool GraphUtils::isSimple(VVI V) {
     int N = V.size();
     VB helper(N,false);

     for( int i=0; i<N; i++ ){
         for( int d : V[i] ){
             if( (d == i) || helper[d] ) return false;
             helper[d] = true;
         }

         for(int d : V[i]) helper[d] = false;
     }

     return true;
}

double GraphUtils::density(VVI &V, bool directed) {
    LL denom = V.size();
    denom *= denom-1;
    denom >>= 1;

    double density = countEdges(V, directed);
    return density / denom;
}


