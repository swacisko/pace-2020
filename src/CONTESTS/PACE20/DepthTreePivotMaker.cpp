//
// Created by sylwester on 3/27/20.
//

#include <CONTESTS/PACE20/DepthTreePivotMaker.h>

#include "CONTESTS/PACE20/DepthTreePivotMaker.h"
#include "CONTESTS/PACE20/ComponentTreeMerger.h"
#include <graphs/GraphUtils.h>
#include "graphs/components/ConnectedComponents.h"
#include <graphs/GraphReader.h>
#include <graphs/trees/Tree.h>
#include <utils/StandardUtils.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include "graphs/cliques/CliqueUtils.h"
#include "graphs/GraphInducer.h"
#include <graphs/matching/MaxMatchBipartite.h>
#include <CONTESTS/PACE20/Pace20.h>

namespace DepthTreePivotMaker{


    DepthTree makePivot( DepthTree & dt ){
        if( dt.height <= 3 ) return dt;

//        cerr << "entering pivot" << endl;
//        DEBUG( (*dt.V) );
//        DEBUG(dt.root);
//        DEBUG(dt.par);
//        DEBUG(dt.height);

        auto stretchT = dt.getStretchStructure();
        VVI T = stretchT.first;
        auto data = stretchT.second;

        if( T[0].size() == 0 ) return dt; // single node in stretch representation

        /*for( int i=0; i<T.size(); i++ ){
            DEBUG(i);
            DEBUG(T[i]);
            sort( ALL(T[i]), [&data](int a, int b){
                return data[a].height > data[b].height;
            } );
            DEBUG(T[i]);
            ENDL(1);
        }*/

//        DEBUG(T[0]);
        sort( ALL(T[0]), [&data](int a, int b){
            return data[a].height > data[b].height;
        } );
//        DEBUG(T[0]);

//        DEBUG(stretchT);


        // HERE SHOULD BE EXACT CHECKING WITH ALL EDGES IN ORIGINAL GRAPH - WHERE ALL SUBTREES END AND WHAT IS THE HEIGHT THEN


        function< void(int,int,VI&) > getSubtreeNodes = [&getSubtreeNodes, &T, &data]( int num, int par, VI& nodes ){
            nodes.insert( nodes.end(), ALL( data[num].sepNodes ) );
            for(int p : T[num]){
                if(p != par) getSubtreeNodes(p,num,nodes);
            }
        };

        function< void(int,int, DepthTree&) > markPar = [&markPar,&getSubtreeNodes, &T, &data]( int num, int par, DepthTree& t ){
            int a = data[par].sepNodes.back();
            int b = data[num].sepNodes[0];
            t.par[b] = a;

            for( int i=1; i<data[num].sepNodes.size(); i++ ){
                int p = data[num].sepNodes[i-1];
                int q = data[num].sepNodes[i];
                t.par[q] = p;
            }

            for(int p : T[num]){
                if(p != par) markPar(p,num,t);
            }
        };

        int v0 = T[0][0];

        int n = T[v0].size();

        VI newStretchNodes = data[v0].sepNodes;
        newStretchNodes.insert( newStretchNodes.end(), ALL( data[0].sepNodes ) );

//        DEBUG(newStretchNodes);

        vector<DepthTree> subtrees;
        VVI comps;

        for( int i=1; i<n; i++ ){
            DepthTree t( *dt.V );
            VI tNodes;
            int p = T[v0][i];
            getSubtreeNodes( p,v0,tNodes );

//            DEBUG(p);
//            DEBUG(tNodes);

            t.root = tNodes[0];
            t.height = data[p].height;

            markPar( p,v0,t );
            t.par[t.root] = -1;

            subtrees.push_back(t);
            comps.push_back(tNodes);
        }

//        for(int i=0; i<subtrees.size(); i++) DEBUG(subtrees[i]);

        for( int i=1; i<T[0].size(); i++ ){
            DepthTree t( *dt.V );
            VI tNodes;
            int p = T[0][i];
            getSubtreeNodes( p,0,tNodes );

//            DEBUG(p);
//            DEBUG(tNodes);

            t.root = tNodes[0];
            t.height = data[p].height;

            markPar( p,0,t );
            t.par[t.root] = -1;

            subtrees.push_back(t);
            comps.push_back(tNodes);
        }

//        for(int i=0; i<subtrees.size(); i++) DEBUG(subtrees[i]);
//        DEBUG(comps);

        Separator sep( *dt.V, newStretchNodes );
//        DEBUG((*sep.V));
        sep.createSeparatorStats();
//        DEBUG(sep);
        ComponentTreeMerger merger( *dt.V, sep, comps, subtrees );
        auto resDt = merger.mergeComponents();

//        DEBUG(resDt);

//        ENDL(1);
//        exit(1);

//        cerr << "Leaving pivotMaker" << endl << endl;

        return resDt;

    }


