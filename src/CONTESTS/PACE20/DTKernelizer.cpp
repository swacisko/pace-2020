//
// Created by sylwester on 4/2/20.
//

#include <CONTESTS/PACE20/DTKernelizer.h>
#include <datastructures/FAU.h>
#include <graphs/trees/Tree.h>
#include <CONTESTS/PACE20/separatorminimizers/SnapToNonpathNodesMinimizer.h>
#include <CONTESTS/PACE20/depthtreecreators/DepthTreeCreatorLarge.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <graphs/VertexCover/VCUtils.h>
#include "graphs/GraphReader.h"
#include "graphs/components/ConnectedComponents.h"
#include "graphs/GraphUtils.h"
#include "utils/StandardUtils.h"
#include "graphs/trees/Centroids.h"

DTKernelizer::DTKernelizer(VVI &V) {
    this->V = &V;
}

void DTKernelizer::createDanglingTrees() {
    VI kolejka;
    int N = V->size();
    VI deg(N,0);
    for( int i=0; i<N; i++ ){
        deg[i] = (*V)[i].size();
        if(deg[i] == 1) kolejka.push_back(i);
    }

    FAU fau(N);

    for( int i=0; i<kolejka.size(); i++ ){
        int p = kolejka[i];
//        DEBUG(p);

        if( deg[p] == 0 ){
            for( int d : (*V)[p] ){
                fau.Union(p,d);
            }
            continue;
        }


        for( int d : (*V)[p] ){
            deg[d]--;

//            cerr << "\td = " << d << "   deg[d] = " << deg[d] << endl;

            if( deg[d] == 1 ){
                kolejka.push_back(d);
            }
            else if( deg[d] == 0 ){
//                cerr << "union" << endl;
                fau.Union(p,d);
            }
        }
    }


    VVI nodes(N);
    for( int p : kolejka ){
        nodes[ fau.Find(p) ].push_back(p);
    }

    for( VI& v : nodes ){
        if(!v.empty()){
            removedTreeNodes.push_back(v);

            VI attPoints = getAttachmentPoints( v );
            for( int d : attPoints ) if( d != -1 ) removedTreeNodes.back().push_back( d );

//            DEBUG(v);
        }
    }
//    DEBUG(removedTreeNodes);


    // creating paths and merging them with existing trees if possible

    VPII edgesToRemove;

    for( VI& tree : removedTreeNodes ) {
        InducedGraph g = GraphInducer::induce(*V, tree);
        VPII edges = GraphUtils::getGraphEdges(g.V);
        for( PII& p : edges ) p = { g.nodes[p.first], g.nodes[p.second] };
        edgesToRemove.insert( edgesToRemove.end(), ALL(edges) );
    }

//    DEBUG(edgesToRemove);

    VVI V2 = *V;
    GraphUtils::removeEdges( V2, edgesToRemove );
//    cerr << "graph after removing edges" << endl;
//    DEBUG(V2);

    VVI paths = SnapToNonpathNodesMinimizer::findPaths(V2);
//    DEBUG(paths);

    for( VI& path : paths ){
        int a = path[0];
        int b = path.back();
        if( a > b ) swap(a,b);
        if( a != b && GraphUtils::containsEdge( *V,a,b ) == false ) edgesToAddToKernelizedGraph.insert({a,b});

//        DEBUG( PII(a,b) );

        for( int i=1; i<path.size()-1; i++ ){
            int p = path[i];
            kolejka.push_back(p);
//            cerr << "adding node " << p << " to kolejka" << endl;

            for( int d : (*V)[p] ){
                if( d != a && d != b ){
                    fau.Union( p,d );
//                    cerr << "unifying " << p << " and " << d << endl;
                }
            }
        }
    }

//    DEBUG( edgesToAddToKernelizedGraph );

    // end of creating paths and merging them with existing trees if possible

    nodes = VVI(N);
    removedTreeNodes.clear();
    for( int p : kolejka ){
        nodes[ fau.Find(p) ].push_back(p);
    }

    for( VI& v : nodes ){
        if(!v.empty()){
            removedTreeNodes.push_back(v);
//            DEBUG(v);
        }
    }

//    DEBUG(removedTreeNodes);
//    DEBUG( max_element(ALL(removedTreeNodes), [](VI& v1, VI& v2){return v1.size() < v2.size();} )->size() );

//    exit(1);



    centroids.clear();
    for( VI& tree : removedTreeNodes ){

        InducedGraph g = GraphInducer::induce(*V,tree);

        if( !Tree::isTree(g.V) ) ERROR( "", "g.V is not a tree in dtKernelizer" );

        VI decomp = Centroids::getCentroidDecomposition( g.V );
        unordered_map<int,int> par;
        for( int i=0; i<decomp.size(); i++ ){
            int a = g.nodes[i];
            int b = decomp[i];
            if( b != -1 ) b = g.nodes[b];
            par[a] = b;
        }
        centroids.push_back(par);
    }

//    DEBUG(centroids);

}


