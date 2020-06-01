//
// Created by sylwester on 5/13/20.
//

#include <CONTESTS/PACE20/TotalPivotMaker.h>
#include <utils/StandardUtils.h>
#include <graphs/components/ConnectedComponents.h>
#include <CONTESTS/PACE20/DepthTreePivotMaker.h>
#include <CONTESTS/PACE20/Pace20Params.h>

#include "CONTESTS/PACE20/TotalPivotMaker.h"

TotalPivotMaker::TotalPivotMaker(DepthTree &dt, int rD) : dt(dt) {
    V = dt.V;
    N = V->size();
    inCmp = VB(N,false);
    recDepth = rD;
}

vector<TotalPivotMaker::InducedSubtreeData>  TotalPivotMaker::getInducedSubgraphs(VVI &tree, vector<DepthTree::StretchStructureNode> &data, int root) {
    const bool debug = false;


    VI allNodes;
    function< void (int) > getAllNodes = [=,&getAllNodes, &tree, &data, &allNodes](int num){
        StandardUtils::append( allNodes, data[num].sepNodes );
        int BEG = ( num == 0 ? 0 : 1 );
        for( int i=BEG; i<tree[num].size(); i++ ) getAllNodes( tree[num][i] ); // omitting the first node - it is parent of root
    };
    getAllNodes(root);

    if(debug){ logSpacing(); DEBUG(tree); logSpacing(); DEBUG(root); logSpacing(); DEBUG(allNodes); }

    VVI comps;
    {
        InducedGraph g = GraphInducer::induce(*V, allNodes);
        comps = ConnectedComponents::getConnectedComponents(g.V); // connected components in induced subgraph
        for (VI &v : comps) for (int &d : v) d = g.nodes[d]; // remapping comps to original graph V
    }


    vector<TotalPivotMaker::InducedSubtreeData> res;
    res.reserve(comps.size());

    for( VI& cmp : comps ){ // now creating all connected components and their induced depth trees
        InducedGraph g = GraphInducer::induce( *V,cmp );
        DepthTree t(g.V);
        for(int p : cmp) inCmp[p] = true;

        for( int p : cmp ){
            int a = g.perm[p];
            int b = dt.par[p];
            if( b != -1 && inCmp[b] == false ) b = -1;
            if( b != -1 ) b = g.perm[b];
            else t.root = a;
            t.par[a] = b;
        }

        t.height = t.calculateHeight();

        if(debug){
            logSpacing(); cerr << "Adding induced tree: " << t << endl;
        }

        assert( t.isCorrect() );


        res.push_back( InducedSubtreeData(t,g) );

        for(int p : cmp) inCmp[p] = false;
    }

//    exit(1);
    return res;
}



DepthTree TotalPivotMaker::makePivots() {
    const bool debug = false;
    const int INIT_H = dt.height;

    if( debug ){
        logSpacing(); DEBUG(dt);
    }

    auto strStr = dt.getStretchStructure();
    VVI tree = strStr.first;
    int T = tree.size();
    auto data =strStr.second;
    for( int i=0; i<T; i++ ) sort( ALL(tree[i]), [&data]( int a, int b ){ return data[a].height > data[b].height; } );

    if(debug){
        logSpacing();
        DEBUG(tree);
        for(int i=0; i<T; i++){
            logSpacing(); DEBUG(data[i]);
        }
    }

    VI nodeOrder(T,0);
    iota(ALL(nodeOrder),0);


    { // create nodeOrder
        VI depth(T,0);
        function< void(int) > dfs = [=,&nodeOrder,&depth, &tree, &dfs](int num){
//            DEBUG(num);
            if( num == 0 ) depth[num] = 0;
            else depth[num] = 1 + depth[ tree[num][0] ];

            int BEG = ( num == 0 ? 0 : 1 );
            for( int i=BEG; i<tree[num].size(); i++ ) dfs(tree[num][i]);
        };
        dfs(0);

        if( debug ){logSpacing();  DEBUG(depth); logSpacing();  DEBUG(nodeOrder); }
        sort( ALL(nodeOrder), [&depth](int a, int b){ return depth[a] < depth[b]; } );
        while( depth[ nodeOrder.back() ] > MAX_DEPTH ) nodeOrder.pop_back();
        if( debug ){ logSpacing(); DEBUG(depth); logSpacing(); DEBUG(nodeOrder); }
    }


    while( !nodeOrder.empty() ){
        int root = nodeOrder.back();
        int ROOT_PAR = ( root == 0 ? -1 : data[ tree[root][0] ].sepNodes.back() );

        if( debug ){ ENDL(2);logSpacing();  DEBUG(root); logSpacing();  DEBUG(ROOT_PAR); }

        nodeOrder.pop_back();

        auto subgraphs = getInducedSubgraphs( tree,data, root );

        for( auto & sg : subgraphs ){
            sg.tree.V = &sg.g.V; // IMPORTANT!! UPDATING POINTERS!

            if(debug){
                ENDL(1);
                logSpacing(); cerr << "considering subgraph with nodes: " << sg.g.nodes << "   and induced depth-tree: " << sg.tree << endl;
            }

            auto pivotDt = DepthTreePivotMaker::makeAllPivots( sg.tree );

            if( pivotDt.height < sg.tree.height ) {
                if (debug && pivotDt.height < sg.tree.height) {
                    logSpacing();
                    cerr << "pivotDt.height = " << pivotDt.height << " < " << sg.tree.height
                         << " = sg.tre.height, starting new TotalPivotMaker" << endl;
                    logSpacing(); DEBUG(pivotDt);
                    logSpacing(); DEBUG(sg.tree);

//                    exit(1);
                }

                TotalPivotMaker totMaker(pivotDt, recDepth+1);
                pivotDt = totMaker.makePivots();

                if(debug){
                    logSpacing();
                    cerr << "after recursive pivots, pivotDt = " << pivotDt << endl;
                }

                if (debug){
                    logSpacing(); cerr << "updating dt at root = " << root << ": " << dt << endl;
                }


                for (int i = 0; i < sg.g.V.size(); i++) {
                    int a = sg.g.nodes[i];
                    int b = pivotDt.par[i];
                    if (b == -1) b = ROOT_PAR;
                    else b = sg.g.nodes[b];
                    dt.par[a] = b;
                }

                dt.height = dt.calculateHeight();

                if (debug){
                    logSpacing(); cerr << "dt updated:" << dt << endl;
                }

                assert( dt.isCorrect() );

//                exit(1);
            }
        }

//        if(debug) cerr << "WTF2?" << endl;
    }

//    if(debug) cerr << "almost exiting totalPivotMAker.makePivots()" << endl;

    dt.height = dt.calculateHeight();

    assert( dt.height <= INIT_H );
    return dt;
}




void TotalPivotMaker::test() {

    if(true){ // hall set pivots test

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

        Pace20Params::inputGraphSize = V.size();
//        Pace20Params::quickAndWeakTreeCreation = false;

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

        TotalPivotMaker tpm(dt,0);
        dt = tpm.makePivots();
    }

    exit(1);
}

