/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/


#include <graphs/GraphUtils.h>
#include <graphs/trees/Tree.h>
#include <graphs/components/ConnectedComponents.h>

#include "graphs/GraphUtils.h"
#include "combinatorics/CombinatoricUtils.h"
#include <graphs/VertexCover/approximation/PartitionSVC.h>
#include <graphs/VertexCover/SolutionVC.h>
#include <utils/StandardUtils.h>
#include "graphs/cliques/CliqueExtension.h"
#include "graphs/components/BridgesAndArtPoints.h"

VI GraphUtils::getComplimentaryNodes( VVI & V, VI & nodes ){
    VB inNodes( V.size(),false );
    for(auto p : nodes) inNodes[p] = true;
    VI res;
    for( int i=0; i<V.size(); i++ ){
        if( !inNodes[i] ) res.push_back(i);
    }
    return res;
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

VPII GraphUtils::getGraphEdges( VVI & V ){
    VPII res;
    res.reserve( countEdges(V) );
    for( int i=0; i<V.size(); i++ ){
        for( int d : V[i] ){
            if(d>i) res.push_back( {i,d} );
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

bool GraphUtils::isMaximalIndependentSet(VVI &V, VI &mis) {
    VB was(V.size(),false);
    for( int d : mis ){
        was[d] = true;
        for(int p : V[d]) was[p] = true;
    }

    for( int i=0; i<V.size(); i++ ){
        if( !was[i] ){
            return false;
        }
    }
    return true;
}

void GraphUtils::addEdge(VVI &V, int a, int b, bool directed) {
    V[a].push_back(b);
    if( !directed ) V[b].push_back(a);
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

int GraphUtils::countEdges(VVI &V) {
    int res = 0;
    for(auto& v : V) res += v.size();
    return res >> 1;
}

VVI GraphUtils::sortNodeNeighborhoods( VVI & V ){
    VVI V2(V.size());
    for(int i=0; i<V.size(); i++) V2[i].reserve(V[i].size());
    for( int i=0; i<V.size(); i++ ){
    for( int d : V[i] ) V2[d].push_back(i);
    }
    return V2;
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
    cerr << "****" << endl;
    for( int i=0; i<V.size(); i++ ){
        cerr << i << ": ";
        VI neigh = V[i];
        sort(ALL(neigh));
        for(int d : neigh) cerr << d << "  ";
        cerr << endl;
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

void GraphUtils::writeBasicGraphStatistics(VVI &V) {
    cerr << "V.size(): " << V.size() << endl;
    cerr << "|E(V)|: " << countEdges(V) << endl;
    cerr << "average degree: " << ( (double)2 * countEdges(V) ) / V.size() << endl;
    int m = min( (int)V.size()-1,5 );
    VI degs(V.size(),0);
    for( VI& v : V ) degs[v.size()]++;
    for(int i=0; i<=m; i++) cerr << "nodes with deg " << i << ": " << degs[i] << endl;
    cerr << endl;

    cerr << "isTree: " << Tree::isTree(V) << endl;

    auto artsAndBridges = BridgesAndArtPoints::getBridgesAndArtPoints(V);
    cerr << "There are " << artsAndBridges.first.size() << " articulation points and " << artsAndBridges.second.size() << " bridges" << endl;
    cerr << endl;

    VVI comps = ConnectedComponents::getConnectedComponents(V);
    cerr << "There are " << comps.size() << " connected components" << endl;
    sort( ALL(comps), [](VI& v1, VI& v2){ return v1.size() > v2.size(); } );
    m = min( (int)comps.size(),7 );
    cerr << "first " << m << " largest component sizes: "; for( int i=0; i<m; i++ ) cerr << comps[i].size() << " "; cerr << endl;
    cerr << "first " << m << " smallest component sizes: "; for( int i=(int)comps.size()-m; i< comps.size(); i++ ) cerr << comps[i].size() << " "; cerr << endl;

    VI clq = CliqueExtension::findMaximalNodeCliqueExtension(V);
    cerr << "maximal clique found via greedy extension: " << clq.size() << endl;

    PartitionSVC vcCreator(V);
    vcCreator.setSupressAllLogs(true); vcCreator.setMaxIterations( 200 ); vcCreator.setMaxRunTime(5'000);vcCreator.getSvcParams().alpha = 0.5;
    vcCreator.getSvcParams().iterationsPerSubgraph = 200;vcCreator.getSvcParams().setInitialSolutionForSubgraph = false;vcCreator.setTakeFirstBestSolution(false);
    vcCreator.getSvcParams().initialSolutionIterations = 1;vcCreator.run();
    VI vc = ((SolutionVC*) vcCreator.getBestSolution())->getVC();
    cerr << "Quickly found vertex cover size: " << vc.size() << "  and corresponding independent set size: " << V.size() - vc.size() << endl;



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
//    cerr << "isConnected not tested yet!" << endl; exit(1);
    VB was(V.size(),false);
    int cnt = 0;
    function< void(int) > dfs = [&V,&was, &dfs, &cnt](int num){
        was[num] = true;
        cnt++;
        for( int d : V[num] ) if( !was[d] ) dfs(d);
    };
    dfs(0);
    return (cnt == V.size());
}