VI DTKernelizer::getAttachmentPoints(VI &treeNodes) {
    unordered_set<int> was(ALL(treeNodes));
    set<int> res;
    for( int p : treeNodes ){
        for( int d : (*V)[p] ){
//            if( was.count(d) == 0 ) return {d};
            if( was.count(d) == 0 ) res.insert(d);
        }
    }

    /*    cerr << "there was no attachment point found in DTKernelizer, returning -1" << endl;*/
    if( res.empty() ) return {-1};
    else return VI(ALL(res));
}


VVI & DTKernelizer::getKernelizedGraph(int KERNEL_MODE) {
    VI nodesToRemove;
    if( KERNEL_MODE & DANGLING_TREES ){
        createDanglingTrees();
        for( VI& tree : removedTreeNodes ) nodesToRemove.insert( nodesToRemove.end(), ALL(tree) );
//        DEBUG(nodesToRemove.size());
//        exit(1);
    }

    VPII edgesToAdd;
    if( KERNEL_MODE & EDGE_COMPRESSION ){
        cerr << "NOT SUPPPORTED YET, SWITCHING TO DANGLING_TREES_MODE" << endl;
    }

    VI nodes = GraphUtils::getComplimentaryNodes(*V, nodesToRemove);
    kernelizedV = GraphInducer::induce(*V, nodes);

//    DEBUG(kernelizedV.V);

    for( PII e : edgesToAddToKernelizedGraph ){
        PII e2 = { kernelizedV.perm[e.first], kernelizedV.perm[e.second] };
//        DEBUG(e);
//        DEBUG(e2);
        GraphUtils::addEdge( kernelizedV.V, e2.first, e2.second );
    }

//    DEBUG(kernelizedV.V);
//    DEBUG(kernelizedV.nodes);

    return kernelizedV.V;
}



DepthTree DTKernelizer::dekernelize(DepthTree dt) {
//    DEBUG(dt.par);

   /* if( dt.root == -1 ){
        DEBUG(dt.root);
        DEBUG(dt.par);
        exit(1);
    }*/

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

    for( int i=0; i<centroids.size(); i++ ){
        VI attachmentPoints = getAttachmentPoints(removedTreeNodes[i]);

        assert( attachmentPoints.size() <= 2 );

        int attachment = attachmentPoints[0];
        if( attachmentPoints.size() > 1 && nodeDepth[ attachmentPoints[1] ] > nodeDepth[ attachmentPoints[0] ]  ) attachment = attachmentPoints[1];

//        DEBUG(removedTreeNodes[i]);
//        DEBUG(attachmentPoints);
//        DEBUG(attachment);
//        ENDL(1);

        for( PII p : centroids[i] ){
            if( p.second == -1 ){
                dt.par[p.first] = attachment;
                if( dt.root == -1 ) dt.root = p.first;
            }
            else dt.par[p.first] = p.second;
        }
    }


//    DEBUG(dt.par);

    dt.height = dt.calculateHeight();
    dt.V = kernelizedV.par;
//    DEBUG(dt);
    return dt;


}




