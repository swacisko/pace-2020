/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <graphs/matching/MaxMatchBipartite.h>
#include <graphs/VertexCover/VCUtils.h>
#include <graphs/GraphUtils.h>
#include <graphs/VertexCover/kernelization/KernelizerVC.h>
#include <Constants.h>
#include <combinatorics/CombinatoricUtils.h>

#include "graphs/VertexCover/kernelization/KernelizerVC.h"


pair<VI, VPII> KernelizerVC::loopKernelization(VVI &G) {
    VI nodes;
    for( int i=0; i<G.size(); i++ ){
        for( int p : G[i] ){
            if( p == i ){
                nodes.push_back(i);
                break;
            }
        }
    }

    set<PII> nodeDegrees; // only to call below function
    VPII edges = removeNodesFromGraph(G, nodes, nodeDegrees );

    return { nodes,edges };
}


pair<VI, VPII> KernelizerVC::crownDecomposition(VVI &G) {

    MaxMatchBipartite matcher;
    VI maximalMatching = matcher.getMaximalMatchingOfMinimalSizeRandom(G, 10);

    VB bipartition(G.size(),false);
    for( int i=0; i<maximalMatching.size(); i++ ){
        if( maximalMatching[i] == -1 ) bipartition[i] = true;
    }


    VPII M1 = matcher.convertToPairs( maximalMatching );
//    cerr << "maximal matching found of size " << M1.size() << endl;   for(auto p : M1) cerr << "(" << p.first << "," << p.second << ")   "; cerr << endl;


    VI maximumMatchingBipartite = matcher.getMaximumMatchingInBipartition( G, bipartition );




    VPII M2 = matcher.convertToPairs( maximumMatchingBipartite );
//    cerr << "maximum matching found of size " << M2.size() << endl; for(auto p : M2) cerr << "(" << p.first << "," << p.second << ")   "; cerr << endl;

    VI X = getMinVCForBipartition(G,maximumMatchingBipartite, bipartition); // X is vertex cover of the graph with given bipartition

    VI H;
    for( int p : X ){
        if( bipartition[p] == false && maximalMatching[p] != -1 ) H.push_back(p);
    }

//    cerr << "Found head of the crown:  "; for(int p : H) cerr << p << " "; cerr << endl;

    pair<VI, VPII> res;
    res.first = H;
    set<PII> nodeDegrees; // this is here only to call below function
    res.second = removeNodesFromGraph( G,H, nodeDegrees );


    return res;
}

