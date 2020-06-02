/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/
#include <CONTESTS/PACE20/separatorcreators/BFSSeparatorCreator.h>
#include <CONTESTS/PACE20/separatorminimizers/GreedyNodeEdgeMinimizer.h>
#include <graphs/graphtraversals/BFS.h>
#include <graphs/GraphInducer.h>
#include <datastructures/FAU.h>
#include <CONTESTS/PACE20/SeparatorEvaluators.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include <CONTESTS/PACE20/separatorminimizers/LargestComponentsVCMinimizer.h>
#include "graphs/VertexCover/BipartiteGraphVertexCover.h"
#include "graphs/GraphUtils.h"
#include "numbertheory/Binom.h"
#include "combinatorics/CombinatoricUtils.h"


BFSSeparatorCreator::BFSSeparatorCreator(VVI &V) {
    this->V = V;
}

vector<Separator> BFSSeparatorCreator::createSeparators(VVI &V, int maxSources) {
//    const int MAX_SOURCES = min( Pace20Params::maxSources, (int)V.size()-1 );
    const int MAX_SOURCES = min( max(1,maxSources), (int)V.size()-1 );
//    DEBUG(MAX_SOURCES);
    VI sourceQuantities(MAX_SOURCES+1);
    for( int i=1; i <= MAX_SOURCES /*min(MAX_SOURCES,10)*/; i++ ) sourceQuantities[i] = MAX_SOURCES+1-i;

//    cerr << "TESTING LARGE SOURCE_SETS" << endl;
//    sourceQuantities = VI(2,0 );
//    sourceQuantities.back() = 30;


    /*sourceQuantities = { 0, 60, 40, 20, 10, 5 };
    if( Pace20Params::inputGraphSize > 100'000 )*/ sourceQuantities = { 0, 16, 8, 4 };

    if( Pace20Params::quickAndWeakTreeCreation ) sourceQuantities = {0,1};

    VVI sourcesSets = getRandomSources(V.size(),sourceQuantities);


//    DEBUG(sourcesSets.size());

    vector<Separator> res;
    {
        for (VI &sources : sourcesSets) {
            if (Pace20Params::tle) break;

//            sources = { 0 };
            auto layerSeps = createLayerSeparators(sources);
            auto *seps = &layerSeps;

            for (auto &sp : *seps) {
                if (sp.stats.size != sp.nodes.size()) ERROR("", sp);
            }

            res.insert(res.end(), ALL((*seps)));

            if( isSetSepEval ) {
                sort(ALL(layerSeps), sepEval);
                auto newSeps = createLayerSeparators(layerSeps[0].nodes);
                res.insert(res.end(), ALL(newSeps));
            }

        }
    }

    return res;
}


