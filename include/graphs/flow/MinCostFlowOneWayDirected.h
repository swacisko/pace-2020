//
// Created by sylwester on 3/28/20.
//

#ifndef ALGORITHMSPROJECT_MINCOSTFLOW_H
#define ALGORITHMSPROJECT_MINCOSTFLOW_H

#include "Makros.h"


/**
 * CAUTION!! Works only for directed graphs in which for each pair of vertices (a,b) there must be only one edge, either a->b or b->a
 *
 * Code form CP-algorithms
 * works in O( N^2 * M^2 )
 */
class MinCostFlowOneWayDirected{
    struct Edge
    {
        int from, to, capacity, cost;
    };

public:

    /**
     * Finds flow of value exactly flowQuantity (or maximal flow if no flow of flowQuantity is possible) with minimum cost of that flow.
     * edge a -> V[a][i] has capacity capacity[a][i] and cost per unit cost[a][i]
     * @param V
     * @param capacity
     * @param cost
     * @param flowQuantity
     * @return value of the flow
     */
    int getMinCostFlow(VVI &V, VVI &capacity, VVI &cost, int flowQuantity, int source, int end);



    VVI cost, capacity;
    static void test();

private:

    VVI adj;
    const int INF = 1e9;

    void shortest_paths(int n, int v0, vector<int>& d, vector<int>& p);

    int min_cost_flow(int N, vector<Edge> edges, int K, int s, int t);


};

#endif //ALGORITHMSPROJECT_MINCOSTFLOW_H