void DTKernelizer::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

//    GraphUtils::writeBasicGraphStatistics(V);
//    exit(1);

    DTKernelizer ker(V);



    VVI W = ker.getKernelizedGraphSubgraphs();

    DepthTreeCreatorLarge dtCr(W,0);
    DepthTree dt = dtCr.getDepthTree();

    /*DepthTree dt(W);
    dt.height = W.size();
    if( !W.empty() ){
        dt.par[0] = -1;
        dt.root = 0;
    }else dt.root = -1;
    for(int i=1; i<W.size(); i++) dt.par[i] = i-1;*/

    auto resDt = ker.dekernelizeSubgraphs(dt);

    if( !resDt.isCorrect() ){
        cerr << "not correct depth tree!" << endl;
        exit(1);
    }


    DEBUG(resDt.par);
    DEBUG(resDt.height);

    exit(1);
}











































VVI DTKernelizer::createDanglingSubgraphs(int recDepth) {
    if( V->size() == 1 || Tree::isTree(*V) ){
        VI v(V->size());
        iota(ALL(v),0);
        return {v};
    }

    VI kolejka;
    int N = V->size();
    VI deg(N,0);
    for( int i=0; i<N; i++ ){
        deg[i] = (*V)[i].size();
        if(deg[i] == 1) kolejka.push_back(i);
    }

    FAU fau(N);

    for( int i=0; i<kolejka.size(); i++ ){
        int p = kolejka[i];
//        DEBUG(p);

        if( deg[p] == 0 ){
            for( int d : (*V)[p] ){
                fau.Union(p,d);
            }
            continue;
        }


        for( int d : (*V)[p] ){
            deg[d]--;

//            cerr << "\td = " << d << "   deg[d] = " << deg[d] << endl;

            if( deg[d] == 1 ){
                kolejka.push_back(d);
            }
            else if( deg[d] == 0 ){
//                cerr << "union" << endl;
                fau.Union(p,d);
            }
        }
    }


    VVI nodes(N);
    for( int p : kolejka ){
        nodes[ fau.Find(p) ].push_back(p);
    }

    for( VI& v : nodes ){
        if(!v.empty()){
            removedSubgraphsNodes.push_back(v);

            VI attPoints = getAttachmentPoints( v );
            for( int d : attPoints ) if( d != -1 ) removedSubgraphsNodes.back().push_back( d );

//            DEBUG(v);
        }
    }
//    DEBUG(removedTreeNodes);


    // creating paths and merging them with existing trees if possible

    VPII edgesToRemove;

    for( VI& tree : removedSubgraphsNodes ) {
        InducedGraph g = GraphInducer::induce(*V, tree);
        VPII edges = GraphUtils::getGraphEdges(g.V);
        for( PII& p : edges ) p = { g.nodes[p.first], g.nodes[p.second] };
        edgesToRemove.insert( edgesToRemove.end(), ALL(edges) );
    }

//    DEBUG(edgesToRemove);



    VVI V2 = *V;
    GraphUtils::removeEdges( V2, edgesToRemove );
//    cerr << "graph after removing edges" << endl;
//    DEBUG(V2);


    VVI paths = SnapToNonpathNodesMinimizer::findPaths(V2);
//    if( recDepth == 0 ) DEBUG(paths);


    for (VI &path : paths) {

        int a = path[0];
        int b = path.back();
        if (a > b) swap(a, b);
        if( a != b && GraphUtils::containsEdge( *V,a,b ) == false ) edgesToAddToKernelizedGraph.insert({a,b});

        for (int i = 1; i < path.size() - 1; i++) {
            int p = path[i];
            kolejka.push_back(p);
//            cerr << "adding node " << p << " to kolejka" << endl;

            for (int d : (*V)[p]) {
                if (d != a && d != b) {
                    fau.Union(p, d);
//                    cerr << "unifying " << p << " and " << d << endl;
                }
            }
        }
    }

    GraphUtils::removeNodesFromGraph( V2,kolejka );
    for( PII p : edgesToAddToKernelizedGraph ) GraphUtils::addEdge(V2,p.first,p.second);

//    DEBUG(V2);


    // end of creating paths and merging them with existing trees if possible

    nodes = VVI(N);
    removedSubgraphsNodes.clear();
    for( int p : kolejka ){
        nodes[ fau.Find(p) ].push_back(p);
    }

//    DEBUG(nodes);

    VB isRemoved = StandardUtils::toVB( V->size(), kolejka );


//    DEBUG(removedSubgraphsNodes);

    VI allNodes;
    for( int p : kolejka ) for( int d : nodes[p] ) allNodes.push_back(d);
//    if( recDepth == 0 ) DEBUG(allNodes);
//    exit(1);
//


    if( !allNodes.empty() && allNodes.size() != V->size() ) {


        VVI comps = ConnectedComponents::getConnectedComponents(V2, allNodes);


        sort( ALL(comps), []( auto& v1, auto& v2 ){ return v1.size() > v2.size(); } );
        assert( comps[0].size() != V->size() );

        for (VI &comp : comps) {
//            InducedGraph g = GraphInducer::induce( *V, comp );
            InducedGraph g = GraphInducer::induce( V2, comp );
            DTKernelizer ker( g.V );

//            if( recDepth == 0 ) cerr << "Creating tree for subgraph of size: " << comp.size() << endl;

            VVI recNodes = ker.createDanglingSubgraphs(recDepth+1);
//            if( recDepth == 0 ) cerr << "recNodes:" << endl;
            for( VI& v : recNodes ){
                for( int & d : v ) d = g.nodes[d];
//                if( recDepth == 0 ) DEBUG(v);

                for( int p : v ){
                    kolejka.push_back(p);
                    isRemoved[p] = true;
                    for( int d : (*V)[p] ){
                        if( isRemoved[d] ) fau.Union( p,d );
                    }
                }
            }
//            if( recDepth == 0 ) ENDL(1);

        }
    }

    sort( ALL(kolejka) );
    kolejka.resize( unique(ALL(kolejka)) - kolejka.begin() );

    nodes = VVI(N);
    removedSubgraphsNodes.clear();
    for( int p : kolejka ){
        nodes[ fau.Find(p) ].push_back(p);
    }

    for( VI& v : nodes ){
        if(!v.empty()){
            removedSubgraphsNodes.push_back(v);
//            DEBUG(v);
        }
    }




//    if( recDepth == 0 ) cerr << "Attachment points:" << endl;

    if( recDepth == 0 ) {

        edgesToAddToKernelizedGraph.clear();
        for (VI &v : removedSubgraphsNodes) {
            VI attachments = getAttachmentPoints(v);

            assert( attachments.size() <= 2 );
//            if (recDepth == 0) DEBUG(v);
//            if (recDepth == 0) DEBUG(attachments);

            int A = attachments.size();
            for (int i = 0; i < A; i++) {
                for (int k = i + 1; k < A; k++) {
                    int a = attachments[i];
                    int b = attachments[k];
                    if (a > b) swap(a, b);
                    if( a != b && GraphUtils::containsEdge( *V,a,b ) == false ) edgesToAddToKernelizedGraph.insert({a, b});
                }
            }
        }

//        DEBUG( edgesToAddToKernelizedGraph );

        for( VI& sub : removedSubgraphsNodes ){

//            assert(sub.size() <= V->size());

            if( sub.size() == V->size() ) break;

//            assert( sub.size() < V->size() );
//            cerr << "in graph induced by " << sub << endl;
            InducedGraph g = GraphInducer::induce( *V,sub );

            DepthTreeCreatorLarge dtCr( g.V,1 );
            dtCr.setSeparatorCreatorsMode( DepthTreeCreatorLarge::ART_POINTS_CREATOR );
            dtCr.MINIMIZE_SEPARATORS = false;
            dtCr.USE_KERNELIZATION = false;

            assert( g.V.size() < V->size() );
//            cerr << "here" << endl;

            DepthTree dt = dtCr.getDepthTree();


            dt.root = g.nodes[dt.root];
            auto par = dt.par;
            par.clear();
            for( PII p : dt.par ){
                int a = p.first;
                a = g.nodes[a];
                int b = p.second;
                if(b != -1) b = g.nodes[b];
                par[a] = b;
            }
            dt.par = par;
            subgraphTrees.push_back(dt);
//            DEBUG(dt);
        }


    }



    if( removedSubgraphsNodes.size() == 1 && removedSubgraphsNodes[0].size() == V->size() ){
        removedSubgraphsNodes.clear();
        subgraphTrees.clear();
    }

    int totalSize = accumulate( ALL(removedSubgraphsNodes),0, []( int s, VI& v ){ return s + v.size(); } );
//    assert( totalSize < V->size()  );


//    assert( GraphUtils::countNodesWithDegree(*V,1,2) == 0 || removedSubgraphsNodes.size() > 0 || ( removedSubgraphsNodes[0].size() == V->size() ) );

    return removedSubgraphsNodes;

}