    DepthTree makePivotMultipleStretch(DepthTree &dt, double balance) {
        const bool debug = false;

        if( debug ) cerr << "in multiple stretch, dt = " << dt << endl;

        auto strStr = dt.getStretchStructure();
        if( debug ) cerr << "strStr created" << endl;

        VVI tree = strStr.first;
        int T = tree.size();
        auto data = strStr.second;

        for( int i=0; i<T; i++ ) sort( ALL(tree[i]), [&data]( int a, int b ){ return data[a].height > data[b].height; } );

        const double imbalanceFactor = balance;

        VI separatorNodes;
        VVI subtreeNodes; // nodesInSmaller parts[i] are the nodes in subtrees removed from the tree
        VI subtreeRoots;
        double secondHighest = 0;

        function< void(int,int) > addToSubtrees = [&tree, &data, &subtreeNodes, &addToSubtrees, &subtreeRoots](int num, int par){
            StandardUtils::append( subtreeNodes.back(), data[num].sepNodes );
//        subtreeRoots.push_back( data[num].sepNodes[0] );
            for( int d : tree[num] ) if( d != par ) addToSubtrees(d,num);
        };


        function< void(int,int) > createSubtreesAndSeparator = [=, &debug,&tree,&T,&data, &separatorNodes, &createSubtreesAndSeparator, &secondHighest, &subtreeNodes,&subtreeRoots]
                (int num, int par){
//            if( tree[num].size() <= 1 ) return; // if in a leaf or there is only one stretch (e.g in a clique)
            VI sons;
            for( int d : tree[num] ) if( d != par ) sons.push_back(d);
            if(debug)DEBUG(sons);


            if( !sons.empty() ) secondHighest = max( secondHighest, (double)dt.height - data[num].height + (double)data[num].sepNodes.size() + data[ sons[1] ].height );
            if(debug)DEBUG(num);
            if(debug)DEBUG(secondHighest);


            // add separator to separatorNodes

            StandardUtils::append( separatorNodes, data[num].sepNodes );


            if( sons.empty() ) return;

            if( secondHighest > dt.height * imbalanceFactor ){ // if the height of the second heighest tree is large enough
                for( int i=0; i<sons.size(); i++ ){
                    subtreeNodes.push_back(VI());
                    subtreeRoots.push_back( data[ sons[i] ].sepNodes[0] );
                    addToSubtrees( sons[i], num );
                }
                return;
            }
            else{
                for( int i=1; i<sons.size(); i++ ){
                    subtreeNodes.push_back(VI());
                    subtreeRoots.push_back( data[ sons[i] ].sepNodes[0] );
                    addToSubtrees( sons[i], num );
                }
                createSubtreesAndSeparator( sons[0], num );
            }
        };

        createSubtreesAndSeparator(0,0);

        if( separatorNodes.empty() || subtreeNodes.empty() ) return dt;


        if(debug){
            DEBUG(separatorNodes);
            DEBUG(subtreeNodes);
            DEBUG(subtreeRoots);
        }


        VI heightOfNodeStretch(dt.V->size(),-1);
        for( int i=0; i<T; i++ ) for( int d : data[i].sepNodes ) heightOfNodeStretch[d] = data[i].height;

        VVI comps = subtreeNodes;
        Separator sep(*dt.V, separatorNodes);
        sep.createSeparatorStats();
        vector<DepthTree> subtrees;
        for( int i=0; i<subtreeNodes.size(); i++ ){
            VI v = subtreeNodes[i];
            int root = subtreeRoots[i];

            DepthTree tmp(*dt.V);
            tmp.root = root;
            for( int p : v ) tmp.par[p] = dt.par[p];
            tmp.par[root] = -1;
            tmp.height = heightOfNodeStretch[root];

            subtrees.push_back( tmp );

            if(debug)cerr << "Adding subtree " << tmp << endl;
        }


        ComponentTreeMerger merger(*dt.V,sep, comps, subtrees);

        DepthTree dtRes = merger.mergeComponents();

        if(debug) dtRes.writeBalanceStructure(2);

        if(debug)DEBUG(dtRes.height);
        assert( dtRes.isCorrect() );

        return dtRes;
    }


