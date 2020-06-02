/*
 * Copyright (c) 2020, Sylwester Swat
*/


#include <graphs/trees/LCABinaryLifting.h>
#include <Constants.h>

#include "graphs/trees/LCABinaryLifting.h"

LCABinaryLifting::LCABinaryLifting(VVI &tree, VVI & W, int root, function<int(int,int)> fun, int neutralElement ){
    adj = tree;
    n = adj.size();
    this->W = W;
    this->fun = fun;
    this->neutralElement = neutralElement;
    preprocess(root);
}


void LCABinaryLifting::dfs(int v, int p)
{
    tin[v] = ++timer;
    up[v][0] = p;

    for( int i=0; i<adj[v].size(); i++ ) if( adj[v][i] == p ){ upQuery[v][0] = fun(W[v][i],neutralElement); break; }


    for (int i = 1; i <= l; ++i){
        up[v][i] = up[up[v][i-1]][i-1];
        upQuery[v][i] = fun( upQuery[v][i-1], upQuery[ up[v][i-1] ][i-1] );
    }

    for (int u : adj[v]) {
        if (u != p)
            dfs(u, v);
    }

    tout[v] = ++timer;
}

bool LCABinaryLifting::is_ancestor(int u, int v)
{
    return tin[u] <= tin[v] && tout[u] >= tout[v];
}

int LCABinaryLifting::lca(int u, int v)
{
    if (is_ancestor(u, v))
        return u;
    if (is_ancestor(v, u))
        return v;
    for (int i = l; i >= 0; --i) {
        if (!is_ancestor(up[u][i], v))
            u = up[u][i];
    }
    return up[u][0];
}

void LCABinaryLifting::preprocess(int root) {
    tin.resize(n);
    tout.resize(n);
    timer = 0;
    l = ceil(log2(n));
    up.assign(n, vector<int>(l + 1));
    upQuery.assign(n, vector<int>(l + 1,neutralElement));
    dfs(root, root);
    this->root = root;
}


int LCABinaryLifting::query(int u, int v) {
    if( u == v ) return neutralElement;
    int res = neutralElement;

    if( !is_ancestor(u,v) ){
        int U = u, V = v;
        for( int i=l; i>=0; --i ){
            if( !is_ancestor(up[U][i],V) ){
                res = fun(res,upQuery[U][i]);
                U = up[U][i];
            }
        }
        res = fun(res,upQuery[U][0]);
    }

    if( !is_ancestor(v,u) ){
        int U = u, V = v;
        for( int i=l; i>=0; --i ){
            if( !is_ancestor(up[V][i],U) ){
                res = fun(res,upQuery[V][i]);
                V = up[V][i];
            }
        }
        res = fun(res,upQuery[V][0]);
    }

    return res;
}

int LCABinaryLifting::k_th_ancestor(int u, int k) {
    int res = u;
    int l = ceil(log2(n));

    k--;
    for (int i = l; i >= 0; --i) {
        if( (1<<i) <= k ){
            u = up[u][i];
            k -= (1<<i);
        }
    }
    if( u == root ) return -1;
    else return up[u][0];
}


void LCABinaryLifting::test(){

    int N = 7;
    VVI tree(N);
    VVI W(N);
    VPII edges = { {4,0},{4,1},{0,2},{0,3},{0,6},{1,5} };
    VI weights = { 1,2,3,4,5,6 };
    int root = 4;



    for( int i=0; i<N-1; i++ ){
        int a = edges[i].first;
        int b = edges[i].second;
        int w = weights[i];
        tree[a].push_back(b);
        W[a].push_back(w);

        tree[b].push_back(a);
        W[b].push_back(w);
    }



    DEBUG(tree);
    DEBUG(W);

//    function<int(int,int)> funMin = [](int a, int b){return min(a,b);};
//    LCABinaryLifting lcabl(tree,W,root, funMin,Constants::INF);



//    function<int(int,int)> funMax = [](int a, int b){return max(a,b);};
//    LCABinaryLifting lcabl(tree,W,root, funMax, -Constants::INF);

    function<int(int,int)> funSum = [](int a, int b){return a+b;};
    LCABinaryLifting lcabl(tree,W,root, funSum, 0);

//    function<int(int,int)> funProd = [](int a, int b){return a*b;};
//    LCABinaryLifting lcabl(tree,W,root, funProd, 1);


    DEBUG(lcabl.up);
    DEBUG(lcabl.upQuery);

    for( int i=0; i<N; i++ ){
        for( int j=0; j<N; j++ ){
            DEBUG(VI({i,j}));
            DEBUG(lcabl.lca(i,j));
            DEBUG( lcabl.query( i,j ) );
            ENDL(1);
        }

//        exit(1);

        ENDL(5);
    }


    exit(1);
}

