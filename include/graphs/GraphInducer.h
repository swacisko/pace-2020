//
// Created by sylwester on 8/8/19.
//

#ifndef ALGORITHMSPROJECT_GRAPHINDUCER_H
#define ALGORITHMSPROJECT_GRAPHINDUCER_H

#include "Makros.h"


struct InducedGraph{
    VVI *par; // parent graph;
    VI nodes; // this is a vector of vertices that induce the graph.
    // vertex with number nodes[i] has in induced graph number i.

    unordered_map<int,int> perm; // perm[t] is the number d such that nodes[d] = t; E.g. if graph is induced by [2,8,5] then perm[8] = 1.  So perm[ nodes[i] ] = i for i in [ 0,SIZE(V) ) and
    // nodes[ perm[i] ] = i for i in {nodes[0], nodes[1], ..., nodes.back() }

    VPII edges; // this is a vector of edges that induce a graph. It may be empty if the graph is induced by nodes
    VVI V; // induced graph

    void write(){
        cerr << "Graph induced by: " << flush; WRITE(nodes);
        if( !edges.empty() ){
            cerr << "induced by edges:" << endl;
            REP(i,SIZE(edges)) cerr << WRP(edges[i]) << endl;
        }
        WRITE_ALL( V, "Graph structure",0 );

    }
};


class GraphInducer{
public:

    // return graph induced by given nodes. Works for directed graphs as well (V can be directed).
    static InducedGraph induce( VVI & V, VI & nodes );

    // returns graph induced by given edges. Works for directed graphs (V can be directed) as welll
    // if directed == true then each edge in edges will be treated as directed edge. Otherwise it will be treated as undirected, bidirectional edge.
    static InducedGraph induce( VVI & V, VPII & edges, bool directed = false );

    /**
     * Function induces many graphs at once. if colors[i] = k, then node i will be in k-th induced graph.
     * @param V
     * @param colors
     * @param WILDCARD_COLOR  wildcard color is a color for nodes that are to be treated as every possible color. Using it we can include selected nodes into all induced graphs.
     * Nodes with WILDCARD_COLOR color will be in all induced graphs, that contain at least one node with different color. If all nodes will have WILDCARD_COLOR, than no graph will be induced
     * nodes with WILDCARD_COLOR will be included only to graphs, that
     * @return vector containing induced graphs. In k-th induced graphs will be all nodes v that have colors[v] = k
     */
    static vector<InducedGraph> induceGraphs(VVI &V, VI &colors, const int WILDCARD_COLOR = -1);

    void testEdges(){
        int N,M;
        cin >> N >> M;
        VVI V(N);
        int a,b;
        REP(i,M){
            cin >> a >> b;
            V[a].PB(b);
            V[b].PB(a);
        }

        int K;
        cin >> K;
        VPII edges(K);
        REP(i,K) cin >> edges[i].ST >> edges[i].ND;

        InducedGraph g = induce( V, edges,false );
        g.write();

        /*

        9 12
        0 1
        0 2
        1 2
        1 3
        2 3
        2 4
        3 4
        3 5
        3 6
        5 6
        3 7
        7 8
        10
        2 4
        2 3
        3 1
        3 5
        5 6
        6 3
        7 3
        7 8
        1 0
        1 2

        */
    }

    void testNodes(){
        int N,M;
        cin >> N >> M;
        VVI V(N);
        int a,b;
        REP(i,M){
            cin >> a >> b;
            V[a].PB(b);
            V[b].PB(a);
        }

        int K;
        cin >> K;
        VI nodes(K);
        REP(i,K) cin >> nodes[i];

        InducedGraph g = induce( V, nodes );
        g.write();

        /*

        9 12
        0 1
        0 2
        1 2
        1 3
        2 3
        2 4
        3 4
        3 5
        3 6
        5 6
        3 7
        7 8
        5
        3 1 0 4 8

        */
    }


};

#endif //ALGORITHMSPROJECT_GRAPHINDUCER_H