    DepthTree makeHallSetPivots(DepthTree &dt, double balance, bool useDeepestPath) {
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

        for( int i=0; i<T; i++ ) sort( ALL(tree[i]), [&data]( int a, int b ){ return data[a].height > data[b].height; } );

        const double imbalanceFactor = balance;
        VI separatorNodes;
        VVI subtreeNodes; // nodesInSmaller parts[i] are the nodes in subtrees removed from the tree
        VI subtreeRoots;
        double secondHighest = 0;

        function< void(int,int) > addToSubtrees = [&tree, &data, &subtreeNodes, &addToSubtrees, &subtreeRoots](int num, int par){
            StandardUtils::append( subtreeNodes.back(), data[num].sepNodes );
            for( int d : tree[num] ) if( d != par ) addToSubtrees(d,num);
        };

        function< void(int,int) > createSubtreesAndSeparator = [=,&tree,&T,&data, &separatorNodes, &createSubtreesAndSeparator, &secondHighest, &subtreeNodes,&subtreeRoots]
                (int num, int par){
            VI sons;
            for( int d : tree[num] ) if( d != par ) sons.push_back(d);
            if(debug)DEBUG(sons);
            if( !sons.empty() ) secondHighest = max( secondHighest, (double)dt.height - data[num].height + (double)data[num].sepNodes.size() + data[ sons[1] ].height );
            if(debug)DEBUG(num);
            if(debug)DEBUG(secondHighest);

            // add separator to separatorNodes
            StandardUtils::append( separatorNodes, data[num].sepNodes );

            if( sons.empty() ) return;
            if( secondHighest > dt.height * imbalanceFactor ){ // if the height of the second heighest tree is large enough
//                for( int i=0; i<sons.size(); i++ ){
                for( int i=(int)sons.size()-1; i>=0; i-- ){ // adding in reverse order - deepest subtree is subtrees.back()
                    subtreeNodes.push_back(VI());
                    subtreeRoots.push_back( data[ sons[i] ].sepNodes[0] );
                    addToSubtrees( sons[i], num );
                }
                return;
            }
            else{
                for( int i=1; i<sons.size(); i++ ){
                    subtreeNodes.push_back(VI());
                    subtreeRoots.push_back( data[ sons[i] ].sepNodes[0] );
                    addToSubtrees( sons[i], num );
                }
                createSubtreesAndSeparator( sons[0], num );
            }
        };

        createSubtreesAndSeparator(0,0);

        if( separatorNodes.empty() || subtreeNodes.empty() ) return dt;

        if(debug){
            DEBUG(separatorNodes);
            DEBUG(subtreeNodes);
            DEBUG(subtreeRoots);
        }

        if( separatorNodes.size() == dt.height ) return dt;


        VI heightOfNodeStretch(dt.V->size(),-1);
        for( int i=0; i<T; i++ ) for( int d : data[i].sepNodes ) heightOfNodeStretch[d] = data[i].height;

        VVI comps = subtreeNodes;
        Separator sep(*dt.V, separatorNodes);
        sep.createSeparatorStats();
        vector<DepthTree> subtrees;
        for( int i=0; i<subtreeNodes.size(); i++ ){
            VI v = subtreeNodes[i];
            int root = subtreeRoots[i];

            DepthTree tmp(*dt.V);
            tmp.root = root;
            for( int p : v ) tmp.par[p] = dt.par[p];
            tmp.par[root] = -1;
            tmp.height = heightOfNodeStretch[root];

            subtrees.push_back( tmp );

            if(debug)cerr << "Adding subtree " << tmp << endl;
        }

//        sort( ALL(subtrees), []( auto & t1, auto & t2 ){ return t1.height > t2.height; } ); // deepest should be subtrees.back(), no need to sort

        if(debug) DEBUG(subtrees);

        VB inT0( dt.V->size(), false );
        VI T0Nodes;
//        DepthTree* T0 = &subtrees[0]; // deepest should be subtrees.back()
        DepthTree* T0 = &subtrees.back();
        for( PII p : T0->par ){
            inT0[ p.first ] = true;
            T0Nodes.push_back(p.first);
        }

        VI P = separatorNodes;
        VB inP = StandardUtils::toVB( dt.V->size(), P );

        if(debug){
            DEBUG(T0Nodes);
            DEBUG(P);
        }

        VVI* V = dt.V;
        int N = V->size();
        VI inducerSet;

        const bool USE_DEEPEST_PATH = useDeepestPath; // if true, then we get the whole neighborhood of P in T0. Otherwise we get that neighborhood without nodes on deepest path in T0

        if( USE_DEEPEST_PATH ){
            for( int i=0; i<N; i++ ) if( inP[i] || inT0[i] ) inducerSet.push_back(i);
        }else{
            VB onDeepestPath(N,false);
            function< void(int,int) > getDeepestPath = [=,&getDeepestPath, &onDeepestPath, &tree, &data](int num, int par){
                for( int p : data[num].sepNodes ) onDeepestPath[p] = true;
                for( int d : tree[num] ) if( d != par ){ getDeepestPath(d,num); break; }
            };
            getDeepestPath(0,0);
            for( int i=0; i<N; i++ ) if( inP[i] || ( inT0[i] && !onDeepestPath[i] ) ) inducerSet.push_back(i);
        }

        if(debug) DEBUG(inducerSet);

        InducedGraph g = GraphInducer::induce( *V,inducerSet );

        int n = inducerSet.size();
        VB bipartition( n, false );
        for( int p : inducerSet ) if(inT0[p]) bipartition[ g.perm[p] ] = true;

        for( int i=0; i<g.V.size(); i++ ){ // removing all edges between nodes in the same bipartition
            for( int j = (int)g.V[i].size()-1; j>=0; j-- ){
                int p  = g.V[i][j];
                if( bipartition[i] == bipartition[p] ){
                    swap( g.V[i][j], g.V[i].back() );
                    g.V[i].pop_back();
                }
            }
        }

        if(debug)DEBUG(g.V);

        MaxMatchBipartite matcher;
        VI matching = matcher.getMaximumMatchingInBipartition( g.V, bipartition );

        VI violator = matcher.getMaximumHallViolator( g.V, bipartition, matching ); // P2 set but in induced graph
//        VI violator = matcher.getRandomMinimalHallViolator( g.V, bipartition, matching ); // #TEST taking minimal hall violator instead of maximal

        if(debug){
            cerr << "matching:" << endl;
            for(int i=0; i<matching.size(); i++) if( matching[i] != -1 ) cerr << "(" << g.nodes[i] << "," << g.nodes[ matching[i] ] << ")" << endl;
        }

        if( violator.empty() ){
            if(debug) cerr << "violator is empty" << endl;
            return dt; // if P2 is empty - no hall violator exists
        }

        if(debug) DEBUG(violator);

        VI P2 = violator;
        for( int& d : P2 ) d = g.nodes[d]; // remapping P2 from induced graph to V


        VB inP2 = StandardUtils::toVB(N,P2);

        VI neighP2 = GraphUtils::getNeighborhoodOfAInB( *V, P2, T0Nodes, false ); // neighborhood of P2 in T0
        VB inNeighP2 = StandardUtils::toVB(N,neighP2);

        VI PNoP2;
        for( int p : P ) if( !inP2[p] ) PNoP2.push_back(p);


        auto nodeDepths = dt.getNodeDepths();
        sort( ALL(P2), [&nodeDepths](int a, int b){ return nodeDepths[a] < nodeDepths[b]; } ); // P2 is now ordered from topmost to bottommost

        if( neighP2.empty() ) neighP2.push_back(-1); // this should fix the case when neighP2 is empty. Appending -1 should make appropriate attachment node

        if(debug){
            DEBUG(P2);
            DEBUG(neighP2);
            DEBUG(PNoP2);
        }

        unordered_map<int,int> newPar;



        { // creating parent of nodes in T0 \ neighP2
            int lowestPNoP2; // lowest node that is in P but not in P2. If such node does not exist (P = P2) then it is last node of neighP2
            if( PNoP2.size() > 0 ) lowestPNoP2 = PNoP2.back();
            else lowestPNoP2 = neighP2.back();

            sort( ALL(T0Nodes), [&nodeDepths](int a, int b){ return nodeDepths[a] > nodeDepths[b]; } );
            for( int p : T0Nodes ){
                if( inNeighP2[p] ) continue;
                int par = dt.par[p];
                while( inNeighP2[par] ) par = dt.par[par];
                if( inP[par] ) par = lowestPNoP2; // if getting out of tree T0, the we attach to lowest node of P that is not in P2.
                newPar[p] = par;
            }
        }

        if(debug) cerr << "After removing neighP2 from T0, newPar:" << endl << newPar << endl;

        {// creating parents in P \ P2
            for( int i = (int)PNoP2.size()-1; i>=0; i-- ){
                int p = PNoP2[i];
                int par = dt.par[p];
                while( par != -1 && inP2[par] ) par = dt.par[par];
                if( par == -1 ) par = neighP2.back();
                newPar[p] = par;
            }
        }

        if(debug) cerr << "After removing P2 from P, newPar:" << endl << newPar << endl;

        { // assignining parent of P \ P2 to last node in neighP2
            if( !PNoP2.empty() ) newPar[ PNoP2[0] ] = neighP2.back();
        }

        { // assigning parent of P2 to last node of PNoP2 or last node of neighP2 if PNoP2 is empty
            if( PNoP2.empty() ) newPar[ P2[0] ] = neighP2.back();
            else newPar[ P2[0] ] = PNoP2.back();
        }

        if(debug) cerr << "After assignining parent of P \\ P2 to last node in neighP2 and assigning parent of P2 to last node of PNoP2 or last node of neighP2 if PNoP2 is empty:"
             << newPar << endl;

        { // assigning parents in P2
            for( int i=1; i<P2.size(); i++ ) newPar[ P2[i] ] = P2[i-1];
        }

        if(debug) cerr << "After assinging parent in P2:" << endl << newPar << endl;

        { // assigning parents in neighP2
            for( int i=1; i<neighP2.size(); i++ ) newPar[ neighP2[i] ] = neighP2[i-1];
        }


        { // And here we attach all subtrees that were attached to P to the deepest node in P \ P2 + P2
            vector<DepthTree*> hasNeighInP2, noNeighInP2;
//            for( int i=1; i<subtrees.size(); i++ ){
            for( int i=0; i<(int)subtrees.size()-1; i++ ){ // #TEST now deepest subtree is at subtrees.back(), so iterating to isze()-1
                VI TiNodes; // TiNodes is the set of all neighbors of nodes in subtrees[i] (these neighbors are in P)

                if(debug) DEBUG(subtrees[i]);

                for( PII p : subtrees[i].par ) TiNodes.push_back( p.first );

                VI subtreeNodes = TiNodes; TiNodes.clear();
                for( int p : subtreeNodes ){
                    for( int d : (*V)[p] ) if( inP[d] ) TiNodes.push_back(d); // we take only neighbors that are in P - either P2 or P \ P2
                }

                sort(ALL(TiNodes));
                TiNodes.resize( unique(ALL(TiNodes)) - TiNodes.begin() );
                sort( ALL(TiNodes), [&nodeDepths, &inP, &inP2]( int a, int b ) { // nodes in P2 must be after those in P \ P2
                    if( inP2[a] && !inP2[b] ) return false;
                    else if( !inP2[a] && inP2[b] ) return true;
                    else return nodeDepths[a] < nodeDepths[b];
                });

                int rootPar = TiNodes.back();

                if(debug){
                    DEBUG(TiNodes);
                    DEBUG(rootPar);
                }

                for( PII p : subtrees[i].par ){
                    int a = p.first;
                    int b = p.second;
                    if( a == subtrees[i].root ) b = rootPar;
                    newPar[a] = b;
                }
            }
        }

        if(debug) cerr << "After attaching subtrees to deepest node in P \\ P2 + P2:" << endl << newPar << endl;

        { // assigning first node of neighP2 as a root
            if( neighP2[0] != -1 ) newPar[ neighP2[0] ] = -1;
        }

        if(debug) cerr << "After assigning first node of neighP2 as the root:" << endl << newPar << endl;


        DepthTree res(*V);
        for( PII p : newPar ) if( p.second == -1 ){ res.root = p.first; break; }
        res.par = newPar;
        res.height = res.calculateHeight();

        assert( res.par.size() == dt.par.size() );
        assert( res.isCorrect() );

        if(debug) DEBUG(res);

//        exit(1);
        if( res.height < dt.height ) return res;
        else return dt;

    }



