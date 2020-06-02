/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/separatorminimizers/SnapToNonpathNodesMinimizer.h>
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include <graphs/GraphReader.h>
#include <graphs/trees/Tree.h>
#include "graphs/GraphInducer.h"

Separator SnapToNonpathNodesMinimizer::minimizeSeparator(Separator sep) {

    VVI* V = sep.V;
    int N = V->size();

    if( Tree::isTree(*V) ) return sep;

    VVI paths = findPaths( *V );
//    DEBUG(paths);

    VPII pathEnds(N,{-1,-1}); // pathEnds[i] is a pair containing ends of a path on which i lies, or {-1,-1} is i does not lie on a path
//    unordered_set<int> pathEndNodes;
    for( VI& path : paths ){

        for( int i=1; i<path.size(); i++ ){
            pathEnds[ path[i] ] = { path[0], path.back() };

//            pathEndNodes.insert( path[0] );
//            pathEndNodes.insert( path.back() );
        }
    }

//    DEBUG(pathEnds);
//    DEBUG(pathEndNodes);

    // now we will find greedily dominating set of a graph - each node on a path must belong to some non-deg2 neighbor

//    VI endNodes(ALL(pathEndNodes));
//    int n = endNodes.size();

    VI deg2InSepNodes;
    for( int t : sep.nodes ) if( (*V)[t].size() == 2 ) deg2InSepNodes.push_back(t);
//    DEBUG(deg2InSepNodes);

    int c = deg2InSepNodes.size();


    unordered_map< int, unordered_set<int> > dominationGraph;
    for( int p : deg2InSepNodes ){
        dominationGraph[ pathEnds[p].first ].insert(p);
        dominationGraph[ pathEnds[p].second ].insert(p);
    }

//    DEBUG(dominationGraph);

    VVI comps = ConnectedComponents::getConnectedComponents(*V,sep.nodes);
    VI componentEdges(N,0); // componentEdges[i] is the number of edges in the component of the graph V \ sep.nodes, that contains i
    VB inComp(N,false);
    for( VI& cmp : comps ){
        for(int p : cmp) inComp[p] = true;
        int edges = 0;
        for( int p : cmp ){
            for(int d : (*V)[p]){
                if( inComp[d] ) edges++;
            }
        }
        edges >>= 1;
        for(int p : cmp){
            inComp[p] = false;
            componentEdges[p] = edges;
        }

//        DEBUG(cmp);
//        DEBUG(edges);
    }

//    DEBUG(componentEdges);


    VI dominatingSet;

    auto removeDominant = [&dominatingSet, &dominationGraph, &pathEnds](int dom){
//        cerr << "adding " << dom << " to dominating set" << endl;
        dominatingSet.push_back(dom);
        for( int p : dominationGraph[dom] ){
            int a = pathEnds[p].first;
            int b = pathEnds[p].second;

            if( a != dom ){
                dominationGraph[a].erase(p);
                if( dominationGraph[a].empty() ) dominationGraph.erase(a);
            }
            if( b != dom ){
                dominationGraph[b].erase(p);
                if( dominationGraph[b].empty() ) dominationGraph.erase(b);
            }

        }
        dominationGraph.erase(dom);
    };


    // REMOVE ALREADY EXISTING IN SEP.NODES DOMINATION NODES!

    auto selectBestDominant = [&dominationGraph, &componentEdges](){
        return (*max_element( ALL(dominationGraph),
                [&componentEdges]( auto it1, auto it2 ){
                    if( it1.second.size() != it2.second.size() ) return it1.second.size() < it2.second.size();
                    else return componentEdges[it1.first] < componentEdges[it2.first];
                }
                )).first;
    };


    for( int t : sep.nodes ){
        if( dominationGraph.count(t) ) removeDominant(t);
    }

    while( !dominationGraph .empty()){
        int v = selectBestDominant();
        removeDominant(v);

//        cerr << "adding best dominant: " << v << endl;
//        DEBUG(dominationGraph);
    }


    unordered_set<int> resNodes( ALL(sep.nodes) );
    for( int p : deg2InSepNodes ) resNodes.erase(p);
    resNodes.insert( ALL(dominatingSet) );
    VI nodes( ALL(resNodes) );


//    exit(1);
    sep.stats.originalGraphSize = V->size();
    sep.stats.originalGraphEdges = GraphUtils::countEdges(*V);
    sep.updatePointers(*V);

    sep.createSeparatorStats();
//    DEBUG(sep);
    Separator resSep( *sep.V, nodes );
    resSep.createSeparatorStats();
//    DEBUG(resSep);

    return resSep;
}

VVI SnapToNonpathNodesMinimizer::findPaths(VVI &V) {

    auto isPath = [&V](){
        int deg1 = 0, beg;
        for(int i=0; i<V.size(); i++){
            if( V[i].size() > 2 ) return false;
            if(V[i].size()==1){
                deg1++;
                beg = i;
            }
            if(deg1 > 2) return false;
        }
        if( deg1 != 2 || ConnectedComponents::getConnectedComponents(V).size() > 1 ) return false;
        return true;
    };

    if( isPath() ){
        VI res;
        for( int i=0; i<V.size(); i++ ){
            if( V[i].size() == 1 ){
                int p = i;
                int next = V[p][0];
                VI pth = {i};
                while( p != next ){
                    pth.push_back(next);
                    int oldNext = next;
                    for(int d : V[next] ) if( d != p ) next = d;
                    p = oldNext;
                }
                return { pth };
            }
        }

        return VVI();
    }


    VVI res;
    int N = V.size();
    VB was(N,false);

    for( int i=0; i<N; i++ ){
        if( V[i].size() == 2 && !was[i] ){
            int degGE3Neighbor = -1;
            for( int d : V[i] ){
                if( V[d].size() >= 3 ){
                    degGE3Neighbor = d;
                    break;
                }
            }

            if( degGE3Neighbor != -1 ){
                int p = degGE3Neighbor;
                int next = i;
                VI path = {p};
                while( V[next].size() == 2 ){
                    was[next] = true;
                    path.push_back(next);
                    for( int d : V[next] ){
                        if( d != p ){
                            p = next;
                            next = d;
                            break;
                        }
                    }
                }

                path.push_back(next);

//                DEBUG(path);

                res.push_back(path);
            }
        }
    }

    // now cycles (they were not visited)

    for( int i=0; i<N; i++ ){
        if( V[i].size()==2 && !was[i] ){
            was[i] = true;
            VI cycle = {i};
            int p = i;
            int next = V[i][0];
            while( V[next].size() == 2 && !was[next] ){
                was[next] = true;
                cycle.push_back(next);
                for( int d : V[next] ){
                    if( d != p ){
                        p = next;
                        next = d;
                        break;
                    }
                }
            }
            cycle.push_back(next);

//            DEBUG(cycle);

            res.push_back(cycle);

        }
    }


//    DEBUG(res);

//    exit(1);
    return res;
}



void SnapToNonpathNodesMinimizer::test() {

    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    SnapToNonpathNodesMinimizer minim;
    minim.findPaths(V);

    VI nodes = {1};
    Separator sep(V,nodes);

    minim.minimizeSeparator(sep);


    exit(1);
}