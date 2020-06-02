/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <CONTESTS/PACE20/SubtreeRerunnerImprover.h>
#include <CONTESTS/PACE20/depthtreecreators/DepthTreeCreatorLarge.h>
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include <graphs/GraphReader.h>
#include <graphs/trees/Tree.h>
#include <utils/StandardUtils.h>
#include <CONTESTS/PACE20/DepthTreePivotMaker.h>
#include "graphs/cliques/CliqueUtils.h"
#include "graphs/GraphInducer.h"

DepthTree SubtreeRerunnerImprover::improve(DepthTree &dt, double balance) {
    const bool debug = false;

    if(debug){
        ENDL(5);
        DEBUG(*dt.V);
        DEBUG(dt);
    }

    auto strStr = dt.getStretchStructure();
    VVI tree = strStr.first;
    int T = tree.size();
    auto data = strStr.second;

    if( debug ){
        DEBUG(tree);
        for(int i=0; i<T; i++) cerr << "data[" << i << "].sepNodes: " << data[i].sepNodes << endl;
    }

    for( int i=0; i<T; i++ ) sort( ALL(tree[i]), [&data]( int a, int b ){ return data[a].height > data[b].height; } );

    VI subtreeNodes;
    VI subtreeSizes(T,0);

    function< void(int,int) > calcSubSizes = [=,&tree,&data, &calcSubSizes, &subtreeSizes](int num, int par) {
        for( int d : tree[num] ) if(d != par) calcSubSizes(d,num);
        subtreeSizes[num] = data[num].sepNodes.size();
        for( int d : tree[num] ) if(d != par) subtreeSizes[num] += subtreeSizes[d];
    };

    calcSubSizes(0,0);

    if(debug) DEBUG(subtreeSizes);

    function< void(int,int) > getSubtreeNodes = [&getSubtreeNodes, &subtreeNodes, &data, &tree](int num, int par){
//        DEBUG(num);
        StandardUtils::append( subtreeNodes, data[num].sepNodes );
        for( int d : tree[num] ) if(d != par) getSubtreeNodes(d,num);
    };

    function< void(int,int) > getSubtreeNodes2 = [=,&tree,&T,&data, &getSubtreeNodes2, &getSubtreeNodes, &subtreeNodes, &subtreeSizes, &balance](int num, int par){
//        DEBUG(num);
        VI sons;
        for( int d : tree[num] ) if( d != par ) sons.push_back(d);


        int s = subtreeSizes[num];

//        DEBUG(s);
//        DEBUG((int)dt.V->size() * balance);

        if( sons.empty() || s < (int)dt.V->size() * balance ){
            getSubtreeNodes(num,par);
            return;
        }else{
            int son = sons[0];
            getSubtreeNodes2(son,num);
        }

    };

    getSubtreeNodes2(0,0);

    if(debug) DEBUG(subtreeNodes);

    VVI* V = dt.V;
    int root = subtreeNodes[0];
    int par = dt.par[root];

    InducedGraph g = GraphInducer::induce( *V, subtreeNodes );

    DEBUG(g.V.size());

    assert( GraphUtils::isConnected(g.V) );

    cerr << "assertion GraphUtils::isConnected(g.V) in SubtreeRerunnerImprover may fail, please consider each connected sugraph separately";
    exit(2);

    DepthTreeCreatorLarge dtcl(g.V,0);
    DepthTree dtree = dtcl.getDepthTree();

    auto newDt = dt;

    int prevHeight = 0;
    for( int i=0; i<T; i++ ) for( int d : data[i].sepNodes ) if( d == root ){ prevHeight = data[i].height; }

    if( dtree.height < prevHeight ){
        cerr << endl << "SubtreeRerunnerImprover found better subtree!" << endl;
        DEBUG(prevHeight);
        DEBUG(dtree);
        ENDL(1);

        for( PII p : dtree.par ){
            int a = g.nodes[ p.first ];
            int b = p.second;
            if( b == -1 ) b = par;
            else b = g.nodes[b];

            newDt.par[a] = b;
        }

        newDt.height = newDt.calculateHeight();

        newDt = DepthTreePivotMaker::makeAllPivots(newDt);

        assert( newDt.isCorrect() );

        DEBUG(dt);
        DEBUG(newDt);

        cerr << "Improving again!" << endl;
        newDt = improve( newDt, balance );
    }

//    exit(1);

    if( newDt.height < dt.height ) return newDt;
    else return dt;
}

void SubtreeRerunnerImprover::test() {

    {
        VVI V = {
                {1,2,7,8,15},   // 0
                {0,2,4,15},     // 1
                {0,1,3,7,8,14}, // 2
                {2,7,8,12,13},  // 3
                {1,5,6},        // 4
                {4,6,7},        // 5
                {4,5,7,8,11},   // 6
                {0,2,3,5,8,11}, // 7
                {0,2,3,6,7,9,10}, // 8
                {8,10},         // 9
                {8,9},          // 10
                {6,7},          // 11
                {3,13},         // 12
                {3,12},         // 13
                {2},            // 14
                {0,1,16,17},    // 15
                {15},           // 16
                {15}            // 17
        };

        DepthTree dt(V);
        dt.root = 0;
        dt.height = 11;
        dt.par = {
                {0,-1},
                {1,0},
                {2,1},
                {3,2},
                {4,3},
                {5,4},
                {6,5},
                {7,6},
                {8,7},
                {9,8},
                {10,9},
                {11,7},
                {12,3},
                {13,12},
                {14,2},
                {15,1},
                {16,15},
                {17,15}
        };

        double balance = 0.75;

        SubtreeRerunnerImprover improver;
        improver.improve(dt, balance);
    }

    exit(1);
}

