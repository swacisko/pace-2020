//
// Created by sylwester on 4/17/20.
//



#include <CONTESTS/PACE20/DTKernelizerDeg3.h>
#include <graphs/GraphReader.h>
#include <graphs/GraphUtils.h>
#include <graphs/components/ConnectedComponents.h>
#include <graphs/VertexCover/VCUtils.h>
#include <CONTESTS/PACE20/DepthTree.h>

#include <graphs/VertexCover/approximation/PartitionSVC.h>
#include <graphs/VertexCover/SolutionVC.h>

VVI DTKernelizerDeg3::kernelize() {
    cerr << endl << "Before deg3 kernelization, V has " << V->size() << " nodes and " << GraphUtils::countEdges(*V) << " edges" << endl;

    VI deg3;
    set<PII> presE;
    for( int i=0 ; i<V->size(); i++ ){
        if( (*V)[i].size() == 3 ) deg3.push_back(i);
        for(int d : (*V)[i]) if( i < d ) presE.insert( {i,d} );
    }

    VI is;

    InducedGraph g3 = GraphInducer::induce( *V,deg3 );
    VVI comps = ConnectedComponents::getConnectedComponents( g3.V );

//    DEBUG(comps.size());
//    sort( ALL(comps), [](auto &v1, auto &v2){ return v1.size() > v2.size(); } );
//    for(int i=0; i<min( (int)comps.size(),5 ); i++) DEBUG(comps[i].size());

//    cerr << "comps:" << endl;
//    for( auto v : comps ){
//        for( int d : v ) cerr << g3.nodes[d] << " ";
//        cerr << endl;
//    }

    for( VI& cmp : comps ){
        InducedGraph g = GraphInducer::induce( g3.V, cmp );

//        {
//            auto vcg = VCUtils::getVCGreedyMaxItarativeDegree(g.V);
//            if( cmp.size() > 3'000 ){
//                cerr << "is.size() in g.V by greedy: " << g.V.size() - vcg.size() << endl;
//            }
//        }

        VI vcg;
        if( g.V.size() > 2 ){
            PartitionSVC vcCreator(g.V);
            vcCreator.setSupressAllLogs(true); vcCreator.setMaxIterations( 20 ); vcCreator.setMaxRunTime(5'000);vcCreator.getSvcParams().alpha = 0.5;
            vcCreator.getSvcParams().iterationsPerSubgraph = 20;vcCreator.getSvcParams().setInitialSolutionForSubgraph = false;vcCreator.setTakeFirstBestSolution(false);
            vcCreator.getSvcParams().initialSolutionIterations = 1;vcCreator.run();
            vcg = ((SolutionVC*) vcCreator.getBestSolution())->getVC();

//            if( cmp.size() > 3'000 ){
//                cerr << "is.size() in g.V by partSVC: " << g.V.size() - vcg.size() << endl << endl;
//            }
        }else if( g.V.size() == 2 ){
            vcg = {0};
        }

//        DEBUG(vcg);
        VI isg = GraphUtils::getComplimentaryNodes( g.V, vcg );
//        DEBUG(isg);

        for( int d : isg ) is.push_back( g3.nodes[ g.nodes[d] ] );
    }

    if( is.empty() ) return *V;

    assert( VCUtils::isIndependentSet( *V,is ) );

//    DEBUG(is.size());

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

    deg3AddedEdges = VPII( ALL(addedEdges) );
    deg3RemovedNodes = is;

    VI remNodes = GraphUtils::getComplimentaryNodes( *V, is );
    kernelizedV = GraphInducer::induce( *V, remNodes );

    for( PII e : addedEdges ){
        int a = e.first;
        int b = e.second;
        GraphUtils::addEdge( kernelizedV.V, kernelizedV.perm[a], kernelizedV.perm[b] );
    }

//    DEBUG(kernelizedV.V.size());
//    exit(1);

    cerr << "After deg3 kernelization, kernelizedV has " << kernelizedV.V.size() << " nodes and " << GraphUtils::countEdges(kernelizedV.V) << " edges" << endl;

    return kernelizedV.V;

}


DepthTree DTKernelizerDeg3::dekernelize(DepthTree dt) {
//    if( deg3AddedEdges.empty() ) return dt;
    if( deg3RemovedNodes.empty() ) return dt;

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

    for( int v : deg3RemovedNodes ){
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



void DTKernelizerDeg3::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );


    DTKernelizerDeg3 ker(V);
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
