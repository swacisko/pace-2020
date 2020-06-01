//
// Created by sylwester on 10/15/19.
//

#ifndef ALGORITHMSPROJECT_MAXFLOWPUSHRELABEL_H
#define ALGORITHMSPROJECT_MAXFLOWPUSHRELABEL_H

#include "Makros.h"

/**
 * Push-relabel improved algorithm.
 * Works in O( VE + V^2 * sqrt(E) ) time.
 * Requires O( V^2 ) memory. It can be reduced to O(E) memory - but for this @{capacity} and @{flow} need to be changed to unordered_map<int,int>
 */
class MaxFlowPushRelabel{
public:

    /**
     * Constructor.
     * Structure of the graph must be given in the vector V. It may be directed graph.
     * Vector W contains weight of edges. W[i][k] is a weight of edge i -> V[i][k]
     * @param V
     * @param W
     */
    MaxFlowPushRelabel( VVI & V, VVI & W );

    /**
     * Calculates maximum flow in given graph from source s to sink t.
     * To extract exact flow in each edge, use getFlow() function.
     * @param s
     * @param t
     * @return value of the maximum flow.
     */
    int max_flow(int s, int t);

    VVI getFlow(){return flow;}

private:

    VVI V;

    const int inf = 1000000000;

    int n;
    vector<vector<int>> capacity, flow;
    vector<int> height, excess;

    void push(int u, int v);

    void relabel(int u);

    vector<int> find_max_height_vertices(int s, int t) ;



};


#endif //ALGORITHMSPROJECT_MAXFLOWPUSHRELABEL_H