    DepthTree makeHallSetPivotsSinglePass(DepthTree &dt, bool useDeepestPath) {
        const bool debug = false;

        VVI* V = dt.V;
        int N = V->size();
        auto strStr = dt.getStretchStructure();
        VVI tree = strStr.first;
        int T = tree.size();

        if( T == 1 ) return dt;

        auto data = strStr.second;
        for( int i=0; i<T; i++ ) sort( ALL(tree[i]), [&data](int a, int b){ return data[a].height > data[b].height; } );

        if(debug){
            for( int i=0; i<T; i++ ) cerr << i << ": " << tree[i] << "\t" << data[i] << endl;

        }

        vector< SubtreeData > subtrees;

        SubtreeData T0(*V);
        unordered_set<int> P, P2, neighP2, onDeepestPath;
        vector< unordered_map< int, unordered_set<int> > > nodeNeighInSubtree(N); // node neighInSubtree[i][root] is a set of neighbors of node i in subtree with given root

        auto nodeDepths = dt.getNodeDepths();

        VI heightOfNodeStretch(dt.V->size(),-1);
        for( int i=0; i<T; i++ ) for( int d : data[i].sepNodes ) heightOfNodeStretch[d] = data[i].height;

        function< bool(int) > isInT0 = [&T0](int a){ return T0.par.count(a); };
        function< bool(int) > isOnDeepestPath = [&onDeepestPath](int a){ return onDeepestPath.count(a); };
        function< bool(int) > isInP = [&P](int a){ return P.count(a); };
        function< bool(int) > isInP2 = [&P2](int a){ return P2.count(a); };
        function< bool(int) > isInNeighP2 = [&neighP2](int a){ return neighP2.count(a); };


        const function<void()> writeState = [&subtrees, &onDeepestPath, &nodeNeighInSubtree, &P, &P2, &T0](){
            DEBUG(P); DEBUG(onDeepestPath);
            DEBUG(T0);
            cerr << "subtrees:" << endl;
            for( auto & d : subtrees ) DEBUG(d);
            cerr << "noeNeighInSubtree:" << endl;
            for( int p : P ){ cerr << p << ": " << nodeNeighInSubtree[p] << endl; }
            ENDL(1);
        };

        auto createInitialState =
                [&dt, &tree, &data, &subtrees, &N, &V, &onDeepestPath, &P, &P2, &T0, &isInP, &isInP2, &isOnDeepestPath, &isInT0, &nodeNeighInSubtree, &writeState](){

            function< void(int) > addSubtree =
                    [&dt, &tree, &data, &subtrees, &N, &V, &onDeepestPath, &P, &P2, &T0, &addSubtree, &isInP, &isInP2, &isOnDeepestPath, &isInT0, &nodeNeighInSubtree](int num){
                SubtreeData* sub = &subtrees.back();
//                cerr << "adding subtree, num = " << num << endl;
                for( int p : data[num].sepNodes ){
                    int par;
                    if( sub->root == p ) par = -1;
                    else par = dt.par[p];
                    sub->par[p] = par;

                    for( int d : (*V)[p] ){
                        if( isInP(d) ){
                            sub->neighInP.insert(d);
                            nodeNeighInSubtree[d][ sub->root ].insert(p);
                        }
                    }
                }
                for( int i=1; i<tree[num].size(); i++ ) addSubtree( tree[num][i] );
            };

            function< void(int) > dfs = [&dt, &tree, &data, &subtrees, &N, &V, &onDeepestPath, &P, &P2, &T0, &dfs, &addSubtree](int num){
//                cerr << "dfs, num = " << num << endl;
                if( tree[num].size() >= 2 ) { // num has sons
                    onDeepestPath.insert(ALL(data[num].sepNodes));
                    P.insert(ALL(data[num].sepNodes));

                    int BEG = 2;
                    if(num == 0) BEG = 1;

                    for (int i = BEG; i < tree[num].size(); i++) { // adding subtrees
                        subtrees.push_back(SubtreeData(*V));
                        int d = tree[num][i];
                        subtrees.back().root = data[d].sepNodes[0];
                        subtrees.back().height = data[d].height;
                        addSubtree(d);
                    }

                    int son = tree[num][1];
                    if( num == 0 ) son = tree[num][0];

                    dfs( son );
                }
                else{ // we are in the deepest leaf
                    subtrees.push_back(SubtreeData(*V));
                    subtrees.back().root = data[num].sepNodes[0];
                    subtrees.back().height = data[num].height;
                    addSubtree(num);
                    T0 = subtrees.back();
                    subtrees.pop_back();
                }
            };

            dfs(0);


            for( int p : T0.neighInP ) nodeNeighInSubtree[p].erase( T0.root );


            if(debug){
                cerr << "Initial state:" << endl;
                writeState();
            }
        };

        createInitialState();


        auto getViolator = [&P, &P2, &isOnDeepestPath,&isInT0, &V, &useDeepestPath](){
            unordered_set<int> inducerSet = P;
            for( int p : P ){
                for( int d : (*V)[p] ){
                    if( useDeepestPath == false ) if( isInT0(d) ) inducerSet.insert(d);
                    else if( isInT0(d) && !isOnDeepestPath(d) ) inducerSet.insert(d);
                }
            }

            VI inducer( ALL(inducerSet) );
            InducedGraph g = GraphInducer::induce( *V, inducer );

            int n = inducerSet.size();
            VB bipartition( n, false );
            for( int p : inducerSet ) if( isInT0(p) ) bipartition[ g.perm[p] ] = true;

            for( int i=0; i<g.V.size(); i++ ){ // removing all edges between nodes in the same bipartition
                for( int j = (int)g.V[i].size()-1; j>=0; j-- ){
                    int p  = g.V[i][j];
                    if( bipartition[i] == bipartition[p] ){
                        swap( g.V[i][j], g.V[i].back() );
                        g.V[i].pop_back();
                    }
                }
            }

            MaxMatchBipartite matcher;
            VI matching = matcher.getMaximumMatchingInBipartition( g.V, bipartition );
            VI violator = matcher.getMaximumHallViolator( g.V, bipartition, matching ); // P2 set but in induced graph

            for( int& d : violator ) d = g.nodes[d];
            P2 = unordered_set<int>(ALL(violator));
        };

        auto getNeighP2 = [&V,&P, &P2, &isInT0, &neighP2](){
            neighP2.clear();
            for(int p : P2){
                for( int d : (*V)[p] ){
                    if( isInT0(d) ) neighP2.insert(d);
                }
            }
        };

        auto checkPivotForImprovement = [&dt, &N, &T, &P, &P2, &neighP2, &isInT0, &data, &tree, &V, &heightOfNodeStretch, &nodeDepths, &nodeNeighInSubtree](){

            if( P2.size() + neighP2.size() >= dt.height ) return false;

            unordered_map<int,int> ordInP2;
            VI tmp( ALL(P2) ); sort(ALL(tmp), [&nodeDepths](int a, int b){ return nodeDepths[a] < nodeDepths[b]; } );
            for( int i=0; i<tmp.size(); i++ ) ordInP2[ tmp[i] ] = i+1;

            if(debug) DEBUG(ordInP2);

            for( int i = (int)tmp.size()-1; i>=0; i-- ){
                int p = tmp[i];

                if(debug){
                    DEBUG(p);
                    DEBUG(ordInP2[p]);
                }
                for( auto &neighs : nodeNeighInSubtree[p] )  {
                    int root = neighs.first;
                    int H = ordInP2[p] + P.size() - P2.size() + neighP2.size() + heightOfNodeStretch[root]; // height of subtree with given root after pivot

                    if(debug) cerr << "height of tree with root " << root << " after (P, P2) - pivot: " << H << endl;

                    if( H >= dt.height ) return false;
                }
            }


            return true;
        };

        auto makePivot =
                [&dt, &N, &T, &P, &P2, &neighP2, &isInT0, &data, &tree, &V, &heightOfNodeStretch, &nodeDepths, &isInP2, &isInP, &isInNeighP2, &subtrees, &T0](){

            VI vecP( ALL(P) ), vecP2(ALL(P2)), vecNeighP2(ALL(neighP2)), PNoP2;


            sort( ALL(vecP) );
            sort(ALL(vecP2));
            if(debug){ DEBUG(vecP); DEBUG(vecP2); DEBUG(PNoP2); }

            set_difference( ALL(vecP), ALL(vecP2), back_inserter(PNoP2) );

            sort( ALL(vecP), [&nodeDepths](int a, int b){ return nodeDepths[a] < nodeDepths[b]; } );
            sort( ALL(vecP2), [&nodeDepths](int a, int b){ return nodeDepths[a] < nodeDepths[b]; } );
            sort( ALL(PNoP2), [&nodeDepths](int a, int b){ return nodeDepths[a] < nodeDepths[b]; } );
            sort( ALL(vecNeighP2), [&nodeDepths](int a, int b){ return nodeDepths[a] < nodeDepths[b]; } ); // this probably is not neccessary

            if( vecNeighP2.empty() ) vecNeighP2.push_back(-1); // this should fix the case when neighP2 is empty. Appending -1 should make appropriate attachment node

            VI T0Nodes;
            for( PII p : T0.par ) T0Nodes.push_back(p.first);

            if(debug){
                DEBUG(P2);
                DEBUG(neighP2);
                DEBUG(PNoP2);
//                DEBUG(T0Nodes);
            }




            unordered_map<int,int> newPar;



            { // creating parent of nodes in T0 \ neighP2
                int lowestPNoP2; // lowest node that is in P but not in P2. If such node does not exist (P = P2) then it is last node of neighP2
                if( PNoP2.size() > 0 ) lowestPNoP2 = PNoP2.back();
                else lowestPNoP2 = vecNeighP2.back();

                sort( ALL(T0Nodes), [&nodeDepths](int a, int b){ return nodeDepths[a] > nodeDepths[b]; } );
                for( int p : T0Nodes ){
                    if( isInNeighP2(p) ) continue;
                    int par = dt.par[p];
                    while( isInNeighP2(par) ) par = dt.par[par];
                    if( isInP(par) ) par = lowestPNoP2; // if getting out of tree T0, the we attach to lowest node of P that is not in P2.
                    newPar[p] = par;
                }
            }

            if(debug) cerr << "After removing neighP2 from T0, newPar:" << endl << newPar << endl;

            {// creating parents in P \ P2
                for( int i = (int)PNoP2.size()-1; i>=0; i-- ){
                    int p = PNoP2[i];
                    int par = dt.par[p];
                    while( par != -1 && isInP2(par) ) par = dt.par[par];
                    if( par == -1 ) par = vecNeighP2.back();
                    newPar[p] = par;
                }
            }

            if(debug) cerr << "After removing P2 from P, newPar:" << endl << newPar << endl;

            { // assignining parent of P \ P2 to last node in neighP2
                if( !PNoP2.empty() ) newPar[ PNoP2[0] ] = vecNeighP2.back();
            }

            { // assigning parent of P2 to last node of PNoP2 or last node of neighP2 if PNoP2 is empty
                if( PNoP2.empty() ) newPar[ vecP2[0] ] = vecNeighP2.back();
                else newPar[ vecP2[0] ] = PNoP2.back();
            }

            if(debug) cerr << "After assignining parent of P \\ P2 to last node in neighP2 and assigning parent of P2 to last node of PNoP2 or last node of neighP2 if PNoP2 is empty:"
                           << newPar << endl;

            { // assigning parents in P2
                for( int i=1; i<P2.size(); i++ ) newPar[ vecP2[i] ] = vecP2[i-1];
            }

            if(debug) cerr << "After assinging parent in P2:" << endl << newPar << endl;

            { // assigning parents in neighP2
                for( int i=1; i<vecNeighP2.size(); i++ ) newPar[ vecNeighP2[i] ] = vecNeighP2[i-1];
            }


            { // And here we attach all subtrees that were attached to P to the deepest node in P \ P2 + P2
                vector<DepthTree*> hasNeighInP2, noNeighInP2;
//            for( int i=1; i<subtrees.size(); i++ ){
                for( int i=0; i<(int)subtrees.size(); i++ ){
                    VI TiNodes; // TiNodes is the set of all neighbors of nodes in subtrees[i] (these neighbors are in P)

                    if(debug) DEBUG(subtrees[i]);

                    for( PII p : subtrees[i].par ) TiNodes.push_back( p.first );

                    VI subtreeNodes = TiNodes; TiNodes.clear();
                    for( int p : subtreeNodes ){
                        for( int d : (*V)[p] ) if( isInP(d) ) TiNodes.push_back(d); // we take only neighbors that are in P - either P2 or P \ P2
                    }

                    sort(ALL(TiNodes));
                    TiNodes.resize( unique(ALL(TiNodes)) - TiNodes.begin() );
                    sort( ALL(TiNodes), [&nodeDepths, &isInP, &isInP2]( int a, int b ) { // nodes in P2 must be after those in P \ P2
                        if( isInP2(a) && !isInP2(b) ) return false;
                        else if( !isInP2(a) && isInP2(b) ) return true;
                        else return nodeDepths[a] < nodeDepths[b];
                    });

                    int rootPar = TiNodes.back();

                    if(debug){
                        DEBUG(TiNodes);
                        DEBUG(rootPar);
                    }

                    for( PII p : subtrees[i].par ){
                        int a = p.first;
                        int b = p.second;
                        if( a == subtrees[i].root ) b = rootPar;
                        newPar[a] = b;
                    }
                }
            }

            if(debug) cerr << "After attaching subtrees to deepest node in P \\ P2 + P2:" << endl << newPar << endl;

            { // assigning first node of neighP2 as a root
                if( vecNeighP2[0] != -1 ) newPar[ vecNeighP2[0] ] = -1;
            }

            if(debug) cerr << "After assigning first node of neighP2 as the root:" << endl << newPar << endl;


            DepthTree res(*V);
            for( PII p : newPar ) if( p.second == -1 ){ res.root = p.first; break; }
            res.par = newPar;
            res.height = res.calculateHeight();

            assert( res.par.size() == dt.par.size() );
            assert( res.isCorrect() );

            if(debug) DEBUG(res);

            return res;
        }; // END OF MAKE_PIVOT


        int currentTreeNode = tree[0][0];
        while( tree[currentTreeNode].size() >= 2 ) currentTreeNode = tree[currentTreeNode][1];
        if(debug) DEBUG(currentTreeNode);
        do{
            getViolator();
            getNeighP2();

            if( !P2.empty() ){
                if( debug ){
                    cerr << "violator found P = " << P << "    P2 = " << P2 << "   neighP2: " << neighP2 << endl;
                }
                if( checkPivotForImprovement() ){
                    if(debug){
                        ENDL(5);
                        cerr << "better tree found" << endl;
                    }

                    auto pivotDt = makePivot();
//                    assert( pivotDt.height <= dt.height );
                    if( pivotDt.height < dt.height ) return pivotDt;
                }else{
                    if(debug){
                        ENDL(2);
                        cerr << "MOVING UP!" << endl;
                        ENDL(2);
                    }
                }
            }

            { // updating neccessary data to move up
                if( currentTreeNode != 0 ){
                    currentTreeNode = tree[currentTreeNode][0]; // moving one 'stretch' up the tree
                    int BEG = 2;
                    if( currentTreeNode == 0 ) BEG = 1;
                    int lastRoot = data[ tree[ currentTreeNode ][BEG] ].sepNodes[0];
                    if(debug){
                        DEBUG(lastRoot);
                        DEBUG(currentTreeNode);
                    }

                    SubtreeData sub(*V);

                    // removing from P and deepestPath
                    for(int p : data[currentTreeNode].sepNodes){
                        onDeepestPath.erase(p);
                        P.erase(p);
                    }

                    if(debug){ cerr << "After removing from P and deepestPath: " << endl;writeState(); }

                    do{
                       sub = subtrees.back();
                       subtrees.pop_back();

                       if(debug) cerr << "merging to T0 subtree " << sub << endl;

                       // updating T0.root and sepNodes in currentTreeNode
                       T0.par[ T0.root ] = dt.par[T0.root];
                       for( int p : data[currentTreeNode].sepNodes ) T0.par[p] = dt.par[p];
                       T0.par[ data[currentTreeNode].sepNodes[0] ] = -1;

                       // updating subtree sub
                       for( PII p : sub.par ){
                           int a = p.first;
                           int b = p.second;
                           if( b == -1 ) b = dt.par[a];
                           T0.par[a] = b;
                       }

                       // updating data[currentTreeNode].sepNodes values of nodeNeighInSubtree
                       for( int p : sub.neighInP ){
                           nodeNeighInSubtree[p].erase( sub.root );
                       }

                    }while( sub.root != lastRoot );

                    if(debug){
                        cerr << "After moving up, currentTreeNode: " << currentTreeNode << endl;
                        writeState();
                        ENDL(3);
                    }

                }
                else currentTreeNode = -1;
            }
        }while( currentTreeNode != 0 );


        return dt;
    }