pair<VI, VPII> KernelizerVC::initialKernelization(VVI &G) {

    pair<VI, VPII> res;

    //******** LOOP
    auto kernel = loopKernelization(G);
//    cerr << "There are following vertices with loops: "; for(auto a : kernel.first) cerr << a << " "; cerr << endl;
    res = kernel;


    int ITERATIONS = 1;
    int iter = 0;

//    cerr << "\tThere are " << GraphUtils::countNodesWithDegree(G,1,G.size()) << " nonisolated vertices before kernelization" << endl;


//    kernel = degree1Kernelization(G);
//    res.first.insert( res.first.end(), kernel.first.begin(), kernel.first.end() );
//    res.second.insert( res.second.end(), kernel.second.begin(), kernel.second.end() );
//
//    cerr << "adding " << kernel.first.size() << " nodes to kernel" << endl;
//    cerr << "\tThere are " << GraphUtils::countNodesWithDegree(G,1,G.size()) << " nonisolated vertices after degree 1 kernelization" << endl;
//
//
//
//    kernel = degree1Kernelization(G);
//    res.first.insert( res.first.end(), kernel.first.begin(), kernel.first.end() );
//    res.second.insert( res.second.end(), kernel.second.begin(), kernel.second.end() );
//
//    cerr << "adding " << kernel.first.size() << " nodes to kernel" << endl;
//    cerr << "\tThere are " << GraphUtils::countNodesWithDegree(G,1,G.size()) << " nonisolated vertices after degree 1 kernelization" << endl;

    int crownAndLPChecks = 0;

    do{
        iter++;

        kernel = degree1Kernelization(G);
        res.first.insert( res.first.end(), kernel.first.begin(), kernel.first.end() );
        res.second.insert( res.second.end(), kernel.second.begin(), kernel.second.end() );


//        cerr << "deg1: adding " << kernel.first.size() << " nodes to kernel" << endl;
//        cerr << "\tThere are " << GraphUtils::countNodesWithDegree(G,1,G.size()) << " nonisolated vertices after degree 1 kernelization" << endl << endl;



        kernel = dominationRule(G,G.size());
        res.first.insert( res.first.end(), kernel.first.begin(), kernel.first.end() );
        res.second.insert( res.second.end(), kernel.second.begin(), kernel.second.end() );

//        cerr << "adding " << kernel.first.size() << " nodes to kernel" << endl;
//        cerr << "\tThere are " << GraphUtils::countNodesWithDegree(G,1,G.size()) << " nonisolated vertices after domination rule" << endl << endl;

        if( kernel.first.size() > 0 ) continue;


        crownAndLPChecks++;
        if( crownAndLPChecks <= 1 ){

            kernel = crownDecomposition(G); // CROWN
            if( kernel.first.size() > 0 ) iter = 0;
            res.first.insert( res.first.end(), kernel.first.begin(), kernel.first.end() );
            res.second.insert( res.second.end(), kernel.second.begin(), kernel.second.end() );

//            cerr << "adding " << kernel.first.size() << " nodes to kernel" << endl;
//            cerr << "\tThere are " << GraphUtils::countNodesWithDegree(G,1,G.size()) << " vertices after crown decomposition" << endl << endl;

    //        if( kernel.first.size() > 0 ) continue;


            kernel = lpDecomposition(G); // LP
            if( kernel.first.size() > 0 ) iter = 0;
            res.first.insert( res.first.end(), kernel.first.begin(), kernel.first.end() );
            res.second.insert( res.second.end(), kernel.second.begin(), kernel.second.end() );

//            cerr << "adding " << kernel.first.size() << " nodes to kernel" << endl;
//            cerr << "\tThere are " << GraphUtils::countNodesWithDegree(G,1,G.size()) << " vertices after LP-like-decomposition" << endl << endl;

            if( kernel.first.size() > 0 ) continue;

        }



//        ENDL(5);

    }while( kernel.first.size() > 0 || iter < ITERATIONS );



    return res;
}


set<PII> KernelizerVC::getNodeDegreesForGraph(VVI &G) {
    set<PII> res;

    for( int i=0; i<G.size(); i++ ){
        if( G[i].size() > 0 ) res.insert( { G[i].size(),i } );
    }
    return res;
}

pair<VI, VPII> KernelizerVC::iterativeKernelization(VVI &G, set<PII> &nodeDegrees, VI &currentVC, VI &bestVC) {
    /**
     * We search for vertex in graph G with size at most K
     */
    int K = bestVC.size()-1 - currentVC.size();


    //**************************** checking for leaves and node with size at least K
    VI kernelNodes;
    VPII kernelEdges;

    while( !nodeDegrees.empty()  ) {

        set<PII>::iterator it;

        auto begin = nodeDegrees.begin();
        if ( G[ begin->second ].size() != begin->first ){
            nodeDegrees.erase(begin);
            continue;
        }


        auto rbegin = nodeDegrees.rbegin();
        if( G[ rbegin->second ].size() != rbegin->first ){
            it = nodeDegrees.end();
            it--;
            nodeDegrees.erase(it);
            continue;
        }

        if( !( rbegin->first > K || begin->first == 1 ||( begin->first == 2 && rbegin->first == 2 ) ) ) break;

        if ( rbegin->first > K){
            it = nodeDegrees.end();
            it--;
        }else it = begin;

        int d = it->second;
//        cerr << "it = (" << it->first << "," << it->second << ")    d = " << d << "    G.size(d) = " << G.size(d) << "   K = " << K << endl << endl;

        if (G[d].size() == it->first) { // if size of given node is correct (there may be many nodes with old size - those nodes should be discarded.

            if( it->first == 1 ) d = G[d][0];

            VI neigh = G[d];
            VPII edgesRemoved = removeNodeFromGraph(G, d);
            kernelEdges.insert(kernelEdges.end(), edgesRemoved.begin(), edgesRemoved.end());
            kernelNodes.push_back(d);

            nodeDegrees.erase(it);
            for (int p : neigh) {
                if (G[p].size() > 0) nodeDegrees.insert({G[p].size(), p});
            }
            K--;
        } else {
            nodeDegrees.erase(it);
        }

    }


    //******************************** checking done


    return { kernelNodes, kernelEdges };
}

