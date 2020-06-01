//
// Created by sylwester on 8/26/19.
//

#include <graphs/trees/Tree.h>
#include <functional>
#include <graphs/graphtraversals/DFS.h>

#include "graphs/trees/Tree.h"

Tree::Tree(VVI &V) {
    this->V = &V;
}

int Tree::getRoot() const {
    return root;
}

void Tree::setRoot(int root) {
    Tree::root = root;
}

int Tree::getPar() const {
    return par;
}

void Tree::setPar(int par) {
    Tree::par = par;
}

bool Tree::isTree(VVI &V) {
    VB was(V.size(),false);
    VI visitOrder(V.size());

    DFS::dfs( V,was,visitOrder, 0,0 );

    int edges = 0;
    for( int i=0; i<V.size(); i++ ){
        if( !was[i] ) return false;
        edges += V[i].size();
    }
    edges >>= 1;
    return edges == V.size()-1;
}


void Tree::test() {
    for(int i=0; i<100; i++){
        int N = 10'000;
        VVI V = GraphGenerator::getRandomTreePrufer(N);
//        DEBUG(V);
        if( Tree::isTree(V) == false ){
            cerr << "GRAPH NTO A TREE" << endl;
            exit(1);
        }
    }
    cerr << "PASSED" << endl;
    exit(0);

}


