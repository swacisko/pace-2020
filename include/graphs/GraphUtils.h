/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_GRAPHUTILS_H
#define ALGORITHMSPROJECT_GRAPHUTILS_H

#include "Makros.h"

class GraphUtils {
public:


    /**
     *
     * @param V
     * @param nodes
     * @return all nodes that are not in @nodes
     */
    static VI getComplimentaryNodes( VVI & V, VI & nodes );

    /**
     * Colors are integers numbered from 0. There greatest value of a color will be at most D, where D is the greatest degree in V (that is pessimistically (D+1)-coloring of V).
     * @param V
     * @return random coloring of graph V. In the returned vector res, value res[i] is the color of vertex i in V.
     */
    static VI getRandomColoring( VVI & V);


    static VPII getGraphEdges( VVI & V );

    /**
     *
     * @return number of edges of graph V
     */
    static int countEdges(VVI & V);


    /**
     * Function finds the neighborhood of S
     * @param V
     * @param S
     * @param useSet if true, then finding neighborhood is done using set - neighbors cannot occur multiple time. This is to ensure that it needs D*logD time, where D
     * is the sum of outdegrees of nodes in S. If false, then an array of size V.size() is constructed and then neighborhood is found in time D (altogether N+D)
     * @return neighborhood of the set S
     */
    static VI getNeighborhood( VVI & V, VI & S, bool useSet = true );

    /**
     *
     * @param V
     * @param A
     * @param B
     * @param useSet if true, then i will use set to store neighbors, otherwise i will use ar array of size V.size()
     * @return intersection of neighborhood of A with the set B.
     */
    static VI getNeighborhoodOfAInB( VVI & V, VI & A, VI & B, bool useSet = true );

    /**
     * Function transposes and returns the graph. Transosed graph is just reversed graph
     * @param v
     * @return
     */
    static VVI transposeGraph(VVI &v);

    /**
     *
     * @param V
     * @return reversed graph of V, that is (a,b) is an edge in V iff (b,a) is in the returned graph
     */
    static VVI reverseGraph(VVI & V){ return transposeGraph(V); }


    /**
     *
     * @param V
     * @param mis
     * @return true if @{mis} is a maximal independent set in @{V}
     */
    static bool isMaximalIndependentSet( VVI & V, VI & mis );

    /**
     * Functions adds edge (a,b) to graph V
     * @param V
     * @param a
     * @param b
     * @param directed if true, then added is only (a,b), otherwise also (b,a)
     */
    static void addEdge( VVI & V, int a, int b, bool directed = false );

    template<class wtype>
    static void addEdge( VVI& V, vector< vector<wtype> > & W, int a, int b, wtype w, bool directed = false ){
        V[a].push_back(b);
        W[a].push_back(w);
        if(!directed){
            V[b].push_back(a);
            W[b].push_back(w);
        }

        cerr << "GraphUtils::addEdge not tested yet" << endl;
        exit(1);
    }

    /**
     * Functions removes edge (a,b) from graph V. Order of neighbors of a (and b if directed == false) is changed.
     * Function terminates after removing first occurence of edge (a,b) (and (b,a) if directed == false)
     * @param V
     * @param a
     * @param b
     * @param directed if true, then removed is only (a,b), otherwise also (b,a)
     */
    static void removeEdge( VVI & V, int a, int b, bool directed = false );

    /**
     * Removes given set of edges from the graph
     * @param V
     * @param edges
     * @param directed
     */
    static void removeEdges(VVI& V, VPII& edges, bool directed = false);

    template<class wtype>
    static void removeEdge(VVI& V, vector<vector<wtype> > &W, int a, int b, bool directed = false){
        auto rem = [ &V, &W ](int a, int b){
            for( int i=(int)V[a].size()-1; i>=0; i-- ){
                if( V[a][i] == b ){
                    swap(V[a][i], V[a].back() );
                    V[a].pop_back();
                    break;
                }
            }
        };

        rem(a,b);
        if( !directed ) rem(b,a);

        cerr << "GraphUtils::removeEdge not tested yet" << endl;
        exit(1);
    }

    /**
     *
     * @param V
     * @param a
     * @param b
     * @return true if V contains edge {a,b}, false otherwise
     */
    static bool containsEdge( VVI & V, int a, int b );

    /**
     * Function removes @{neighbors} from the adjacency list V[a].
     * @param V
     * @param a
     * @param neighbors
     */
    static void removeNeighborsOfNode( VVI & V, int a, unordered_set<int> neighbors);

    /**
     * Function removes node a from the DIRECTED graph
     * @param V
     * @param a
     */
    static void removeNodeFromGraph(VVI &V, int a);

    template<class wtype>
    static void removeNodeFromGraph(VVI &V, vector<vector<wtype>>& W, int a) {
        for( int d : V[a] ) removeEdge( V,W,d,a,true );
        V[a].clear();
        W[a].clear();

        cerr << "GraphUtils::removeNodeFromGraph not tested yet" << endl;
        exit(1);
    }
    /**
     *
     * @param V
     * @param nodes nodes to remove
     */
    static void removeNodesFromGraph(VVI &V, VI nodes);

    /**
     * The same as @{removeNodesFromGraph(VVI,VI)} but also removes weights from given vector W
     * @param V
     * @param W
     * @param nodes
     */
    static void removeNodesFromGraph(VVI &V, VVI& W, VI nodes);

    /**
     *
     * @param V
     * @return number of nodes with degree in closed range [d,D]
     */
    static int countNodesWithDegree( VVI & V, int d, int D );

    /**
     * Function sorts neighborhood of each node in given graph.
     * Works in O(E).
     * @param V
     * @return
     */
    static VVI sortNodeNeighborhoods( VVI & V );

    /**
     * Function merges node a into node b, that is a is removed from graph, but all neighbors d of a now become neighbors of b. No duplicates.
     * @param V
     * @param a
     * @param b
     */
    static void mergeNodeToNode( VVI& V, int a, int b );

    /**
     * Contract edge of given graph, that is uses mergeNodeToNode(V,a,b) - node a is merged to node b.
     * @param V
     * @param a
     * @param b
     */
    static void contractEdge(VVI& V, int a, int b);

    static bool isConnected( VVI& V );

    static void writeGraphHumanReadable(VVI& V);

    static void writeBasicGraphStatistics(VVI& V);
};


#endif //ALGORITHMSPROJECT_GRAPHUTILS_H