void KernelizerVC::restoreGraphAndNodeDegreesFromKernel(VVI &G, set<PII> &nodeDegrees, pair<VI, VPII> &kernel, VI &currentVC) {
    set<int> neighborhood;

    for( PII e : kernel.second ){ // add all edges to G and mark all nodes that should be added to nodeDegrees
//        G.addEdge(e.first, e.second);
        GraphUtils::addEdge(G,e.first, e.second);

        neighborhood.insert( e.first );
        neighborhood.insert( e.second );
    }

    for( int d : neighborhood ){ // add pairs to nodeDegrees
        nodeDegrees.insert( { G[d].size(),d } );
    }

    currentVC.resize( currentVC.size() - kernel.first.size() ); // removing from currentVC nodes that are in given kernel (or at least that number of nodes that were
    // recently added to currentVC)

}

VI KernelizerVC::getMinVCForBipartition(VVI &G, VI &matching, VB &bipartition) {
//    cerr << "Determining minimal VC for bipartition 0:   "; for(int i=0; i<G.size(); i++) if( bipartition[i] == false ) cerr << i << " "; cerr << endl;

    VI U; // vector of all unmatched vertices of G, that are in bipartition 0
    for( int i=0; i<G.size(); i++ ){
        if( matching[i] == -1 && bipartition[i] == false ) U.push_back(i);
    }

//    cerr << "Set U of unmatched nodes in bipartition 0:  "; for(int p : U) cerr << p << " "; cerr << endl;


    VI Z; // vector of all vertices that are either in U or are connected to U by an alternating path.
    VB was(G.size(),false);
    VI neigh = U; // this if for bfs to mark all alternating paths.
    for( int i=0; i<neigh.size(); i++ ){
        int d = neigh[i];
        if( was[d] ) continue;
        was[d] = true;
//        cerr << "d = " << d << "   adding to Z" << endl;
        Z.push_back(d);

        for( int p : G[d] ){
            if( was[p] || bipartition[p] == bipartition[d] ) continue;
//            cerr << "\tadding p = " << p << " to Z" << endl;
            Z.push_back(p);
            neigh.push_back( matching[p] );

            was[p] = true;
        }
    }

//    cerr << "Set Z of all vertices that are in U or are connected by alternating path to U:  "; for(int p : Z) cerr << p << " "; cerr << endl;

    VB Z2(G.size(),false); // Z2 is just Z but in other representation
    for( int p : Z ) Z2[p] = true;

    VI K; // K should be ( L \ Z ) + ( R & Z ), where L is the set of all nodes with bipartition 0 and R with bipartition 1

    for( int i=0; i<G.size(); i++ ){
        if( G[i].size() == 0 ) continue;

        if( bipartition[i] == false && !Z2[i] ){
            K.push_back(i);
        }else if( bipartition[i] == true && Z2[i] ){
            K.push_back(i);
        }
    }

//    cerr << "minimal vertex cover K:  "; for(int p : K) cerr << p << " "; cerr << endl;

    // NPW K should be a minimum vertex cover of all the edges between bipartition of the graph.
    return K;
}

