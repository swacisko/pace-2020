//
// Created by sylwester on 8/8/19.
//

#include <graphs/GraphInducer.h>

#include "graphs/GraphInducer.h"

InducedGraph GraphInducer::induce( VVI & V, VI & nodes ){

    InducedGraph g;
    g.nodes = nodes;
    g.par = &V;
    int N = SIZE(nodes);

    g.perm = unordered_map<int,int>();
    g.perm.reserve( nodes.size() * 2 );
    REP( i, N ) g.perm[ nodes[i] ] = i;

    g.V = VVI( nodes.size() );
    for( int i=0; i<nodes.size(); i++ ){
        for( int d : V[ nodes[i] ] ){
            auto it = g.perm.find(d);
            if( it != g.perm.end() ){
                int indD = it->second;
                g.V[ i ].push_back( indD );
            }
        }
    }

    return g;

}


vector<InducedGraph> GraphInducer::induceGraphs(VVI &V, VI &colors, const int WILDCARD_COLOR) {

    VPII wildcardEdges; // list of edges that are to be added to every induced graph - edges between two nodes with WILDCARD_COLOR
    VI wildcardNodes;
    for( int i=0; i<V.size(); i++ ){
        if( colors[i] == WILDCARD_COLOR ){
            wildcardNodes.push_back(i);
            for( int d : V[i] ){
                if( colors[d] == WILDCARD_COLOR ) wildcardEdges.push_back( {i,d} );
            }
        }
    }

    int C = *max_element(ALL(colors));
    vector<InducedGraph> graphs(C);
    VVI nodes(C+1);
    for( int i=0; i<colors.size(); i++ ){
        if( colors[i] != WILDCARD_COLOR ) nodes[ colors[i] ].push_back(i);
    }

    VI global_perm(V.size(),-1);

    for( int i=0; i<C; i++ ){
        InducedGraph &g = graphs[i];
        VI & _nodes = nodes[i];

        g.nodes = nodes[i];
        g.nodes.insert( g.nodes.end(), ALL(wildcardNodes) );
        g.perm = unordered_map<int,int>();
        g.perm.reserve( 2 * ( _nodes.size() + wildcardNodes.size() )  );
        g.par = &V;

        for( int j=0; j< _nodes.size(); i++ ){
            g.perm[ _nodes[j] ] = j;
            global_perm[ _nodes[j] ] = j;
        }

        // HERE I ADD WILDCARD NODES TO g
        for( int j=0; j<wildcardNodes.size(); j++ ){
            int w = wildcardNodes[j];
            g.perm[ w ] = _nodes.size() + j;
        }

        // HERE I ADD ALL EDGES BETWEEN TWO NODES WITH WILDCARD COLOR. I have to use g.perm map, because WILDCARD NODES can be in multiple graphs, and i cannot store that values in array
        for( PII e : wildcardEdges ){
            int a = g.perm[ e.first ];
            int b = g.perm[ e.second ];
            g.V[a].push_back(b);
        }
    }

    // HERE I ADD ALL EDGES WITH BEGIN IN NON-WILDCARD NODE
    for( int i=0; i<V.size(); i++ ){
        int c = colors[i];
        InducedGraph &g = graphs[c];
//        int indI = g.perm[i];
        int indI = global_perm[i]; // faster than map

        for( int d : V[i] ){
            if( colors[d] == WILDCARD_COLOR || colors[d] == c  ){
//                int indD = g.perm[d];


                int indD;
                if( colors[d] == WILDCARD_COLOR ) indD = g.perm[d];
                else indD = global_perm[d]; // faster than map

                g.V[ indI ].push_back( indD );
            }
        }
    }

    // HERE I ADD ALL EDGES FROM WILDCARD NODES TO NON-WILDCARD NODES
    for( int i : wildcardNodes ){
        for( int d : V[i] ){
            if( colors[d] != WILDCARD_COLOR ){
                int c = colors[d];
                InducedGraph &g = graphs[c];
                int a = g.perm[i];
//                int b = g.perm[d];
                int b = global_perm[d]; // faster

                g.V[a].push_back(b);
            }
        }
    }

}

InducedGraph GraphInducer::induce( VVI & V, VPII & edges, bool directed ){
    InducedGraph g;
    g.edges = edges;
    g.par = &V;

    g.perm = unordered_map<int,int>();
    g.perm.reserve( edges.size() / 4 );
    unordered_map<int,int>::iterator it;

    REP( i, SIZE(edges) ){
        int a = edges[i].ST;
        int b = edges[i].ND;
        //	cout << "adding edge " << a << "," << b << endl;

        it = g.perm.find(a);
        if( it == g.perm.end() ){
            g.nodes.PB( a );
            g.perm[a] = SIZE( g.nodes )-1;
            g.V.PB( V[a] );
        }

        it = g.perm.find(b);
        if( it == g.perm.end() ){
            g.nodes.PB( b );
            g.perm[b] = SIZE( g.nodes )-1;
            g.V.PB( V[b] );
        }
    }


    //	WRITE_ALL( g.V, "before relabelling:",0 );

    set< PII > edgeSet( ALL(edges) );
    REP( i, SIZE(g.V) ){
        REP( k, SIZE( g.V[i] ) ){

            int a = g.nodes[i];
            int b = g.V[i][k];
            bool isEdge = edgeSet.count( MP(a,b) );
            if( directed == false && isEdge == false ) isEdge = edgeSet.count( MP(b,a) );

            if( !isEdge ){
                swap( g.V[i][k], g.V[i].back() );
                g.V[i].pop_back();
                k--;
            }else{
                g.V[i][k] = g.perm[b];
            }


        }
    }

    return g;
}


