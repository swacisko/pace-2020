/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include "graphs/components/ConnectedComponents.h"
#include "contests/pace20/ComponentTreeMerger.h"

#include "IntGenerator.h"
#include "StandardUtils.h"

ComponentTreeMerger::ComponentTreeMerger(VVI &V, Separator &sep, VVI &comps, vector<DepthTree> &subtrees, Config c) {

    this->V = &V;
    this->sep = &sep;
    this->comps = &comps;
    this->subtrees = &subtrees;
    this->RANDOM_REPS = 10;
    this->cnf = c;

    createSepGraph();
}

DepthTree ComponentTreeMerger::mergeComponents(int MERGE_TYPE, int reps) {
    this->MERGE_TYPE = MERGE_TYPE;
    this->RANDOM_REPS = reps;

    if( MERGE_TYPE == AUTOMATIC ){
        this->MERGE_TYPE= ComponentTreeMerger::LARGEST_FIRST;
        // if( comps->size() <= Pace20Params::CTMergerSmallSize ) this->MERGE_TYPE = ComponentTreeMerger::ALL_ORDERS;
        if( comps->size() <= cnf.ct_merger_small_size ) this->MERGE_TYPE = ComponentTreeMerger::ALL_ORDERS;
    }

    VI mergeOrder = getMergeOrder();
    DepthTree res = mergeForOrder(mergeOrder);

    return res;
}

int ComponentTreeMerger::getDTHeightForOrder(VI mergeOrder) {
    VB was(sep->nodes.size(),false);
    int H = 0;
    int t = 0;

    for( int c : mergeOrder ){
        for( int d : sepGraph[c] ){
            if( !was[d] ){
                t++;
                was[d] = true;
            }
        }

        H = max( H, t + (*subtrees)[ c - sep->nodes.size() ].height );
    }

    H = max( H, (int)sep->nodes.size() );
    return H;
}

VI ComponentTreeMerger::getMergeOrder() {

    VI bestOrder( comps->size() );
    iota(ALL(bestOrder),sep->nodes.size());

    switch( MERGE_TYPE ){

        case LARGEST_FIRST:{
            // sorting trees bu height. If there is more than one tree with the same height, we sort them by greatest number of neighbors in separator sep
            // sorting only by height is enough to ensure optimality of DepthTree height, but we additionally sort it (in case of equal heights) by number of neighbors in separator
            // sort( ALL(bestOrder), [=]( int a, int b ){
            sort( ALL(bestOrder), [&]( int a, int b ){
                if( (*subtrees)[ a - sep->nodes.size() ].height != (*subtrees)[ b - sep->nodes.size() ].height ){
                    return (*subtrees)[ a - sep->nodes.size() ].height > (*subtrees)[ b - sep->nodes.size() ].height;
                }else{
                    return sepGraph[a].size() > sepGraph[b].size();
                }
            } );

            VI temp = bestOrder;
            // sort( ALL(temp), [=]( int a, int b ){
            sort( ALL(temp), [&]( int a, int b ){
                    return (*subtrees)[ a - sep->nodes.size() ].height + sepGraph[a].size()   >   (*subtrees)[ b - sep->nodes.size() ].height + sepGraph[b].size();
            } );

            if( getDTHeightForOrder(temp) < getDTHeightForOrder(bestOrder) ){
                cerr << "sorting subtrees by height+nodes is better than just by height" << endl;
                bestOrder = temp;
            }

            break;
        }
        case RANDOM_PERMUTATIONS:{
            int bestHeight = getDTHeightForOrder( bestOrder );

            VI order = bestOrder;
            for( int r = 0; r < RANDOM_REPS; r++ ){
                // random_shuffle(ALL(order));
                IntGenerator rnd;
                StandardUtils::shuffle(order,rnd);
                int newHeight = getDTHeightForOrder( order );
                if( newHeight < bestHeight ){
                    bestHeight = newHeight;
                    bestOrder = order;
                }
            }

            break;
        }
        case ALL_ORDERS:{

            VI order = bestOrder;
            int bestHeight = getDTHeightForOrder(order);

            sort(ALL(order));
            while( next_permutation( ALL(order) ) ){
                int newHeight = getDTHeightForOrder( order );
                if( newHeight < bestHeight ){
                    bestHeight = newHeight;
                    bestOrder = order;
                }
            }

            break;
        }
    }

    return bestOrder;
}