DepthTree DTKernelizer::dekernelizeSubgraphs(DepthTree dt) {
//    cerr << "dekernelizing" << endl;
//    DEBUG(dt.par);

    /* if( dt.root == -1 ){
         DEBUG(dt.root);
         DEBUG(dt.par);
         exit(1);
     }*/

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

    for( int i=0; i<removedSubgraphsNodes.size(); i++ ){
        VI attachmentPoints = getAttachmentPoints( removedSubgraphsNodes[i] );

        int attachment = attachmentPoints[0];
        int maxDepth = nodeDepth[attachment];
        for( int atp : attachmentPoints ){
            if( nodeDepth[atp] > maxDepth ){
                attachment = atp;
                maxDepth = nodeDepth[atp];
            }
        }

//        DEBUG(removedTreeNodes[i]);
//        DEBUG(attachmentPoints);
//        DEBUG(attachment);
//        ENDL(1);

        DepthTree& subt = subgraphTrees[i];
        dt.par[subt.root] = attachment;
        for( PII p : subt.par ){
            if( p.second != -1 ){
                dt.par[ p.first ] = p.second;
            }
        }

        if( dt.root == -1 ){
            assert( subgraphTrees.size() == 1 );
            dt.root = subt.root;
        }
    }


//    DEBUG(dt.par);

    dt.height = dt.calculateHeight();


//    DEBUG(dt);
    return dt;



}

VVI &DTKernelizer::getKernelizedGraphSubgraphs(int KERNEL_MODE) {
    VI nodesToRemove;
    if( KERNEL_MODE & DANGLING_TREES ){
        createDanglingSubgraphs(0);
//        exit(1);
        for( VI& tree : removedSubgraphsNodes ) nodesToRemove.insert( nodesToRemove.end(), ALL(tree) );

    }

//    assert( !nodesToRemove.empty() );

    VI nodes = GraphUtils::getComplimentaryNodes(*V, nodesToRemove);

//    assert( !nodes.empty() );

    kernelizedV = GraphInducer::induce(*V, nodes);

//    DEBUG(kernelizedV.V);

    assert( nodes.size() > 0 || edgesToAddToKernelizedGraph.empty() );

    for( PII e : edgesToAddToKernelizedGraph ){
        PII e2 = { kernelizedV.perm[e.first], kernelizedV.perm[e.second] };
//        DEBUG(e);
//        DEBUG(e2);
        GraphUtils::addEdge( kernelizedV.V, e2.first, e2.second );
    }

//    DEBUG(kernelizedV.V);
//    DEBUG(kernelizedV.nodes);

    return kernelizedV.V;

}