pair<VI, VPII> KernelizerVC::lpDecomposition(VVI &G) {
    VVI layerG( 2*G.size() );

    for( int i=0; i<G.size(); i++ ){
        for( int d : G[i] ){
//            layerG.addEdge( i, G.size() + d );
            GraphUtils::addEdge( layerG, i, G.size()+d );
        }
    }

    VB bipartition(layerG.size(),false);
    for( int i=0; i<G.size(); i++ ) bipartition[i] = true;

    MaxMatchBipartite matcher;
    VI matching = matcher.getMaximumMatchingInBipartition(layerG,bipartition);

//    cerr << "layerG:" << endl; layerG.write();
//    cerr << "matching found in layer graph:  "; for(auto p : matcher.convertToPairs(matching)) cerr << "(" << p.first << "," << p.second << ")  "; cerr << endl;


    pair<VI, VPII> kernel;
    VI minVC = getMinVCForBipartition( layerG, matching, bipartition );
    VB minVC2( bipartition.size(), false );
    for(int p : minVC) minVC2[p] = true;

    for( int i=0; i<G.size(); i++ ){
        if( minVC2[i] && minVC2[G.size()+i] ) kernel.first.push_back(i);
        else if( !minVC2[i] && !minVC2[ G.size()+i ] ){
            auto v = G[i];
            for(auto d : v){
//                G.removeEdge(i,d);
                GraphUtils::removeEdge(G,i,d);
            }
        }
    }

    set<PII> nodeDegrees;
    kernel.second = removeNodesFromGraph( G, kernel.first, nodeDegrees );

//    cerr << "kernel of size " << kernel.first.size() << " found via LP method:  "; for(int p : kernel.first) cerr << p << " "; cerr << endl;

    return kernel;
}

VPII KernelizerVC::removeNodesFromGraph(VVI &G, VI &toRemove, set<PII> &nodeDegrees) {
//        cerr << "removing nodes from graph: "; for(int p : toRemove) cerr << p << " "; cerr << endl;

    VPII res;
    VI nodes;
    for( int p : toRemove ){
        VPII temp = removeNodeFromGraph( G, p );
        res.insert( res.end(), temp.begin(), temp.end() );
        for( auto p : temp ){
            nodes.push_back(p.first);
            nodes.push_back(p.second);
        }
    }

    sort( nodes.begin(), nodes.end() );
    nodes.resize( unique( nodes.begin(), nodes.end() ) - nodes.begin() );
    for( int p : nodes ) if( G[p].size() > 0 ) nodeDegrees.insert( { G[p].size(),p } );

    return res;
}

VPII KernelizerVC::removeNodeFromGraph( VVI & G, int toRemove ){
    VPII res;
    for( int p : G[toRemove] ) res.push_back( { toRemove, p } );
    for( PII p : res ){
//            G.removeEdge( p.first, p.second );
        int a = p.first;
        int b = p.second;
        for( int i=G[a].size()-1; i>=0; i-- ){
            if( G[a][i] == b ){
                swap(G[a][i], G[a].back());
                G[a].pop_back();
                break;
            }
        }

        swap(a,b);
        for( int i=G[a].size()-1; i>=0; i-- ){
            if( G[a][i] == b ){
                swap(G[a][i], G[a].back());
                G[a].pop_back();
                break;
            }
        }
    }
    return res;
}

void KernelizerVC::addEdgesToGraph( VVI & G, VPII & edges ){
    for( PII e : edges ){
//            G.addEdge( e.first, e.second );
        G[e.first].push_back(e.second);
        G[e.second].push_back(e.first);
    }
}

pair<VI, VPII> KernelizerVC::degree1Kernelization(VVI &V) {
//    MyPQ<int> pq(V.size(),0);
//    for( int i=0; i<V.size(); i++ ){
//        if( V[i].size() > 0 ) pq.setPriority( i, -V[i].size() );
//        else pq.setPriority( i, -Constants::INF );
//    }
//
//    pair<VI,VPII> kernel;
//    while( pq.getPriority( pq.top() ) == -1 ){
//        int d = pq.top();
//
//        kernel.first.push_back(d);
//
//        if( V[d].size() != 1 ){
//            cerr << "V[" << d << "].size() != 1 in deg1 kernelization, it cannot happen!" << endl;
//            DEBUG(V[d].size());
//            DEBUG(pq.getPriority(d));
//
//            exit(1);
//        }
//        d = V[d][0];
//
//        for( int p : V[d] ){
//            pq.setPriority( p, pq.getPriority(p)+1 );
//            if( pq.getPriority(p) == 0 ) pq.setPriority(p, -Constants::INF);
//            kernel.second.push_back( {d,p} );
//        }
//
//        GraphUtils::removeNodeFromGraph(V,d);
//        pq.setPriority(d, -Constants::INF);
//    }


    pair<VI,VPII> kernel;
    VI neigh;
    VI perm = CombinatoricUtils::getRandomPermutation(V.size());
    for(int i : perm){
        if( V[i].size() == 1 ){
            neigh.push_back(i);
        }
    }

    for( int i=0; i<neigh.size(); i++ ){
        int p = neigh[i];
        if( V[p].empty() ) continue;

        int d = V[p][0];


        kernel.first.push_back(d);

        for( int x : V[d] ){
            GraphUtils::removeEdge( V, x,d, true );
            kernel.second.push_back( {x,d} );
        }

        for( int x : V[d] ){
            if( V[x].size() == 1 ) neigh.push_back(x);
        }

        V[d].clear();
    }

    return kernel;
}