vector<Separator> BFSSeparatorCreator::createLayerSeparators(VI sources){


    bool debug = false;

//    cerr << "Setting sources to 0,10,20,30,40" << endl;
//    sources = {0,10,20,30,40};



    VVI layers = BFS::getBfsLayers(V, sources);


//    DEBUG(layers);

//    DEBUG(layers.size());
//    cerr << "{";
//    for( int i=0; i<layers.size(); i++ ) cerr << layers[i].size() << "  ";
//    cerr << "}" << endl;
//    ENDL(1);


    int N = V.size();
    int L = layers.size();

    if( L == 1 ){
        Separator res(V,{0});
        res.createSeparatorStats();
        return {res};
    }

    assert(L > 1);



    /**
     * compBorderR[i] is a partiiton of nodes in layer i that are in the same connected components in graph L_i +L_{i+1} + ...  (on the right of layer i)
     * compBorderL[i] is a partiiton of nodes in layer i that are in the same connected components in graph L_0 +L_{1} + ... L_{i}  (on the left of layer i)
     *
     */
    VVVI compBorderR(L), compBorderL(L);

    /**
     * nodesInCompL[i] is the number of nodes in a connected component of graph L_0 + ... + L_inLayer[i], to which i belongs
     * The same for R suffix, but from the other side.
     */
    VI nodesInCompL(N), nodesInCompR(N), edgesInCompL(N), edgesInCompR(N);

    /**
     * ComponentIdL is the index of 'component border' to which i belongs (in the graph V[ L_0 + ... + L_i ]). In other words it is such a number j, that
     * i belongs to compBorderL[ inLayer[i] ][ componentIdL[i] ]
     */
    VI componentIdL(N), componentIdR(N);




    auto createLayerData = [=,&L,&N]( VVI & layers, VVVI& compBorder, VI& nodesInComp, VI& edgesInComp, VI& componentId ){

        if(debug) DEBUG(layers);

        /**
         * inLayer[i] is the layer in which node i is
         */
        VI inLayer(N);
        for( int i=0; i<L; i++ ) for(int d : layers[i]) inLayer[d] = i;


        /**
         * Helper to quickly map components in layer merged using fau
         */
        VVI borders(N);

        VI idPar(N,0);

        VB bordCntVec(N,false);
        VI bordCnt;

        /**
         * borderNeigh[j] is the set of components {x_0, x_1, ... } in layer i-1, such j-th component in layer i have a neighbor in components x_0, ...
         */
//        unordered_map<int,unordered_set<int>> borderNeigh;
        vector< unordered_set<int> > borderNeigh(N);

        // creating left borders
        FAU fau(V.size());
        for( int i=0; i<L; i++ ){

            if(debug)DEBUG(layers);

            VI l = layers[i];

            if(debug)cerr << "Layer #i = " << i << ":  " << l << endl;

            for( int p : l ){
                for( int d : V[p] ){
                    if( inLayer[d] <= i ){
                        fau.Union(p,d);
                    }
                }
            }


            for(int p : l) borders[ fau.Find(p) ].clear();

            for(int p : l){
                int x = fau.Find(p);
                borders[ x ].push_back(p);
                if( !bordCntVec[ x ] ){
                    bordCnt.push_back(x);
                    bordCntVec[x] = true;
                }
            }

            if(debug)DEBUG(bordCnt);


            int cnt = 0;
            for( int t : bordCnt ){
                compBorder[i].push_back( borders[t] );
                idPar[t] = cnt++;

                bordCntVec[t] = false;
            }

            bordCnt.clear();

            for(int p : l){
                componentId[p] = idPar[ fau.Find(p) ];
                nodesInComp[p] = fau.countNodesInComponent(p);

                if(debug){
                    DEBUG(p);
                    DEBUG(componentId[p]);
                }
            }

            if(debug)DEBUG(compBorder[i]);

            // here compBorderL and componentIdL should be correctly done


            for( int j=0; j<compBorder[i].size(); j++ ){
                VI b = compBorder[i][j];
                int edges = 0;
                for( int p : b ){
                    for( int d : V[p] ){
                        if( inLayer[d] == i && p < d ){
                            edges++;
                        }
                    }
                }

                if(debug)cerr << "Edges in i-th layer: " << edges << endl;

                for(int p : b) edgesInComp[p] = edges;
            }

            if(i > 0){

                for( int j=0; j<compBorder[i].size(); j++ ) {
                    VI b = compBorder[i][j];
                    borderNeigh[j].clear();

                    for (int p : b) {
                        for (int d : V[p]) {
                            if (inLayer[d] == i - 1) {
                                borderNeigh[j].insert( componentId[ d ] );
                            }
                        }
                    }

                    if(debug)for (int c : borderNeigh[j]) DEBUG(compBorder[i-1][c]);

                    int edges = 0;
                    for (int p : b) {
                        for (int d : V[p]) {
                            if (inLayer[d] == i - 1) {
                                edges++;
                            }
                        }
                    }

                    if(debug){
                        cerr << "Edges between " << i << "-th and " << (i + 1) << "-th layer: " << edges << endl;
                        DEBUG(edges);
                    }

                    for (int p : b) edgesInComp[p] += edges;

                    edges = 0;
                    for (int comp : borderNeigh[j]) {
                        int representative = compBorder[i - 1][comp][0];
                        if(debug){
                            DEBUG(comp);
                            DEBUG(representative);
                        }
                        edges += edgesInComp[representative];
                    }

                    if(debug)cerr << "Edges from components " << compBorder[i][j] << ":  " << edges << endl;
                    for (int p : b) edgesInComp[p] += edges;

                }

            }

            if(debug){
                cerr << "There are " << compBorder[i].size() << " border components" << endl;
                for( int j=0; j<compBorder[i].size(); j++ ) {
                    VI b = compBorder[i][j];
                    DEBUG(b);
                }

                for( int p : l ){
                    DEBUG(p);
                    DEBUG(nodesInComp[p]);
                    DEBUG(edgesInComp[p]);
                    ENDL(1);
                }
                ENDL(5);
            }
        }

    };

    createLayerData( layers, compBorderL, nodesInCompL, edgesInCompL, componentIdL );

    reverse( ALL(layers) );
    createLayerData( layers, compBorderR, nodesInCompR, edgesInCompR, componentIdR );
    reverse(ALL(layers));
    reverse( ALL(compBorderR) );



    vector<Separator> separators;

    if(debug){
        for( int i=0; i<L; i++ ){
            DEBUG(i);
            DEBUG(compBorderL[i].size());
            DEBUG(compBorderR[i].size());
        }
    }


    /**
     * inLayer[i] is the layer in which node i is
     */
    VI inLayer(N);
    for( int i=0; i<L; i++ ) for(int d : layers[i]) inLayer[d] = i;

    const int ORIGINAL_GRAPH_EDGES = GraphUtils::countEdges(V);
    auto createSeparatorForLayer = [=,&edgesInCompL, &edgesInCompR, &nodesInCompL, &nodesInCompR, &compBorderL, &compBorderR]( int i ){


        Separator sep( V,layers[i] );
        sep.stats.originalGraphEdges = ORIGINAL_GRAPH_EDGES;
        sep.stats.originalGraphSize = V.size();

        if( i > 0 ){
            for( int p : layers[i] ){

                for( int d : V[p] ){
                    if( inLayer[d] == i-1 ){
                        sep.stats.maxCompEdges = max( sep.stats.maxCompEdges, edgesInCompL[d] );
                        sep.stats.maxCompSize = max( sep.stats.maxCompSize, nodesInCompL[d] );
                        sep.stats.maxCompAvgDeg = max( sep.stats.maxCompAvgDeg, (double)2*edgesInCompL[d] / nodesInCompL[d]  );
                        sep.stats.maxCompDensity = max( sep.stats.maxCompDensity, (double)2*edgesInCompL[d] / ( nodesInCompL[d] * nodesInCompL[d] )  );
                    }
                }
            }

            sep.stats.numberOfComponents += compBorderL[i-1].size();
            for( VI & b : compBorderL[i-1] ) sep.stats.compsSizeSquares += nodesInCompL[b[0]] * nodesInCompL[b[0]];
        }

        if( i < L-1 ){
            for( int p : layers[i] ){

                for( int d : V[p] ){
                    if( inLayer[d] == i+1 ){
                        sep.stats.maxCompEdges = max( sep.stats.maxCompEdges, edgesInCompR[d] );
                        sep.stats.maxCompSize = max( sep.stats.maxCompSize, nodesInCompR[d] );
                        sep.stats.maxCompAvgDeg = max( sep.stats.maxCompAvgDeg, (double)2*edgesInCompR[d] / nodesInCompR[d]  );
                        sep.stats.maxCompDensity = max( sep.stats.maxCompDensity, (double)2*edgesInCompR[d] / ( nodesInCompR[d] * max(1,(nodesInCompR[d]-1)) )  );
                    }
                }
            }

            sep.stats.numberOfComponents += compBorderR[i+1].size();
            for( VI & b : compBorderR[i+1] ) sep.stats.compsSizeSquares += nodesInCompR[b[0]] * nodesInCompR[b[0]];
        }

        sep.stats.size = sep.nodes.size();
        return sep;
    };

    for( int i=0; i<L; i++ ){
        separators.push_back( createSeparatorForLayer(i) );
    }

    if(debug) DEBUG(separators);

    const bool USE_GREEDY_MINIMIZER = false;
//    DEBUG(USE_GREEDY_MINIMIZER);

    if(USE_GREEDY_MINIMIZER) {

        vector<unordered_map<int, int> > W(N);
        VI mapper(N, -1);
        auto getSeparatorGraph = [=, &compBorderL, &compBorderR, &nodesInCompL, &edgesInCompL, &edgesInCompR, &nodesInCompR, &W, &mapper](
                Separator &sep, int i, VI &nodeW, VI &edgeW) {

            bool debug = false;

            int C = 0;
            if (i > 0) C += compBorderL[i - 1].size();
            if (i < L - 1) C += compBorderR[i + 1].size();

            edgeW = VI(sep.nodes.size() + C, 0);
            nodeW = VI(sep.nodes.size() + C, 1);

            if (debug) {
                ENDL(3);
                DEBUG(sep);
                DEBUG(C);
                DEBUG(i);
            }


            VVPII res(sep.nodes.size() + C);

//        unordered_map<int, unordered_map<int,int> > W;

//        unordered_map<int,int> mapper;
            int cnt = sep.nodes.size();

            if (i > 0) {

                for (int j = 0; j < compBorderL[i - 1].size(); j++) {
                    VI b = compBorderL[i - 1][j];
                    for (int p : b) mapper[p] = cnt;
                    nodeW[cnt] = nodesInCompL[b[0]];
                    edgeW[cnt] = edgesInCompL[b[0]];
                    cnt++;
                }

            }

            if (i < L - 1) {
                for (int j = 0; j < compBorderR[i + 1].size(); j++) {
                    VI b = compBorderR[i + 1][j];
                    for (int p : b) mapper[p] = cnt;
                    nodeW[cnt] = nodesInCompR[b[0]];
                    edgeW[cnt] = edgesInCompR[b[0]];
                    cnt++;
                }
            }

            for (int j = 0; j < sep.nodes.size(); j++) mapper[sep.nodes[j]] = j;


            unordered_set<int> neigh;
            for (int j = 0; j < sep.nodes.size(); j++) {
                int p = sep.nodes[j];

                W[j].clear();

                neigh.clear();
                for (int k = 0; k < V[p].size(); k++) {
                    int d = V[p][k];
                    if (inLayer[d] == i) {
                        res[j].push_back({mapper[d], 1});
                    } else {
                        neigh.insert(mapper[d]);
                        W[j][mapper[d]]++;
                    }
                }

                for (int d : neigh) {
                    res[j].push_back({d, W[j][d]});
                    res[d].push_back({j, W[j][d]});
                }
            }

            if (debug) {
                DEBUG(nodeW);
                DEBUG(edgeW);
                DEBUG(res);
            }

            return res;
        };


        GreedyNodeEdgeMinimizer gEdgeNodeMinim(GreedyNodeEdgeMinimizer::MINIMIZE_EDGES);
        VI separatorsToRemove;
        for(int i=0; i<L; i++){

            if( separators[i].nodes.size() > Pace20Params::maxSeparatorSizeForGNEMinimizer ) continue;

            if( separators[i].stats.numberOfComponents <= 1 ){
                separatorsToRemove.push_back(i);
                continue;
            }

            VI nodeW, edgeW;
            auto sepGraph = getSeparatorGraph(separators[i],i, nodeW, edgeW);

            auto sp = gEdgeNodeMinim.minimizeSeparator( separators[i], sepGraph, nodeW, edgeW ); // adding minimized one as new separator
            separators.push_back(sp);
        }

        int p = L-1;
        while( !separatorsToRemove.empty() ){
            if( separatorsToRemove.back() == p ){
                swap( separators[p], separators.back() );
                separators.pop_back();
                separatorsToRemove.pop_back();
            }
            p--;
        }


    }else {


        //***************** SECTION WITH FAST MINIMIZER



        auto createLargestComponentsSeparatorForLayer = [=, &edgesInCompL, &edgesInCompR, &nodesInCompL, &nodesInCompR, &compBorderL, &compBorderR]
                (int i, VB &was, FAU &fau, bool useVC = false) {

            struct Comp {
                Comp(int l, int ind, int e, int n) {
                    layer = l;
                    index = ind;
                    edges = e;
                    nodes = n;
                }

                int layer, index, edges, nodes;
            };

            vector<Comp> comps;
            /*if( i > 0 ){
                for( int j=0; j<compBorderL[i-1].size(); j++ ){
                    int repr = compBorderL[i-1][j][0];
                    comps.emplace_back( i-1,j, edgesInCompL[repr], nodesInCompL[repr] );
                }
            }*/

            if (i < L - 1) {
                for (int j = 0; j < compBorderR[i + 1].size(); j++) {
                    int repr = compBorderR[i + 1][j][0];
                    comps.emplace_back(i + 1, j, edgesInCompR[repr], nodesInCompR[repr]);
                }
            }

            sort(ALL(comps), [](auto c1, auto c2) { return c1.edges > c2.edges; });

            // ONLY TESTING NOT TAKING LEFT COMPONENT
            if (i > 0) { // ONLY TESTING NOT TAKING LEFT COMPONENT
                for (int j = 0; j < compBorderL[i - 1].size(); j++) {
                    int repr = compBorderL[i - 1][j][0];
                    comps.emplace_back(i - 1, j, edgesInCompL[repr], nodesInCompL[repr]);
                }
            } // ONLY TESTING NOT TAKING LEFT COMPONENT

            int totalEdges = accumulate(ALL(comps), 0, [](int a, Comp &c) { return a + c.edges; });
            int totalNodes = accumulate(ALL(comps), 0, [](int a, Comp &c) { return a + c.nodes; });

//        DEBUG(totalEdges);
//        DEBUG(totalNodes);
//            cerr << "layer[i].size: " << layers[i].size() << endl;
//            for (auto cmp : comps) {
//                if (cmp.edges > 200)
//                    cerr << "cmp: layer: " << cmp.layer << ", index: " << cmp.index << "   edges: " << cmp.edges
//                         << "   nodes: " << cmp.nodes << endl;
//            }


            vector<Comp> largestComps;
            int lcEdges = 0;
            int lcNodes = 0;
            for (int j = 0; j < comps.size(); j++) {
                largestComps.push_back(comps[j]);
                lcEdges += comps[j].edges;
                lcNodes += comps[j].nodes;
                if (lcEdges > (1 - Pace20Params::balance) * totalEdges) break;
            }

//            cerr << "largest comps: " << endl;
//            for (auto cmp : largestComps)
//                cerr << "cmp: layer: " << cmp.layer << ", index: " << cmp.index << "   edges: " << cmp.edges
//                     << "   nodes: " << cmp.nodes << endl;

            VI nodes;
            for (auto cmp : largestComps) {
                VI b;
                if (cmp.layer == i - 1) b = compBorderL[i - 1][cmp.index];
                else if (cmp.layer == i + 1) b = compBorderR[i + 1][cmp.index];

                for (int p : b) {
                    for (int d : V[p]) {
                        if (inLayer[d] == i && !was[d]) {
                            was[d] = true;
                            nodes.push_back(d);
                        }
                    }
                }
            }

            // now nodes is just the neighborhood of largestComponents intersected with i-th layer
            // CAUTION!! STATS IN THIS CASE MAY BE WORSE THAN ORIGINALLY! vertices in vc, but NOT in layer[i] are not counted, and they may disjoin the graph more
            // than just those from vc that are in i-th layer.
            if( useVC ){
                for( int p : nodes ) was[p] = false;

                VI inducerSet = nodes;
                for (auto cmp : largestComps) {
                    VI b;
                    if (cmp.layer == i - 1) b = compBorderL[i - 1][cmp.index];
                    else if (cmp.layer == i + 1) b = compBorderR[i + 1][cmp.index];

                    for (int p : b) {
                        for (int d : V[p]) {
                            if (inLayer[d] == i && !was[d]) {
                                inducerSet.push_back(p);
                                break;
                            }
                        }
                    }
                }

//                DEBUG(inducerSet);

                InducedGraph g = GraphInducer::induce( V, inducerSet );
                VB bipartition( g.V.size(),false );
                for( int p : nodes ) bipartition[ g.perm[p] ] = true;
                VI vc = BipartiteGraphVertexCover::getVertexCoverOfBipartiteGraph(g.V, bipartition);

                for(int& d : vc) d = g.nodes[d];
//                DEBUG(vc);

//                nodes.clear();
                nodes = vc;
                for( int d : vc ){
                    if( inLayer[d] == i ){
//                        nodes.push_back(d);
                        was[d] = true;
                    }
                }

            }




//            DEBUG(nodes);


            Separator sep(V, nodes);
            sep.stats.numberOfComponents = largestComps.size();
            sep.stats.maxCompSize = (*max_element(ALL(largestComps),
                                                  [](auto cmp1, auto cmp2) { return cmp1.nodes < cmp2.nodes; })).nodes;
            sep.stats.maxCompEdges = (*max_element(ALL(largestComps),
                                                   [](auto cmp1, auto cmp2) { return cmp1.edges < cmp2.edges; })).edges;

            unordered_map<int, int> edgesCnt, nodesCnt;

            for (int d : layers[i]) if (!was[d]) nodesCnt[d] = 1;

            for (int j = largestComps.size(); j < comps.size(); j++) { // merging smaller components
                Comp cmp = comps[j];
                VI b;
                if (cmp.layer == i - 1) b = compBorderL[i - 1][cmp.index];
                else if (cmp.layer == i + 1) b = compBorderR[i + 1][cmp.index];

                unordered_set<int> toMergeSet;
                for (int p : b) {
                    for (int d : V[p]) {
                        if (inLayer[d] == i && !was[d]) {
                            toMergeSet.insert(d);
                            edgesCnt[d]++;
                        }
                    }
                }

                VI toMerge(ALL(toMergeSet));
//            DEBUG(b);
//            DEBUG(toMerge);

                int p = b[0];
                if (toMerge.empty()) {
                    sep.stats.maxCompSize = max(sep.stats.maxCompSize,
                                                (cmp.layer == i - 1) ? nodesInCompL[p] : nodesInCompR[p]);
                    sep.stats.maxCompEdges = max(sep.stats.maxCompEdges,
                                                 (cmp.layer == i - 1) ? edgesInCompL[p] : edgesInCompR[p]);
                    sep.stats.numberOfComponents++;
                } else {
                    edgesCnt[toMerge[0]] += (cmp.layer == i - 1) ? edgesInCompL[p] : edgesInCompR[p];
                    nodesCnt[toMerge[0]] += (cmp.layer == i - 1) ? nodesInCompL[p] : nodesInCompR[p];

//                cerr << "Increasing size of " << toMerge[0] << " by size of component containing "
//                    << p << " by " << ((cmp.layer == i-1) ? edgesInCompL[ p ] : edgesInCompR[p] ) << " edges" << endl;
//                cerr << "Increasing size of " << toMerge[0] << " by size of component containing "
//                     << p << " by " << ( (cmp.layer == i-1) ? nodesInCompL[ p ] : nodesInCompR[p] ) << " nodes" << endl;

                    for (int k = 1; k < toMerge.size(); k++) {
                        int a = toMerge[k - 1];
                        int b = toMerge[k];
                        fau.Union(a, b);
                    }
                }
            }

//        DEBUG(edgesCnt);
//        DEBUG(nodesCnt);

            for (int d : layers[i]) {
                if (was[d])continue;
                for (int p : V[d]) if (inLayer[p] == i && !was[p]) fau.Union(d, p);
            }

//        for( int d : layers[i] ){
//            cerr << "fau.Find(" << d << ") = " << fau.Find(d) << endl;
//        }

            unordered_map<int, int> sumEdges, sumNodes;
            unordered_map<int, VI> classes;
            for (int d : layers[i]) {
                if (was[d]) continue;
                int x = fau.Find(d);
                classes[x].push_back(d);
                sumEdges[x] += edgesCnt[d];
                sumNodes[x] += nodesCnt[d];
            }

//        DEBUG(classes);

            for (pair<int, VI> c : classes) {
                sep.stats.numberOfComponents++;
                int x = c.first;
                VI v = c.second;
                int e = 0; // number of edges between nodes in class containing node x
                for (int d : v) {
                    for (int p : V[d]) if (inLayer[p] == i && !was[p]) e++;
                }
                e >>= 1;
//            DEBUG(x);
//            DEBUG(e);
//            DEBUG(v);
//            DEBUG(was);
                sumEdges[x] += e;
            }

//        DEBUG(sumNodes);
//        DEBUG(sumEdges);

            int maxEdges = 0, maxNodes = 0;
            for (PII p : sumEdges) maxEdges = max(maxEdges, p.second);
            for (PII p : sumNodes) maxNodes = max(maxNodes, p.second);


            sep.stats.maxCompSize = max(sep.stats.maxCompSize, maxNodes);
            sep.stats.maxCompEdges = max(sep.stats.maxCompEdges, maxEdges);
            sep.stats.size = nodes.size();


            return sep;
//        exit(1);
        };


        VB was(N, false);
        FAU fau(N);

        int ogNodes = V.size();
        int ogEdges = GraphUtils::countEdges(V);
        for (int i = 0; i < L; i++) {

            for( int useVC = 0; useVC <= ( Pace20Params::quickAndWeakTreeCreation ? 0 : 1 )   ; useVC++ ) {
//                DEBUG(layers[i]);
//                DEBUG(was);
//                for( int k=0; k<V.size(); k++ ) cerr << fau.Find(k) << " "; cerr << endl;

                Separator sep = createLargestComponentsSeparatorForLayer(i, was, fau, useVC);

//            Separator sep = createLargestComponentsSeparatorForLayer(i, was, fau, true);
//                DEBUG(sep.nodes);
//                DEBUG(sep.stats);

                for (int d : layers[i]) {
                    fau.getP()[d] = -1;
                    fau.getW()[d] = 1;
                    was[d] = false;
                }
                sep.stats.originalGraphSize = ogNodes;
                sep.stats.originalGraphEdges = ogEdges;
                sep.updatePointers(V);

                /*if(useVC){
                    cerr << "\rin BFS createSeparatorStats()" << flush;
                    sep.createSeparatorStats();
                }*/

                separators.push_back(sep);
//                break;
            }

//            ENDL(5);


//        DEBUG( layers[i].size() );
//            DEBUG(sep.stats);
//            ENDL(1);


    //        if( sep.stats.size < layers[i].size() ){
    //            GreedyNodeEdgeMinimizer gEdgeNodeMinim( GreedyNodeEdgeMinimizer::MINIMIZE_EDGES );
    //            VI nodeW, edgeW;
    //            auto sepGraph = getSeparatorGraph(sep,i, nodeW, edgeW);
    //            auto sp = gEdgeNodeMinim.minimizeSeparator( sep, sepGraph, nodeW, edgeW ); // adding minimized one as new separator
    //            separators.push_back(sp);
    ////            DEBUG(sp);
    //        }


        }

//    exit(1);



    }
    //***************** END OF SECTION WITH FAST MINIMIZER


    return separators;

}

