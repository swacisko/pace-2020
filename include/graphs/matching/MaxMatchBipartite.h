/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_MAXMATCH_H
#define ALGORITHMSPROJECT_MAXMATCH_H

#include "Makros.h"

/**
 * Class responsible for finding a maximum matching in a bipartite graph. The bipartite graph in question will be taken as induced bipartite graph with specified bipartition.
 *
 * CAUTION. Currenly its running time is N*E. It needs to be reimplemented for bfs and multiple augmenting paths per iteration.
 */
class MaxMatchBipartite {

public:

    /**
     * Function looks for a maximal matching in graph G. A maximal matching with minimal found size will be returned.
     * FUNCTION WORKS   NOT ONLY    IN BIPARTITE GRAPHS. But will probably give results far from optimal
     * @param G
     * @return random maximal matching of G in the form res[i] is the index of matched neighbour or -1 if it is not matched
     */
    VI getMaximalMatchingOfMinimalSizeRandom(VVI &G, int iterations);

    /**
     *
     * @param G
     * @param bipartition
     * @return random maximal matching of G with given bipartition in the form res[i] is the index of matched neighbour or -1 if it is not matched
     */
    VI getMaximalMatchingRandom(VVI & G, VB& bipartition);

    /**
     * This function finds maximum matching in graph @G with given bipartition. In the matching there will be only edges between nodes a and b
     * in G that have bipartition[a] != bipartition[b].
     * Function checks, whether all nodes with bipartition[i] = false can be matched - useing alternating paths implemented poorly and slowly O(N*E) with dfs.
     * @param G
     * @param bipartition
     * @param fastSearch if true (as default), then i look for a maximal set of disjoint augmenting paths, otherwise i look for a path starting from single node, and that is much slower.
     * @return maximum matching of G in the form res[i] is the index of matched neighbour or -1 if it is not matched
     */
    VI getMaximumMatchingInBipartition(VVI &G, VB &bipartition, bool fastSearch = true);



    /**
     * Does the same as the above function but given an initial matching - this may be used e.g. to find approximation of MAX_WEIGHT matching, by initializing initial matching to
     * greedily found matching (with edges sorted by their weights).
     * @param G
     * @param bipartition
     * @param initialMatching
     * @param fastSearch
     * @return
     */
    VI getMaximumMatchingInBipartition(VVI &G, VB &bipartition, VI & initialMatching, bool fastSearch = true);





    /**
     * Function creates and returns a vector containing a maximal set of disjoint augmenting paths.
     * @param bipartition
     * @param matching
     * @return
     */
    VVI getMaximalSetOfDisjointAugmentingPaths( VVI & B, VB & bipartition, VI & matching );



    /**
     * Converts given matching to a set of paris.
     * @param matching
     * @return vector of pairs containing matching
     */
    VPII convertToPairs( VI & matching );

    /**
     * Function returns a set S \ subset bipartition[false], this is a subset of bipartition part that has bipartition[v] = false for nodes, with property |N(S)| < |S|.
     * If there are multiple such set, the largest set is returned.
     *
     * Function works as follows: for each unmatched node with bipartition[v] = false we find a set of all nodes T \subset bipartition[true] that can be reached via alternating paths
     * @param V
     * @param bipartition
     * @param matching
     * @return
     */
    VI getMaximumHallViolator( VVI & V, VB & bipartition, VI & matching );


    /**
     * Function selects one random node that is not matched, and takes violator for this node
     * @param V
     * @param bipartition
     * @param matching
     * @return
     */
    VI getRandomMinimalHallViolator( VVI & V, VB & bipartition, VI & matching );



    /**
     * Function finds an augmenting path in graph G with given bipartition (look description of  @getMaximumMatchingInBipartition), that has a beginning in node beg, with given current
     * matching. Result is added to @path. @was is an array required to mark whether any augmented path was found since last search. Instead of resetting the whole was array, we just
     * increase the value of currentWas and proceed - this way there is no need to clear the whole tab each time.
     * @param G
     * @param bipartition
     * @param matching
     * @param path
     * @return true if a path was found, false otherwise
     */
    bool findAugmentingPath( VVI & G, int beg, VB& bipartition, VI& matching, VI& path, VI& was, int currentWas );

    /**
     * Function changes matching in @matching based in the augmenting path
     * @param matching
     * @param path
     */
    void applyAugmentingPath( VI& matching, VI& path );


    /**
     * Function changes order of matching based on the alternating path. This path need not to be augmenting. This function can be used to generate
     * random matchings based on the given matching. To see any changes, the alternating path must provide matching at least same lenght as there is, it cannot reduce the size of the
     * matching
     * @param matching
     * @param path
     */
    void applyAlternatingPath(VI& matching, VI & path);

private:

    /**
     * Function creates layer graph - from each unsaturated node with bipartition[i] == false we start creating layers with bfs and add edges to the layer graph.
     * Then we can in O(E) get a maximal set of disjoint augmenting paths.
     * @param G
     * @param bipartition
     * @param matching
     * @return layer graph ready for dfs search for maximal set of disjoint augmenting paths.
     */
    VVI createLayerGraph(VVI &G, VB &bipartition, VI &matching);

    /**
     * Function finds and returns a maximal set of disjoint augmenting paths in layer graph @layerG. This layer graph can be obtained with @createLAyerGraph function
     * @param layerG
     * @param bipartition
     * @param was helper array to remember if i can create augmenting path from given node. This is to ensure O(E) running time
     * @param p current node
     * @param augmentingPaths vector containing augmenting paths
     */
    bool
    getMaximalSetOfDisjointAugmentingPaths(VVI &layerG, VB &bipartition, VB &was, int p, VVI &augmentingPaths);


};


#endif //ALGORITHMSPROJECT_MAXMATCH_H