    DepthTree makeAllPivots(DepthTree &dt) {
        if( dt.height <= 3 ) return dt;
        auto res = dt;


//        cerr << "making usual pivots" << endl;

        auto pivotDt = DepthTreePivotMaker::makePivot(res);
        while ((pivotDt.height < res.height) ) {

            if (pivotDt.height < res.height) {
//                cerr << "usual pivot makes better!" << endl;
//                DEBUG(res.height);
//                DEBUG(pivotDt.height);
//                ENDL(1);

                res = pivotDt;
            }
            pivotDt = DepthTreePivotMaker::makePivot(pivotDt);
        }
        assert(res.isCorrect());


//        VD balances = { 0.5, 0.4, 0.3, 0.2, 0.1 }; // original version
        VD balances = { 0.5, 0.3, 0.1 }; // #TEST
        if( Pace20Params::quickAndWeakTreeCreation ) balances = {};
        else if( Pace20Params::inputGraphEdges <= 100'000 ) balances = { 0.60, 0.45, 0.30, 0.15 };
        else if( Pace20Params::inputGraphEdges <= 10'000 ) balances = { 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1 };

//        cerr << "making multiple-stretch pivots, balances = " << balances << endl;


        for( double balance : balances ){
            pivotDt = makePivotMultipleStretch( res, balance );
            if( pivotDt.height < res.height ){
//                cerr << "pivot multiple stretch makes better!" << endl;
//                DEBUG(res.height);
//                DEBUG(pivotDt.height);
//                ENDL(1);
                res = pivotDt;
//                balance = 1; // starting from the beginning
            }
        }

//        cerr << "done" << endl;



        bool useHallSetPivots = true;
        if (Pace20Params::quickAndWeakTreeCreation) useHallSetPivots = false;
        else if( Pace20Params::inputGraphSize > 100'000 && dt.V->size() < 100 ) useHallSetPivots = false;


        if(useHallSetPivots) {
//            cerr << "making HS-pivots-balance" << endl;

            if (Pace20Params::inputGraphSize < 100'000) balances = {0.95, 0.9, 0.85, 0.8, 0.75, 0.7, 0.65, 0.6}; // #TEST
//            else balances = {0.9, 0.85, 0.8, 0.75, 0.7}; // keeping only those that appear most often in improvements
            else balances = {0.95, 0.9, 0.85, 0.8, 0.75 }; // #TEST - 0.95 seems to be much more often that 0.7

//            if( Pace20Params::inputGraphSize <= 500 ) balances = { 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.01 }; // #TEST - for exact track




//            small values (  0.65, 0.7, 0.75 ) of balances make improvements in trees with small height (e.g improves height 6 for 5), while larger
//            make improvements in trees with greater heights

            {

                reverse( ALL(balances) ); // #TEST - balances seem to improve tree repeatedly but most often only in increasing steps of balance
                for (int i = 0; i < balances.size(); i++) {
                    double balance = balances[i];
                    pivotDt = makeHallSetPivots(res, balance, false);
                    if (pivotDt.height < res.height) {
//                        cerr << "Hall-set-pivotDt makes better tree!" << endl;
//                        DEBUG(res);DEBUG(pivotDt);DEBUG(balance);ENDL(1);
                        res = pivotDt;
//                        i = -1; // original
                        i--; // #TEST - it seems that multiple improvements are done most often in increasing sequence of balances
                    }/* else { // #TEST
                        pivotDt = makeHallSetPivots(res, 1 - balance, true);
                        if (pivotDt.height < res.height) {
                            cerr << "Hall-set-pivotDt-DEEPEST_PATH makes better tree!" << endl;
                            DEBUG(res);DEBUG(pivotDt);DEBUG(balance);ENDL(1);
                            res = pivotDt;  i = -1;
                        }
                    }*/
                }

                { // #TEST section
                    balances.clear(); for( double d = 0.45; d >= 0.18; d -= 0.05 ) balances.push_back(d);
                    reverse( ALL(balances) ); // #TEST - balances seem to improve tree repeatedly but most often only in increasing steps of balance

                    for (int i = 0; i < balances.size(); i++) {
                        double balance = balances[i];
                        pivotDt = makeHallSetPivots(res, 1 - balance, true);
                        if (pivotDt.height < res.height) {
//                            cerr << "Hall-set-pivotDt-DEEPEST_PATH makes better tree!" << endl;
//                            DEBUG(res);DEBUG(pivotDt);DEBUG(balance);ENDL(1);
                            res = pivotDt;
//                            i = -1;// original
                            i--;// #TEST - it seems that multiple improvements are done most often in increasing sequence of balances
                        }
                    }
                }

            }


            bool useHallSetPivotsSinglePass = false;
            if( Pace20Params::inputGraphSize <= 500 ) useHallSetPivotsSinglePass = true;
            if(useHallSetPivotsSinglePass){
                int H;
                do {
                    H = res.height;

//                    cerr << "making HS-pivots-single-pass" << endl;

                    res = makeHallSetPivotsSinglePass(res, false);
                    res = makeHallSetPivotsSinglePass(res, true);



//                    if (res.height < H) {
//                        cerr << "Hall-set-single-pass makes better tree!" << endl;
//                        DEBUG(res);DEBUG(pivotDt);ENDL(1);
//                        DEBUG(H);
//                        DEBUG(res.height);
//                        ENDL(1);
//                    }

                } while (res.height < H);
            }

        }

//        cerr << "exiting makeAllPivots" << endl;

        return res;

    }





    void test(){
        if(false) { // usual pivot test
//        VVI V = { {1,2,4}, {0,5}, {0}, {4}, {3,0,7}, {1,6}, {5,10,12}, {4,8}, {7,9}, {8}, {6,11}, {10}, {6,13}, {12} }; // tree
//        VVI V = { {1,2,4}, {0,5}, {0}, {4,9}, {3,0,7}, {1,6}, {5,10,12}, {4,8}, {7,9}, {8,3}, {6,11}, {10}, {6,13}, {12} }; // additional edge (9,3)
            VVI V = {{1, 2,  4},
                     {0, 5},
                     {0},
                     {4},
                     {3, 0,  7,  6},
                     {1, 6},
                     {5, 10, 12, 4},
                     {4, 8},
                     {7, 9},
                     {8},
                     {6, 11},
                     {10},
                     {6, 13},
                     {12}}; // additional edge (4,6)

            int N = V.size();
            DepthTree dt(V);
            dt.root = 3;
            dt.height = 8;
            dt.V = &V;
            dt.par = {{3,  -1},
                      {4,  3},
                      {0,  4},
                      {2,  0},
                      {1,  0},
                      {5,  1},
                      {6,  5},
                      {10, 6},
                      {11, 10},
                      {12, 6},
                      {13, 12},
                      {7,  4},
                      {8,  7},
                      {9,  8}};

            DEBUG(dt);

            auto resDt = makePivot(dt);
            DEBUG(resDt);
            ENDL(1);

            resDt = makePivot(resDt);
            DEBUG(resDt);
            ENDL(1);

            resDt = makePivot(resDt);
            DEBUG(resDt);
            ENDL(1);
        }

        if(false){ // multiple stretch test
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

//                GraphUtils::addEdge( V,N, N/2 );
//                for( int i=N; i < 4*N; i++ )  for( int k=i+1; k < 4*N; k++ ) GraphUtils::addEdge( V,i,k );

            //    DEBUG(V);
            dt.height = dt.calculateHeight();
            dt.writeBalanceStructure();

            makePivotMultipleStretch( dt,0.2 );

        }


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

            double balance = 0.4;
//            auto res = makeHallSetPivots( dt, balance );
//            auto res = makeAllPivots( dt );

            auto res = makeHallSetPivotsSinglePass( dt ); ENDL(10);
            res = makeHallSetPivotsSinglePass( res ); ENDL(10);
            res = makeHallSetPivotsSinglePass( res ); ENDL(10);
        }



        exit(1);
    }



}