vector<Separator> BFSSeparatorCreator::createInterlayerVCSeparators(VI sources) {
    VVI layers = BFS::getBfsLayers(V, sources);

//    DEBUG(layers);

    if( layers.size() == 1 ) return vector<Separator>();

    int L = layers.size();

    VI inLayer(V.size()); // inLayer[d] is index of layer in which node d is
    for( int i=0; i<L; i++ ){
        for(int d : layers[i]) inLayer[d] = i;
    }

    VVI vertexCovers;

    for( int i=1; i<L; i++ ){

        VPII interLayerEdges;
        for( int d : layers[i-1] ){
            for( int t : V[d] ){
                if( inLayer[t] == i ){
                    interLayerEdges.emplace_back(d,t);
                }
            }
        }

        InducedGraph gr = GraphInducer::induce(V,interLayerEdges);

        VB bipartition(gr.V.size(),true);
        for( int j=0; j<gr.nodes.size(); j++ ){
            if( inLayer[ gr.nodes[j] ] == i-1 ) bipartition[j] = false;
        }

        auto vc = BipartiteGraphVertexCover::getVertexCoverOfBipartiteGraph(gr.V,bipartition);
        for(int& d : vc) d = gr.nodes[d]; // remapping back to graph V

        vertexCovers.push_back(vc);
    }

//    DEBUG(vertexCovers);

//    exit(1);
//    ENDL(3);



    vector<Separator> res;
    for(auto v : vertexCovers){
        res.push_back( Separator(V,v) );
        res.back().createSeparatorStats(); // TEMPORARY, SO SLOW!!
    }




    return res;
}

VVI BFSSeparatorCreator::getRandomSources(int N, VI quantities) {
    VVI res;
    set<VI> sets;

    LL maxSets = 0;
    for(int i=1; i<quantities.size(); i++){
        if( maxSets < Constants::INF ){
            maxSets = Binom::binom(N,i);
            maxSets = min( 1ll*Constants::INF, maxSets );
        }
        quantities[i] = min( quantities[i], (int)maxSets );
    }

//    DEBUG(quantities);

    for(int q=1; q < min( (int)quantities.size(), (int)V.size()-1 ); q++){

        for( int i=0; i<quantities[q]; i++ ){
//            VI sources = getRandomSources(q);
            VI sources = CombinatoricUtils::getRandomSubset( N-1,q );

            sort(ALL(sources));
            if( sets.count( sources ) ){
                i--;
                continue;
            }

            sets.insert(sources);
            res.push_back(sources);
        }
    }

    return res;


}


