//
// Created by sylwester on 3/28/20.
//

#include <graphs/flow/MinCostFlowOneWayDirected.h>

#include "graphs/flow/MinCostFlowOneWayDirected.h"


void MinCostFlowOneWayDirected::shortest_paths(int n, int v0, vector<int>& d, vector<int>& p) {
    d.assign(n, INF);
    d[v0] = 0;
    vector<bool> inq(n, false);
    queue<int> q;
    q.push(v0);
    p.assign(n, -1);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        inq[u] = false;
        for (int v : adj[u]) {
            if (capacity[u][v] > 0 && d[v] > d[u] + cost[u][v]) {
                d[v] = d[u] + cost[u][v];
                p[v] = u;
                if (!inq[v]) {
                    inq[v] = true;
                    q.push(v);
                }
            }
        }
    }
}

int MinCostFlowOneWayDirected::min_cost_flow(int N, vector<Edge> edges, int K, int s, int t) {
    adj.assign(N, vector<int>());
    cost.assign(N, vector<int>(N, 0));
    capacity.assign(N, vector<int>(N, 0));
    for (Edge e : edges) {
        adj[e.from].push_back(e.to);
        adj[e.to].push_back(e.from);
        cost[e.from][e.to] = e.cost;
        cost[e.to][e.from] = -e.cost;
        capacity[e.from][e.to] = e.capacity;
    }

    int flow = 0;
    int cost = 0;
    vector<int> d, p;
    while (flow < K) {
        shortest_paths(N, s, d, p);
        if (d[t] == INF)
            break;

        // find max flow on that path
        int f = K - flow;
        int cur = t;
        while (cur != s) {
            f = min(f, capacity[p[cur]][cur]);
            cur = p[cur];
        }

        // apply flow
        flow += f;
        cost += f * d[t];
        cur = t;
        while (cur != s) {
            capacity[p[cur]][cur] -= f;
            capacity[cur][p[cur]] += f;
            cur = p[cur];
        }
    }

    if (flow < K)
        return -1;
    else
        return cost;
}


int MinCostFlowOneWayDirected::getMinCostFlow(VVI &V, VVI &capacity, VVI &cost, int flowQuantity, int source, int end) {
    vector<Edge> edges;
    int N = V.size();
    for( int i=0; i<N; i++ ){
        for( int j=0; j<V[i].size(); j++ ){
            Edge e;
            e.from = i;
            e.to = V[i][j];
            e.capacity = capacity[i][j];
            e.cost = cost[i][j];
            edges.push_back(e);
        }
    }
    return min_cost_flow( N,edges,flowQuantity,source,end );
}


void MinCostFlowOneWayDirected::test(){

//    const int INF = 1e9;
//    VVI V = { {1,2,3,4,5}, {6,7}, {7,8}, {8,9}, {9,10}, {10,11}, {12}, {12}, {12}, {12}, {12}, {12}, {} };
//    VVI cap = { {INF,INF,INF,INF,INF}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1},{1},{1},{1},{1},{1},{} };
//    VVI cost = { {1,1,1,1,1}, {0,0}, {0,0}, {0,0}, {0,0}, };


    exit(1);
}

