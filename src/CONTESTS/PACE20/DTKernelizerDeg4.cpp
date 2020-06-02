/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include "CONTESTS/PACE20/DTKernelizerDeg4.h"
#include <graphs/GraphReader.h>
#include <graphs/GraphUtils.h>
#include <graphs/components/ConnectedComponents.h>
#include <graphs/VertexCover/VCUtils.h>
#include <CONTESTS/PACE20/DepthTree.h>

#include <graphs/VertexCover/approximation/PartitionSVC.h>
#include <graphs/VertexCover/SolutionVC.h>

VVI DTKernelizerDeg4::kernelize() {

    cerr << endl << "Before deg4 kernelization, V has " << V->size() << " nodes and " << GraphUtils::countEdges(*V) << " edges" << endl;

    VI deg4;
    set<PII> presE;
    VB inNeigh(V->size(),false);
    for( int i=0 ; i<V->size(); i++ ){
        if( (*V)[i].size() == 4 ){

            for( int d : (*V)[i] ) inNeigh[d] = true;

            int cnt = 0, cnt17 = 0; // cnt is the number of edges between N(i), cnt17 is the number of neighbors of a neighbor of i in N(i)
            bool containsG17 = false; // true if v with N(v) contains graphlet G17 as a subgraph

            unordered_set<int> ends;

            for( int p : (*V)[i] ){
                cnt17 = 0;
                for( int d : (*V)[p] ){
                    if( p < d && d != i && inNeigh[d] ) cnt++;
                    if( d != i && inNeigh[d] ){
                        cnt17++;
                        ends.insert(p);
                        ends.insert(d);
                    }
                }
                if( cnt17 >= 2 ) containsG17 = true;
            }

            assert( ends.size() <= 4 );

            bool connected = false;
            if( cnt >= 3 && ends.size() == 4 ) connected = true;

            const int G17 = 1;
            const int CONNECTED = 2;
            int MODE = CONNECTED;

            if( MODE == G17 && containsG17 ) deg4.push_back(i);
            else if( MODE == CONNECTED && connected ) deg4.push_back(i);

            for( int d : (*V)[i] ) inNeigh[d] = false;
        }

        for(int d : (*V)[i]) if( i < d ) presE.insert( {i,d} );
    }

//    DEBUG(deg4);

    VI is;

    InducedGraph g4 = GraphInducer::induce( *V,deg4 );
    VVI comps = ConnectedComponents::getConnectedComponents( g4.V );


    for( VI& cmp : comps ){
        InducedGraph g = GraphInducer::induce( g4.V, cmp );

        VI vcg;
        if( g.V.size() > 2 ){
            PartitionSVC vcCreator(g.V);
            vcCreator.setSupressAllLogs(true); vcCreator.setMaxIterations( 20 ); vcCreator.setMaxRunTime(5'000);vcCreator.getSvcParams().alpha = 0.5;
            vcCreator.getSvcParams().iterationsPerSubgraph = 20;vcCreator.getSvcParams().setInitialSolutionForSubgraph = false;vcCreator.setTakeFirstBestSolution(false);
            vcCreator.getSvcParams().initialSolutionIterations = 1;vcCreator.run();
            vcg = ((SolutionVC*) vcCreator.getBestSolution())->getVC();

        }else if( g.V.size() == 2 ){
            vcg = {0};
        }

        VI isg = GraphUtils::getComplimentaryNodes( g.V, vcg );

        for( int d : isg ) is.push_back( g4.nodes[ g.nodes[d] ] );
    }

    if( is.empty() ) return *V;

    assert( VCUtils::isIndependentSet( *V,is ) );

//    DEBUG(is);

    set<PII> addedEdges;

    for( int p : is ){
        VI v = (*V)[p];
        sort(ALL(v));

        for(int i=0; i<v.size(); i++){
            int a = v[i];
            for( int k=i+1; k<v.size(); k++ ){
                int b = v[k];

//                assert( a<b );

                if( presE.count( {a,b} ) == 0 ) addedEdges.insert( {a,b} );
            }
        }

    }

    deg4AddedEdges = VPII( ALL(addedEdges) );
    deg4RemovedNodes = is;

    VI remNodes = GraphUtils::getComplimentaryNodes( *V, is );
    kernelizedV = GraphInducer::induce( *V, remNodes );

    for( PII e : addedEdges ){
        int a = e.first;
        int b = e.second;
        GraphUtils::addEdge( kernelizedV.V, kernelizedV.perm[a], kernelizedV.perm[b] );
    }

//    DEBUG(kernelizedV.V.size());
//    exit(1);

    cerr << "After deg4 kernelization, kernelizedV has " << kernelizedV.V.size() << " nodes and " << GraphUtils::countEdges(kernelizedV.V) << " edges" << endl;


//    DEBUG(deg4AddedEdges);
    DEBUG(deg4RemovedNodes.size());

    return kernelizedV.V;



}

DepthTree DTKernelizerDeg4::dekernelize(DepthTree dt) {
//    if( deg3AddedEdges.empty() ) return dt;
    if( deg4RemovedNodes.empty() ) return dt;

    if( dt.root != -1 ) dt.root = kernelizedV.nodes[ dt.root ];

    int n = kernelizedV.V.size();

    assert( !( dt.root == -1 && n > 0) );

    auto dtPar = dt.par;
    dt.par.clear();

    for( int i=0; i<n; i++ ){
        int a = kernelizedV.nodes[i];
        int b = dtPar[i];
        if( b != -1 ) b = kernelizedV.nodes[b];
        dt.par[a] = b;
    }

//    DEBUG(dt.par);


    unordered_map<int, VI> stdStruct;
    for( PII p : dt.par ){
        if( p.second != -1 ){
            stdStruct[p.first].push_back(p.second);
            stdStruct[p.second].push_back(p.first);
        }
    }

//    DEBUG(stdStruct);

    unordered_map<int,int> nodeDepth;
    function< void(int,int,int) > calcDepthDfs = [ &dt, &stdStruct, &nodeDepth, &calcDepthDfs ]( int num, int par, int depth ){
        nodeDepth[num] = depth;
        for( int p : stdStruct[num] ) if( p != par ) calcDepthDfs( p,num,depth+1 );
    };

    calcDepthDfs( dt.root, dt.root, 0 );

//    DEBUG(nodeDepth);

    for( int v : deg4RemovedNodes ){
        int lowestNode = -1;
        int lowestHeight = -1;

        for( int d : (*V)[v] ){
            if( nodeDepth[d] > lowestHeight ){
                lowestNode = d;
                lowestHeight = nodeDepth[d];
            }
        }

//        cerr << "setting par of " << v << " to " << lowestNode << endl;
        dt.par[v] = lowestNode;
    }


//    DEBUG(dt.par);

    dt.height = dt.calculateHeight();
    dt.V = kernelizedV.par;
//    DEBUG(dt);
    return dt;

}



void DTKernelizerDeg4::test(){
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );


    DTKernelizerDeg4 ker(V);
    VVI newV = ker.kernelize();

    cerr << "kernelized!" << endl;

    DepthTree dt(newV);
    dt.height = newV.size();
    dt.root = 0;
    for( int i=0; i<newV.size(); i++ ) dt.par[i] = i-1;


    dt = ker.dekernelize(dt);

    cerr << "dekernelized" << endl;

    assert( dt.isCorrect() );

    exit(1);

}

