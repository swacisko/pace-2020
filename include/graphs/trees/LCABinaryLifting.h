//
// Created by sylwester on 10/15/19.
//

#ifndef ALGORITHMSPROJECT_LCABINARYLIFTING_H
#define ALGORITHMSPROJECT_LCABINARYLIFTING_H

#include "Makros.h"

/**
 * Class responsible for calculating LCA of given two nodes in a rooted tree.
 * It can also be used to calculate k-th ancestor of given node.
 * Works in O(lgN) time for each query.
 * Preprocessing requires O(NlgN) time and O(NlgN) memory.
 * Uses binary lifting (similar to Sparse Table) for each node.
 *
 * It can be easily adapted to compute minimum on a given path, a sum of weights of edges on given path, etc.
 */

class LCABinaryLifting{

public:

    /**
     * Constructor.
     * @param tree
     * @param W
     * @param root
     * @param fun
     * @param neutralElement neutral element for operations done by fun(). For sum queries it should be 0, for product queries it should be 1, for minimum it should be INF, for maximum -INF.
     */
    LCABinaryLifting( VVI & tree, VVI & W, int root, function< int(int,int) > fun, int neutralElement );

    /**
     * Works in O(1) time - based on dfs code.
     * @param u
     * @param v
     * @return true, if @{u} is an ancestor of @{v}  (u lies closer to the root than v).
     */
    bool is_ancestor(int u, int v);

    /**
     *
     * @param u
     * @return k-th ancestor of u, or -1 if it does not exists
     */
    int k_th_ancestor(int u, int k);

    /**
     * Works in O(lgN) time.
     * @param u
     * @param v
     * @return index of the node that is LCA of u and v.
     */
    int lca(int u, int v);

    /**
     * Works in O(lgN) time.
     * @param u
     * @param v
     * @return value for the query on path for u to v. Result is calculated using fun function passed to constructor.
     */
    int query( int u, int v );

    static void test();

    vector<vector<int>> up, upQuery;
private:

    int n, l; // n is the number of nodes, l is
    int neutralElement;
    VVI adj;
    VVI W;
    int root;

    /**
     * Function used to calculate queries for paths.
     * Supports operations the same way as Segment trees.
     */
    function<int(int,int)> fun;

    int timer;
    vector<int> tin, tout;

    void dfs(int v, int p);



    void preprocess(int root);


};

#endif //ALGORITHMSPROJECT_LCABINARYLIFTING_H