DepthTree ComponentTreeMerger::mergeForOrder(VI mergeOrder) {
    DepthTree dt(*V);
    dt.root = -1;
    dt.height = getDTHeightForOrder(mergeOrder);

    VB was(sep->nodes.size(), false);

    VI visitTime(sep->nodes.size(), -1);
    int vis = 0;

    int prev = -1;


    auto it = max_element( ALL(mergeOrder), [&]( int a, int b ) {
        return (*subtrees)[a-sep->nodes.size()].par.size() < (*subtrees)[b-sep->nodes.size()].par.size();
    } );

    int maxSizeInd = it - mergeOrder.begin();

    swap( dt.par, (*subtrees)[ mergeOrder[maxSizeInd] - sep->nodes.size() ].par );

    for (int i = 0; i < mergeOrder.size(); i++) {

            int c = mergeOrder[i];

            int lastVisitTime = -1;
            int par = -1;

            for (int d : sepGraph[c]) {
                if (!was[d]) {
                    was[d] = true;
                    int remappedD = sep->nodes[d];

                    if (dt.root == -1) dt.root = remappedD;

                    dt.par[remappedD] = prev;
                    prev = remappedD;

                    visitTime[d] = vis;
                    vis++;
                }

                if (visitTime[d] > lastVisitTime) {
                    lastVisitTime = visitTime[d];
                    par = sep->nodes[d];
                }
            }

            DepthTree *subdt = &(*subtrees).at(c - sep->nodes.size());

            if( i == maxSizeInd ){ // for first tree we swap par - this (with the assumption that highest DTrees come from largest components) is to get NlogN instead of N^2 for overall merging
//                swap( dt.par, subdt->par );
                dt.par[subdt->root] = par; // ... and setting parent of subdt.root to last visited node (that is to prev)
            }else {

                dt.par.insert(ALL(subdt->par)); // adding all parent relations of the subtree...
                dt.par[subdt->root] = par; // ... and setting parent of subdt.root to last visited node (that is to prev)
            }

    }

    for( int i=0; i<sep->nodes.size(); i++ ){
        if(!was[i]){
            dt.par[ sep->nodes[i] ] = prev;
            prev = sep->nodes[i];
        }
    }

    assert(dt.root != -1);
    assert( dt.height == dt.calculateHeight() );

    return dt;
}

void ComponentTreeMerger::createSepGraph() {
    int n = sep->nodes.size();
    int N = n + comps->size();

    sepGraph = VVI(N);
    mapper = VI(V->size(),-1);

    for( int i=0; i<n; i++ ){
        int d = sep->nodes[i];
        mapper[d] = i;
    }

    VB was(n,false);

    for( int i = n; i<N; i++ ){
        int c = i-n;
        for( int p : (*comps)[c] ){
            mapper[p] = i;
            for(int d : (*V)[p]){

                int mappedV = mapper[d];
                if( mappedV >= 0 && mappedV < n && !was[mappedV] ){
                    sepGraph[i].push_back( mappedV );
                    sepGraph[mappedV].push_back(i);
                    was[mappedV] = true;
                }

            }

            for(int d : (*V)[p]){
                int mappedV = mapper[d];
                if( mappedV >= 0 && mappedV < n  ) was[mappedV] = false;
            }
        }
    }

}

void ComponentTreeMerger::test() {
    VVI V = { {5}, {5,8}, {8}, {8}, {8}, {0,1,6}, {5,7}, {6}, {1,2,3,4,9}, {8} };
    DEBUG(V);

    DepthTree t1(V);
    t1.height = 3;
    t1.root = 5;
    t1.par[t1.root] = -1;
    t1.par[6] = 5;
    t1.par[7] = 6;

    DepthTree t2(V);
    t2.height = 2;
    t2.root = 8;
    t2.par[8] = -1;
    t2.par[9] = 8;

    VI nodes = {0,1,2,3,4};
    Separator sep( V,nodes );

    VVI comps = { {5,6,7}, {8,9} };
    vector<DepthTree> subtrees = { t1,t2 };

    Config cnf{};
    ComponentTreeMerger merger(V, sep, comps, subtrees, cnf);
    merger.mergeComponents();


    exit(1);
}