pair<VI, VPII> KernelizerVC::dominationRule(VVI &V, int maxDegree) {



    pair<VI,VPII> res;
    VB neigh(V.size(),false);
    VI perm = CombinatoricUtils::getRandomPermutation(V.size());

    for( int i : perm){

        if( V[i].size() >= 2 && V[i].size() <= maxDegree ){  // CAUTION, THIS IF IS DONE ASSUMING THAT DEG1-kernlization was done before as in initialKernelization().


            for( int d : V[i] ) neigh[d] = true;
            neigh[i] = true;

            for( int d : V[i] ){



                bool isDominated = true; // will be tru if d is dominated by i
                for( int p : V[d] ){
                    if( !neigh[p] ){
                        isDominated = false;
                        break;
                    }
                }

                if( isDominated ){


                    res.first.push_back(i);
                    for( int p : V[i] ){
                        res.second.push_back( {i,p} );
                        neigh[p] = false;
                    }
                    neigh[i] = false;

                    GraphUtils::removeNodeFromGraph(V, i);
                    break;
                }

            }

            for( int t : V[i] ) neigh[t] = false;
            neigh[i] = false;
        }
    }






    // THIS VERSION SHOULD IN THEORY BE A BIT FASTER BUT IN PRACTICE IT ISN'T
    /*pair<VI,VPII> res;
    VB neigh(V.size(),false);
    VI perm = CombinatoricUtils::getRandomPermutation(V.size());

    deque<int> nodesToCheck;
    VI stateChanged(V.size(),false);
    unordered_map<PII,int, pairhash> sizeOfNeighborsInLastCheck;

    for( int i : perm ){
        if( V[i].size() <= maxDegree ){
            nodesToCheck.push_back(i);
            stateChanged[i]  = true;
        }
        for( int d : V[i] ) sizeOfNeighborsInLastCheck[ {i,d} ] = V[d].size();
    }


    while( !nodesToCheck.empty() ){
        int i = nodesToCheck.front();
        nodesToCheck.pop_front();

        if( !stateChanged[i] ) continue;
        stateChanged[i] = false;

//            cerr << "\rchecking domination, " << (100*i /V.size()) << "%" << flush;

            for( int d : V[i] ) neigh[d] = true;
            neigh[i] = true;

            for( int d : V[i] ){

                if( sizeOfNeighborsInLastCheck[ {i,d} ] == V[d].size() ) continue;
                sizeOfNeighborsInLastCheck[ {i,d} ] = V[d].size();


                bool isDominated = true; // will be tru if d is dominated by i
                for( int p : V[d] ){
                    if( !neigh[p] ){
                        isDominated = false;
                        break;
                    }
                }

                if( isDominated ){

//                    cerr << "Node " << i << " dominates " << d << endl;
//                    DEBUG(V[i]);
//                    DEBUG(V[d]);
//                    ENDL(1);

                    res.first.push_back(i);
                    for( int p : V[i] ){
                        res.second.push_back( {i,p} );
                        neigh[p] = false;
                    }
                    neigh[i] = false;

                    GraphUtils::removeNodeFromGraph(V,i);

                    // since d was dominated by i, one edge of V[d] is removed. It may happen now, that some neighbors of d dominates d now.
                    for( int t : V[d] ){
                        nodesToCheck.push_back(t);
                        stateChanged[t] = true;
                    }

                    break;
                }

            }

            for( int t : V[i] ) neigh[t] = false;
            neigh[i] = false;

    }*/


    return res;
}

