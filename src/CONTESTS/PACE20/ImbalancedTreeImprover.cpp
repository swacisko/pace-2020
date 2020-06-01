//
// Created by sylwester on 4/24/20.
//

#include <CONTESTS/PACE20/ImbalancedTreeImprover.h>
#include <CONTESTS/PACE20/depthtreecreators/DepthTreeCreatorLarge.h>
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include <graphs/GraphReader.h>
#include <graphs/trees/Tree.h>
#include <utils/StandardUtils.h>
#include "graphs/cliques/CliqueUtils.h"
#include "graphs/GraphInducer.h"

DepthTree ImbalancedTreeImprover::improve(DepthTree &t) {
    dt = &t;
    auto strStr = dt->getStretchStructure();
    VVI tree = strStr.first;
    int T = tree.size();
    auto data = strStr.second;

    for( int i=0; i<T; i++ ) sort( ALL(tree[i]), [&data]( int a, int b ){ return data[a].height > data[b].height; } );

    const double imbalanceFactor = 0.25;

    VI nodesToClose;
    VVI nodesInSmallerParts; // nodesInSmaller parts[i] are the nodes in subtrees removed from the tree
    VI rootsInSmallerParts;
    double secondHighest = 0;

    function< void(int,int) > addToSmallerParts = [&tree, &data, &nodesInSmallerParts, &addToSmallerParts, &rootsInSmallerParts](int num, int par){
        StandardUtils::append( nodesInSmallerParts.back(), data[num].sepNodes );
//        rootsInSmallerParts.push_back( data[num].sepNodes[0] );
        for( int d : tree[num] ) if( d != par ) addToSmallerParts(d,num);
    };


    function< void(int,int) > getNodesToClose = [=,&tree,&T,&data, &nodesToClose, &getNodesToClose, &secondHighest, &nodesInSmallerParts,&rootsInSmallerParts]
            (int num, int par){
        if( tree[num].size() <= 1 ) return; // if in a leaf or there is only one stretch (e.g in a clique)
        VI sons;
        for( int d : tree[num] ) if( d != par ) sons.push_back(d);


        secondHighest = max( secondHighest, (double)dt->height - data[num].height + (double)data[num].sepNodes.size() + data[ sons[1] ].height );
//        DEBUG(num);
//        DEBUG(secondHighest);

        // add separator to nodesToClose

        StandardUtils::append( nodesToClose, data[num].sepNodes );



        if( secondHighest > dt->height * imbalanceFactor ) return; // if the height of the second heighest tree is large enough
        else{
            for( int i=1; i<sons.size(); i++ ){
                nodesInSmallerParts.push_back(VI());
                rootsInSmallerParts.push_back( data[ sons[i] ].sepNodes[0] );
                addToSmallerParts( sons[i], num );
            }
            getNodesToClose( sons[0], num );
        }
    };

    getNodesToClose(0,0);

    if( nodesToClose.size() <= 10 ){ // #TEST using imbalanced tree improver only if there are at least 10 nodes to close
//        cerr << "nodesToClose are empty" << endl;
        return t;
    }

    DEBUG(nodesToClose.size());
    DEBUG(nodesInSmallerParts.size());

    VI allNodesInSmallerParts;
    for( VI& v : nodesInSmallerParts ) StandardUtils::append(allNodesInSmallerParts, v);
//    DEBUG(allNodesInSmallerParts);


    VI nodesInLargerPart = GraphUtils::getComplimentaryNodes( *dt->V, allNodesInSmallerParts );

//    DEBUG(nodesInLargerPart);

//    DEBUG(nodesInSmallerParts);
//    DEBUG(rootsInSmallerParts);

    VVI V2 = *dt->V;
    int N = V2.size();

//    DEBUG(V2);
    CliqueUtils::fillToClique( V2, nodesToClose );
//    DEBUG(V2);

    // now filling clique

    InducedGraph gLarge = GraphInducer::induce( V2, nodesInLargerPart ); // gLarge is connected



    DepthTreeCreatorLarge creator( gLarge.V, 0 );
    DepthTree dtLarge = creator.getDepthTree();

    DEBUG(dtLarge.height);


    DepthTree dtRes(*dt->V);
    dtRes.root = gLarge.nodes[ dtLarge.root ];
    for( PII p : dtLarge.par ){ // rewriting dtLarge to dtRes
        int a = p.first;
        a = gLarge.nodes[a];

        int b = p.second;
        if( b != -1 ) b = gLarge.nodes[b];

        dtRes.par[a] = b;

//        DEBUG(VI({a,b}));
    }

    auto gLargeDepths = dtLarge.getNodeDepths();

    VI depths(N,-1); // depths of dtLarge mapped to ids from dt
    for( PII p : gLargeDepths ) depths[ gLarge.nodes[p.first] ] = p.second;

    VB subtreesRoots = StandardUtils::toVB(N,rootsInSmallerParts);

    for( VI& v : nodesInSmallerParts ){
        int attachment = -1, attHeight = -1;
        for( int p : v ){ // for each node in subtree
            for( int d : V2[p] ){ // V2 is the same for nodes in subtrees as dt->V
                if( depths[d] > attHeight ){
                    attachment = d;
                    attHeight = depths[d];
                }
            }
        }

        for( int p : v ){
            if( subtreesRoots[p] ){
                dtRes.par[p] = attachment;
//                cerr << endl << "Assigning par of root " << p << " to " << attachment << endl;
            }
            else dtRes.par[p] = dt->par[p];
        }
    }



    // appending smaller parts to current tree

    dtRes.height = dtRes.calculateHeight();
//    dtRes.writeBalanceStructure();

    DEBUG(dtRes.height);
    assert( dtRes.isCorrect() );

//    exit(1);
    return dtRes;
}

bool ImbalancedTreeImprover::isImbalanced(DepthTree &dt) {
    auto strStr = dt.getStretchStructure();

}

void ImbalancedTreeImprover::test() {
//    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
//    DEBUG(V.size());
//    DEBUG( GraphUtils::countEdges(V) );

    int N = 16;
    VVI V(N);
    V[0] = {5}; V[1] = {3}; V[2] = {4}; V[3] = {1,4,5}; V[4] = {2,3,5}; V[5] = {0,3,4};


//    DEBUG(V);
    DepthTree dt(V);
    dt.root = 0;
    dt.par = {
            {0,-1},
            {1,0},
            {2,1},
            {3,2},
            {4,3},
            {5,4}
    };

    int p = 2;
    for( int i=6; i<N; i++ ){
        dt.par[i] = p;
        p = i;

        if( i <= N/2 ) GraphUtils::addEdge( V,i,p%3 );

        for( int k=i+1; k<N; k++ ) GraphUtils::addEdge(V,i,k); // forming a clique from 6 up

    }

    p = N/2;

    for( int i=N; i<4*N; i++ ){
        dt.par[i] = p;
        V.push_back(VI());
        GraphUtils::addEdge( V, i,p );
        p = i;
    }

//    GraphUtils::addEdge( V,N, N/2 );
//    for( int i=N; i < 4*N; i++ )  for( int k=i+1; k < 4*N; k++ ) GraphUtils::addEdge( V,i,k );

//    DEBUG(V);
    dt.height = dt.calculateHeight();
    dt.writeBalanceStructure();


    ImbalancedTreeImprover improver;
    dt = improver.improve(dt);
    dt.writeBalanceStructure();


    improver.improve(dt);
    dt.writeBalanceStructure();


    exit(1);

}

