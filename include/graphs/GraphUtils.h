//
// Created by sylwester on 8/8/19.
//

#ifndef ALGORITHMSPROJECT_GRAPHUTILS_H
#define ALGORITHMSPROJECT_GRAPHUTILS_H

// #include <combinatorics/sequences/Sorting.h>
#include "Makros.h"

class GraphUtils {
public:


    /**
     * @return all nodes that are not in @nodes
     */
    static VI getComplimentaryNodes( VVI & V, VI & nodes );

    static bool isBipartite(VVI & V);

    static bool isCycle(VVI & V);

    static bool hasCycle( VVI & V );
    static bool isForest(VVI & V){ return !hasCycle(V); }
    static bool isTree(VVI & V){ return isForest(V) && isConnected(V); }

    /**
     * Colors are integers numbered from 0. There greatest value of a color will be at most D, where D is the greatest
     * degree in V (that is pessimistically (D+1)-coloring of V).
     * @return random coloring of graph V. In the returned vector res, value res[i] is the color of vertex i in V.
     */
    static VI getRandomColoring( VVI & V);



    /**
     * @return number of edges of graph V
     */
    static int countEdges(VVI & V, const bool directed = false);

    template< class wtype>
    static int countEdges( vector< vector< pair<int,wtype> > >& V){
        int res = 0;
        for(auto& v : V) res += v.size();
        return res >> 1;
    }

    static VPII getGraphEdges( VVI & V, bool directed = false );
    static VPII getDirectedGraphEdges( VVI & V );

    /**
     * Creates a graph for given set of edges.
     */
    static VVI getGraphForEdges(VPII edges, bool directed = false);

    /**
     * Finds all edges in a graph. If the graphs is undirected, then only one of two edges (a,b) and (b,a) will be
     * returned, even if the have different weights in V. (to get both set [directed] to true).
     */
    template<class wtype>
    static vector<tuple<int,int,wtype>> getGraphEdges( vector< vector< pair<int,wtype> > >& V, bool directed = false ){
//        clog << "vector<tuple<int,int,wtype>> getGraphEdges(...) not tested yet" << endl;
        vector<tuple<int,int,wtype>> res;
        res.reserve( countEdges(V) );
        for( int i=0; i<V.size(); i++ ){
            for( auto & [d,w] : V[i] ){
                if(directed || d>i ) res.emplace_back( i,d,w );
            }
        }
        return res;
    }

    /**
     * Function finds the neighborhood of S
     * @param useSet if true, then finding neighborhood is done using set - neighbors cannot occur multiple time.
     * This is to ensure that it needs D*logD time, where D is the sum of outdegrees of nodes in S. If false, then an
     * array of size V.size() is constructed and then neighborhood is found in time D (altogether N+D)
     * @return neighborhood of the set S
     */
    static VI getNeighborhood( VVI & V, VI & S, bool useSet = true );

    /**
     *
     * @param useSet if true, then i will use set to store neighbors, otherwise i will use ar array of size V.size()
     * @return intersection of neighborhood of A with the set B.
     */
    static VI getNeighborhoodOfAInB( VVI & V, VI & A, VI & B, bool useSet = true );

    /**
     * Finds sets N(A) \ A. Uses (for speedup) helper array, that needs to be set to false.
     */
    static VI getNeighborhoodExclusive( VVI & V, VI & A, VB & helper );

    /**
     * Function transposes and returns the graph. Transosed graph is just reversed graph
     * @param v
     * @return
     */
    static VVI transposeGraph(VVI &v);

    /**
     * @return reversed graph of V, that is (a,b) is an edge in V iff (b,a) is in the returned graph
     */
    static VVI reverseGraph(VVI & V){ return transposeGraph(V); }

    /**
     * Function creates and returns complementary graph.
     */
    static VVI getComplimentaryGraph( VVI & V );


    /**
     * Functions adds edge (a,b) to graph V
     * @param V
     * @param a
     * @param b
     * @param directed if true, then added is only (a,b), otherwise also (b,a)
     */
    static void addEdge( VVI & V, int a, int b, bool directed = false );

    /**
     * Function checks if edge a->b already exists in the graph. If not, then it is added
     * @return true if edge was added, false otherwise
     */
    static bool addEdgeSafe( VVI & V, int a, int b, bool directed = false );

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

    template<class wtype>
    static void addEdge( vector< vector< pair<int,wtype> > >& V, int a, int b, wtype w, bool directed = false ){
        V[a].push_back( {b,w} );
        if(!directed){
            V[b].push_back( {a,w} );
        }
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
     * CAUTION! Vector [edges] is modified, if directed == false
     */
    static void removeEdges(VVI& V, VPII& edges, bool directed = false);
    static void removeEdges(VVI& V, VPII& edges, VB & helper, bool directed = false);

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

    template<class wtype>
    static VI getNeighborhood(vector< vector< pair<int,wtype> > > & V, VI &S, bool useSet) {
        VI res;
        if( useSet ){
            unordered_set<int> zb;
            for( int p : S ){
                for(auto d : V[p]){
                    zb.insert(d.first);
                }
            }
            res = VI(ALL(zb));
        }else{
            VB was(V.size(),false);
            for( int p : S ){
                for(auto d : V[p]){
                    if( !was[d.first] ){
                        was[d.first] = true;
                        res.push_back(d.first);
                    }
                }
            }
        }
        return res;
    }

    template<class wtype>
    static void removeNodesFromGraph( vector< vector< pair<int,wtype> > > & V, VI nodes ){
        unordered_set<int> nd(ALL(nodes));
        VI neigh = getNeighborhood( V,nodes,true );
        for( int t : neigh ){
            if( nd.count(t) ) continue;

            for( int i = (int)V[t].size()-1; i>=0; i-- ){
                int a = V[t][i].first;
                if( nd.count(a) ){
                    swap( V[t][i], V[t].back() );
                    V[t].pop_back();
                }
            }
        }

        for( int d : nodes ) V[d].clear();
    }


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

    template< class wtype>
    static void writeGraphHumanReadable( vector< vector< pair<int,wtype> > >& V){
        clog << "****" << endl;
        for( int i=0; i<V.size(); i++ ){
            clog << i << ": ";
            auto neigh = V[i];
            sort(ALL(neigh));
            for(auto d : neigh) clog << d << "  ";
            clog << endl;
        }
    }

    static void writeBasicGraphStatistics(VVI& V);

    /**
     * Finds the regularity of graph V. If all nodes have the same degree, then that degree is returned, otherwise -1.
     */
    static int regularity(VVI V);

    /**
     * Checks whether given graph is simple.
     */
    static bool isSimple(VVI V);

    /**
    * Makes graph simple, by removing all self-loops and all parallel edges.
    */
    static VVI makeSimple(VVI V);

    /**
    * Creates a graph isomorphic to given graph [V], by remapiing nodes:   i --> perm[i]
    */
    static VVI remapGraph( VVI V, VI perm );


    /**
     * Calculated and return density of a graph.
     * Density s calculated as number_of_edges (or arcs if [directed] is true) divided by (N*(N-1)/2)
     */
    static double density( VVI & V, bool directed );
};


#endif //ALGORITHMSPROJECT_GRAPHUTILS_H
