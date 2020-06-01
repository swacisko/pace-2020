//
// Created by sylwester on 8/7/19.
//

#include <graphs/VertexCover/approximation/SwapVC.h>
#include <graphs/VertexCover/approximation/SmallHallViolatorFinder.h>
#include <graphs/GraphUtils.h>
#include <graphs/matching/MaxMatchBipartite.h>
#include <graphs/GraphInducer.h>
#include <utils/TimeMeasurer.h>
#include <graphs/scc/StronglyConnectedComponents.h>
#include <graphs/toposort/TopoSort.h>
#include <graphs/GraphReader.h>
#include "graphs/VertexCover/SolutionVC.h"
#include "graphs/generators/GraphGenerator.h"
#include "graphs/VertexCover/kernelization/KernelizerVC.h"
#include "utils/RandomNumberGenerators.h"
//#include "../../GraphInducer.cpp"
//#include "../SolutionVC.cpp"





SwapVC::SwapVC(VVI &V) : StochasticApproximation(), candidatesChecked(0) {
    this->V = V;
//    nodeWeights = VD( V.size(),1 );
    findMinWeighedVC = false;
}

SwapVC::~SwapVC() {
    if( bestSolution != nullptr ) delete bestSolution;
    if( currentSolution != nullptr ) delete currentSolution;
}

Solution *SwapVC::getInitialSolution() {
   VI vc = VCUtils::getRandomMinimalVC(V);
//   VI bullsToChange = VCUtils::getVCGreedyMaxDegree(V);
//   VI bullsToChange = VCUtils::getVCGreedyMaxItarativeDegree(V);
   SolutionVC * sol = new SolutionVC( vc );
   newBestSolutionFound();


    if( !supressAllLogs ) cerr << "initial VC of size: " << sol->size() << endl;
//    exit(1);


   return sol;
}

void SwapVC::makeInitialWork() {
    nodeWeights = VD( V.size(),1 );
    lastTimeVisit = VI(V.size(),0);

    SolutionVC * sol = (SolutionVC*) currentSolution;
    X = sol->getVC();


    S = GraphUtils::getComplimentaryNodes( V, X );


//    if( GraphUtils::isMaximalIndependentSet( V, S ) == false ){
//        cerr << "S is not maximal!" << endl;
//        exit(1);
//    }else cerr << "S is maximal" << endl;


    sort( ALL(X) );
    sort( ALL(S) );

    if( !supressAllLogs ) cerr << "initial VC of size: " << X.size() << endl;

//   cerr << "S = " << S << endl << "X = " << X << endl;
}

void SwapVC::nextIteration() {

    if( params.useBothColoringAndPermutatingMis ){
        nextIterationWithColoring();
        nextIterationWithPermutingMis();
    }
    else if( params.useNextIterationPermutingMis ) {
        nextIterationWithPermutingMis();
    }else{
        nextIterationWithColoring();
    }


    if( params.useFluctuatingDegreesMaximization ){
        if( ( getIterationsDone() + 1 )  % max(  (int)( maxIterations / ( (LL)params.fluctuatingDegreesMaximizationFrequency + 1 ) ), 1 )  == 0 ){
            params.minimizeWeightOfMIS = !params.minimizeWeightOfMIS;
            cerr << endl << " SWAPPING DEGREE: MINIMIZATION: " << params.minimizeWeightOfMIS << endl;
        }
    }


    int vcWeight = 0;
    string vcWeightString = "";
    if( findMinWeighedVC ){
        for( int d : X ) vcWeight += nodeWeights[d];
        vcWeightString = "   weight(VC): " + to_string(vcWeight);
    }

    if( !supressAllLogs ) {
        cerr << (params.useNextIterationPermutingMis ? (params.useLocalIterations ? "Perm-mis-local" : "Perm-mis") : "Coloring")
             << "  candidates checked for Ix = " << candidatesChecked << "   deg(S) = " << getSumOfDegreesOfNodesInS()
             << "   VC.size() = " << X.size() << vcWeightString << endl;
    }



//    TimeMeasurer::startMeasurement("VC_check");
    if( VCUtils::isVertexCover( V,X ) == false ){ // checking whether it really is a vertex cover.
        cerr << "FAILURE!! X is not a vertex cover!" << endl;
        exit(1);
    }
//    TimeMeasurer::stopMeasurement("VC_check");
}

bool SwapVC::updateBestSolution(Solution *sol) {
    // CAUTION!! THE FRAGMENT *((SolutionVC*)bestSolution) < sol is ESSENTIAL TO USE operator < from SolutionVC !!!
    if( bestSolution == nullptr || ( *((SolutionVC*)bestSolution) < sol) || ( takeFirstBestSolution == false && *((SolutionVC*)bestSolution) == sol ) ){
        if( bestSolution != nullptr ) delete bestSolution;
        SolutionVC* s = (SolutionVC*) sol;
        bestSolution = new SolutionVC(*s);
        return true;
    }

    return false;
}

VI SwapVC::getRandomMISFromXWithNodes(VI &nodes) {
    return VCUtils::fillRandomToMaximalIS( V,nodes );
}

VI SwapVC::findSwapCandidate() {
    MaxMatchBipartite matcher;

    VB bipartition( SIxInduced.V.size(), false );
    for( int i=Ix.size(); i < SIxInduced.V.size(); i++ ) bipartition[i] = true; // now elements v that are in S have marked bipartition[v] = true.

    VI matching = matcher.getMaximumMatchingInBipartition(SIxInduced.V, bipartition, true);

//    VPII matchingEdges = matcher.convertToPairs( matching );
//    cerr << "Maximum matching found in SIx: " << matchingEdges << endl;

    VI hallViolator;
    if( findMinWeighedVC == false ) hallViolator = matcher.getMaximumHallViolator( SIxInduced.V, bipartition, matching );

    if( !hallViolator.empty() ){
        if( !supressAllLogs ) cerr << "------------->  HALL VIOLATOR found! |hallViolator| = " << hallViolator.size() << endl << endl;
//        cerr << "S:" << endl << hallViolator << endl;
//        VI neighborhood = GraphUtils::getNeighborhood( SIxInduced.V, hallViolator,true );
//        cerr << "N(S):" << endl << neighborhood << endl;

        return hallViolator;

    }else{
        VI nodeDegrees( SIxInduced.V.size() );
        for( int i=0; i<nodeDegrees.size(); i++ ){
            if( findMinWeighedVC ) nodeDegrees[i] = nodeWeights[ SIxInduced.nodes[i] ];
            else nodeDegrees[i] = V[ SIxInduced.nodes[i] ].size();
        }
        TimeMeasurer::startMeasurement("equalizer");
        VI equalizer = findHallEqualizer(SIxInduced.V, matching, bipartition, nodeDegrees, SIxInduced.nodes);
        TimeMeasurer::stopMeasurement("equalizer");

        if( !equalizer.empty() ){
            VI neighborhood = GraphUtils::getNeighborhood( SIxInduced.V, equalizer );
            if( !supressAllLogs ) cerr << "-----------------> Found EQUALIZER!  |equalizer| = " << equalizer.size() << endl << endl;

//            if( neighborhood.size() > equalizer.size() ){
//                cerr << "ERROR, equalizer = " << equalizer << endl << "neighborhood = " << neighborhood << endl;
//                for( int d : neighborhood ){
//                    cerr << "matching[" << d << "] = " << matching[d] << endl;
//                }
//                exit(1);
//            }

            return equalizer;
        }
    }


    return VI();
}


void SwapVC::nextIterationWithColoring() {

    TimeMeasurer::startMeasurement( "inducing graphs" );
    createInducedX();
    TimeMeasurer::stopMeasurement( "inducing graphs" );

    VI coloring = GraphUtils::getRandomColoring( XInduced.V );


    int D = *max_element( ALL(coloring) );
    VVI layers(D+1);
    for( int i=0; i<XInduced.V.size(); i++ ){
        layers[ coloring[i] ].push_back( i );
    }


    if( !supressAllLogs ) cerr << "Inducing done and layers created" << endl;

    for( int i=0; i<=D; i++ ){
        candidatesChecked++; // this is only for statistics to compare two nextIteration methods

        Ix = VCUtils::fillRandomToMaximalIS( XInduced.V, layers[i] );
        for( int & p : Ix ){
            p = XInduced.nodes[p];
        }


        TimeMeasurer::startMeasurement( "inducing graphs" );
        createInducedSIx();
        TimeMeasurer::stopMeasurement( "inducing graphs" );

        if( params.useHallEqualizerOnlyAfterIteration ){
            if( i < D ) params.useHallEqualizers = false;
            else params.useHallEqualizers = true;
        }

        VI swapCandidate = findSwapCandidate();
        if( !swapCandidate.empty() ){
            applySwapCandidate(swapCandidate, true, SIxInduced.V);
            break;
        }

    }

    if( currentSolution != nullptr ) delete currentSolution;
    currentSolution = new SolutionVC( X );


}


void SwapVC::createInducedSIx() {
    GraphInducer inducer;

    VI SIx = S;
    SIx.insert( SIx.begin(), ALL(Ix) );
    SIxInduced = inducer.induce( V, SIx );
}

void SwapVC::createInducedX() {
    GraphInducer inducer;
    XInduced = inducer.induce( V,X );
}

void SwapVC::applySwapCandidate(VI swapCandidate, bool idsInSIx, VVI &SIxStructure) {


//    cerr << "Before applying" << endl;
//    if( VCUtils::isVertexCover( V,X ) == false ){ // checking whether it really is a vertex cover.
//        cerr << "FAILURE!! X is not a vertex cover!" << endl;
//        exit(1);
//    }
//    if( !VCUtils::isIndependentSet( V,swapCandidate ) ){
//        ERROR(swapCandidate, "NOT A MIS!");
//    }

//    DEBUG(swapCandidate);
//    DEBUG(SIxStructure);
//    DEBUG(S);
//    DEBUG(X);
////    DEBUG(Ix);
//    ENDL(2);

//    for( int p : swapCandidate ){
//        if( SIxStructure[p].size() != GraphUtils::getNeighborhoodOfAInB( V, swapCandidate,S ).size() ){
//            DEBUG(p);
//            DEBUG( SIxStructure[p].size() );
//            DEBUG( GraphUtils::getNeighborhoodOfAInB( V, swapCandidate,S ).size() );
//            ERROR("", "SHOULD BE THE SAME!" );
//        }
//    }


    VI neighborhood;
    if( params.useSmallHallViolatorFinder ) neighborhood = GraphUtils::getNeighborhoodOfAInB( V, swapCandidate,S,false );
    else neighborhood = GraphUtils::getNeighborhood( SIxStructure, swapCandidate ); // this should be neighborhood of swapCandidate that belongs to S




    if( idsInSIx ){
        for(int & p : neighborhood) p = SIxInduced.nodes[p];
        for(int & p : swapCandidate) p = SIxInduced.nodes[p];
    }



    sort( ALL(swapCandidate) );
    sort( ALL(neighborhood) );



//    VI neigh2 = GraphUtils::getNeighborhoodOfAInB( V, swapCandidate,S,false );
//    sort( ALL(neigh2) );
//    VI sym_diff;
//    set_symmetric_difference( ALL(neighborhood), ALL(neigh2), back_inserter( sym_diff ) );
//    if( sym_diff.size() > 0 ){
//        DEBUG(neighborhood);
//        DEBUG(neigh2);
////        ERROR(sym_diff, "NEIGHBORHOODS SHOULD BE EQUAL!");
//    }

//    VI inters;
//    set_intersection( ALL(swapCandidate), ALL(neighborhood), inserter( inters, inters.begin() ) );
//    if( inters.size() > 0 ){
//        ERROR( inters, " INTERSECTION OF swapCandidate and neighborhood should be empty!" );
//    }


//    VI Sdiff(S.size());
//    Sdiff.resize( set_difference( ALL(S), ALL(neighborhood), Sdiff.begin() ) - Sdiff.begin() );
//
//    VI Xdiff(X.size());
//    Xdiff.resize( set_difference( ALL(X), ALL(swapCandidate), Xdiff.begin() ) - Xdiff.begin() );
//
//    S.resize( Sdiff.size() + swapCandidate.size() );
//    merge( ALL(Sdiff), ALL(swapCandidate), S.begin() );
//
//    X.resize( Xdiff.size() + neighborhood.size() );
//    merge( ALL(Xdiff), ALL(neighborhood), X.begin() );


    VI Sdiff;
    set_difference( ALL(S), ALL(neighborhood), inserter( Sdiff,Sdiff.begin() ) );

    VI Xdiff;
    set_difference( ALL(X), ALL(swapCandidate), inserter( Xdiff,Xdiff.begin() ) );

    S.clear();
    merge( ALL(Sdiff), ALL(swapCandidate), inserter( S, S.end() ) );

    X.clear();
    merge( ALL(Xdiff), ALL(neighborhood), inserter( X, X.begin() ) );




//    cerr << "After applying" << endl;
//
//    if( !VCUtils::isIndependentSet( V,S ) ){
//        ERROR(S, "NOT A MIS!");
//    }
//
//    if( VCUtils::isVertexCover( V,X ) == false ){ // checking whether it really is a vertex cover.
//        cerr << "FAIL AFTER APPLYING!!! X is not a vertex cover!" << endl;
//        exit(1);
//    }


//    DEBUG(swapCandidate);
//    DEBUG(SIxStructure);
//    DEBUG(S);
////    DEBUG(Ix);
//    DEBUG(X);
//    ENDL(2);
}

VI SwapVC::findHallEqualizer(VVI &structure, VI &matching, VB &bipartition, VI &nodeDegrees, VI mapper) {



    if( params.useHallEqualizers == false ) return VI();


    if( findMinWeighedVC && params.minimizeWeightOfMIS == false ){
        VB was(V.size(),false);
        VI mis = S;
        for( int d : mis ){
            was[d] = true;
            for(int p : V[d]) was[p] = true;
        }

        VI equalizer;
        for( int i=0; i<V.size(); i++ ){
            if( !was[i] ){
                equalizer.push_back(i);
                was[i] = true;
                for(int d : V[i]) was[d] = true;
            }
        }

        if( !equalizer.empty() ) return equalizer;

        if( GraphUtils::isMaximalIndependentSet( V, S ) == false ){
            cerr << "S is not maximal!" << endl;
            exit(1);
        }
    }




    VI removeNodes;
    if( findMinWeighedVC == false ) removeNodes = getNodesToRemoveFromEqualizerGraph(structure, bipartition, matching, mapper);
    VB toRemove(structure.size(),false);
    for(int d : removeNodes) toRemove[d] = true;


    VI nodesInNeighGraph;
    unordered_set<int> inS(ALL(S));
    for(int i=0; i<structure.size(); i++){

        if( bipartition[i] == false && toRemove[i] == false ){
            nodesInNeighGraph.push_back(i);
        }

        if( findMinWeighedVC && inS.count( ( mapper.empty() )? i : mapper[i] ) > 0 && matching[i] == -1 && structure[i].size() > 0 ) nodesInNeighGraph.push_back(i);

    }


    VVI neighGraph( structure.size() ); // this is a neigh graph that will be used to create strongly-connected-component graph

    for( int p : nodesInNeighGraph ){
        for( int d : structure[p] ){
            if( d != matching[p] ){
                if( findMinWeighedVC ){ // in MWVC we admit 'equalizers' with unmatched nodes from S.

                    if(matching[d] == -1){
                        neighGraph[p].push_back(d); // i add node d as the 'leaf'.
                    }
                    else neighGraph[p].push_back( matching[d] );
                }
                else neighGraph[p].push_back( matching[d] );

            }
        }
    }


//    DEBUG(neighGraph);
//    DEBUG(structure);
//    DEBUG(S);
//    DEBUG(matching);

    GraphInducer inducer;
    InducedGraph neighGraphInduced = inducer.induce( neighGraph, nodesInNeighGraph );


    StronglyConnectedComponents sccCreator( neighGraphInduced.V );
    VVI sccGraph = sccCreator.getStronglyConnectedComponentGraph();

//    DEBUG(sccGraph);

    VVI components = sccCreator.getComponents(); // components is the vector containing stronlgy connected components but with ids in structure
    for( VI & C : components ){
        for( int & p : C ) p = neighGraphInduced.nodes[p];
    }


    VD sccWeights = assignStronglyConnectedComponentWeights(components, nodeDegrees, matching);

    //******************************************  NOW CHOOSING SET EQUALIZER MAXIMIZING THE ASSIGNED WEIGHTS.
    TopoSort sorter( sccGraph );
    VI topologicalOrder = sorter.sortTopologically();





    double maxWeight = -(double)Constants::INF;
    double currentWeight = 0;
    int maxWeightIndex = -1;

    for( int i=topologicalOrder.size()-1; i>=0; i-- ){

        currentWeight += sccWeights[ topologicalOrder[i] ];
        if( currentWeight >= maxWeight ){
            maxWeight = currentWeight;
            maxWeightIndex = i;

        }
    }

    //****************************************** CREATING EQUALIZER - topoSort based method

    /**
     * This is just to test - some errors occur with equlizers...
     */
    bool equalizerFoundInTopoSortMethod = false;

    VI equalizer;
    VB inEqualizer( sccGraph.size(),false ); // This is used to admit maximum set of disjoint equalizers (usually from disjoint connected components of sccGraph). true if a component from sccGraph is in equalizer, false otherwise
    VI sccEqualizerComponents; // this is a vector that contains components of the sccGraph that are in the equalizer. It is used to clear inEqualizer array.
    bool admitDisjointEqualizers = false;

    /**
     * I admit only equalizers which reduce the sum of degrees or large equalizers that may help to get out of local extremums
     */

        if( ( findMinWeighedVC == false && topologicalOrder.size() > 0) || (findMinWeighedVC /*&& maxWeight > 0*/ ) ){

            equalizerFoundInTopoSortMethod = true;
            if( findMinWeighedVC == false ){
                if(params.useRandomEqualizerSwaps){
//                    maxWeightIndex = rand() % topologicalOrder.size();
                    maxWeightIndex = 0;
                    maxWeight = Constants::INF / 2;
                }else if( maxWeight <= 0 ){
                    maxWeightIndex = Constants::INF;
                    equalizerFoundInTopoSortMethod = false;
                }
            }else{
                if( maxWeight < 0 ){
                    maxWeightIndex = Constants::INF;
                    equalizerFoundInTopoSortMethod = false;
                }
            }

            for( int i=maxWeightIndex; i<topologicalOrder.size(); i++ ){
                int comp = topologicalOrder[i];

                inEqualizer[comp] = true;
                sccEqualizerComponents.push_back(comp);

                for( int d : components[comp] ){
                    equalizer.push_back(d);
                }
            }

        }



    //****************************************** CREATING EQUALIZER - all descendants method
    /**
     * This is just to test - some errors occur with equlizers...
     */
    bool equalizerFoundInAllDescendantsMethod = false;



    for( int i=topologicalOrder.size()-1; i>=0; i-- ){
        VB was( topologicalOrder.size(), false );
        VI neigh;

        bool isDisjoint = true;

        neigh.push_back( topologicalOrder[i] );

        currentWeight = 0;
        for( int j=0; j<neigh.size(); j++ ){
            int p = neigh[j];


            if( inEqualizer[p] ) isDisjoint = false;


            was[p] = true;
            currentWeight += sccWeights[p];

            for(int d : sccGraph[p]){
                if( !was[d] ){
                    neigh.push_back(d);
                    was[d] = true;
                }
            }
        }

        if( (currentWeight > maxWeight && currentWeight > 0) ){

            equalizerFoundInAllDescendantsMethod = true;
            maxWeight = currentWeight;

            for( int d : sccEqualizerComponents ) inEqualizer[d] = false;
            sccEqualizerComponents = neigh;

            equalizer.clear();
            for( int c : neigh ){

                inEqualizer[c] = true;

                equalizer.insert( equalizer.end(), ALL(components[c]) );
            }

        }else if( currentWeight > 0 && isDisjoint && admitDisjointEqualizers ){
            equalizerFoundInAllDescendantsMethod = true;
            maxWeight += currentWeight;

            for( int c : neigh ){
                inEqualizer[c] = true;
                equalizer.insert( equalizer.end(), ALL(components[c]) );
            }
            sccEqualizerComponents.insert( sccEqualizerComponents.end(), ALL(neigh) );

        }

    }

    if( findMinWeighedVC && params.useRandomEqualizerSwaps == false && maxWeight <= 0 ) equalizer.clear();


    if( findMinWeighedVC ){
        equalizer.resize( remove_if(ALL(equalizer), [&inS, &mapper](int d){ return inS.count( ( mapper.empty() )? d : mapper[d] ) > 0; } ) - equalizer.begin()  );
    }


    VI neighborhood = GraphUtils::getNeighborhood( structure, equalizer );

    VI equalizer2, neighborhood2;
    if( !mapper.empty() ){
        for(int i=0; i<equalizer.size(); i++) equalizer2.push_back( mapper[equalizer[i]] );
        for(int i=0; i<neighborhood2.size(); i++) neighborhood2.push_back( mapper[neighborhood[i]] );
    }
    if( getWeightOf(equalizer2) < getWeightOf(neighborhood2) ){
        ENDL(3);
        cerr << "V: " << endl << V << endl;
        cerr << "S: " << S << endl;
        cerr << "X: " << X << endl;
        cerr << "structure: " << endl << structure << endl;
        cerr << "nodeDegrees: " << nodeDegrees << endl;
        cerr << "equalizer: " << equalizer << endl;
        cerr << "neighborhood: " << neighborhood << endl;
        cerr << "equalizer weight: " << getWeightOf(equalizer) << endl;
        cerr << "neighborhood weight: " << getWeightOf(neighborhood) << endl;

        cerr << "nodesInNeighGraph: " << nodesInNeighGraph << endl;
        cerr << "neighGraph: " << neighGraph << endl;
        cerr << "matching: " << matching << endl;
        cerr << "sccGraph: " << endl << sccGraph << endl;
        cerr << "components: " << endl << components << endl;
        cerr << "topological order: " << topologicalOrder << endl;
        cerr << "sccWeights: " << sccWeights << endl;
        cerr << "maxWeight = " << maxWeight << endl;
        cerr << "maxWeightIndex = " << maxWeightIndex << endl;
        exit(1);
    }


    if( findMinWeighedVC == false && neighborhood.size() != equalizer.size() ){
//        cerr << "ERROR, equalizer = " << equalizer << endl << "neighborhood = " << neighborhood << endl;
        cerr << "ERROR!" << endl;
        DEBUG(equalizer.size());
        DEBUG(neighborhood.size());
        ENDL(1);
//        exit(1);
    }

    return equalizer;
}

VD SwapVC::assignStronglyConnectedComponentWeights(VVI &comps, VI &nodeDegrees, VI &matching) {

    VD compWeights(comps.size(),0);
    for(int i=0; i<comps.size(); i++){
        for(int d : comps[i]){
            double weight;

            if( findMinWeighedVC && matching[d] == -1 ) { // this can happen only if we consider MWVC
                weight = -nodeDegrees[d];
            }else{
                weight = nodeDegrees[d] - nodeDegrees[ matching[d] ];
            }

            if( params.minimizeWeightOfMIS ) weight = -weight;

                compWeights[i] += weight;
        }
    }
    return compWeights;
}


VI SwapVC::getNodesToRemoveFromEqualizerGraph(VVI &structure, VB &bipartition, VI &matching, VI &mapper) {
    VI neigh;
    VB was(structure.size(),false);


    VI nodesInS;
    if( mapper.empty() ) nodesInS = S; // this is the case when we use permutating mis technique
    else{
        unordered_set<int> zb(ALL(S));
        for( int i=0; i<structure.size(); i++ ){
            if( zb.count( mapper[i] ) ){
                nodesInS.push_back(i);
            }
        }
    }

    for( int d : nodesInS ){
        if( matching[d] == -1 ){
            neigh.push_back(d);
            was[d] = true;
        }
    }


    for(int i=0; i<neigh.size(); i++){
        int p = neigh[i];
        if( bipartition[p] == true ){

            for( int d : structure[p] ){
                if( !was[d] ){
                    neigh.push_back(d);
                    was[d] = true;
                }
            }

        }else{
            int d = matching[p];
            neigh.push_back(d);
            was[d] = true;
        }

    }

    VI nodesInX;
    if( mapper.empty() ) nodesInX = X; // this is the case when we use permutating mis technique
    else{
        unordered_set<int> zb(ALL(X));
        for( int i=0; i<structure.size(); i++ ){
            if( zb.count( mapper[i] ) ){
                nodesInX.push_back(i);
            }
        }
    }

    sort(ALL(neigh));
    VI nodesOnAlternatingPaths(neigh.size() + nodesInX.size());

    auto it = set_intersection( ALL(neigh), ALL(nodesInX), nodesOnAlternatingPaths.begin() );
    nodesOnAlternatingPaths.resize( it - nodesOnAlternatingPaths.begin() );


    return nodesOnAlternatingPaths;
}



//******************************************************************************* PERMUTING MIS
void SwapVC::nextIterationWithPermutingMis() {

    VI fillOrder = X;

    random_shuffle(ALL(fillOrder));
    VI score( V.size(),0 );

    // HERE I MAKE score[i] = vertex_cover_support[i]
    // in order to make Ix contain nodes with possibly lowest VCS values, i should make negative score or reverse the table fill order after sorting
    UniformIntGenerator gen(0, Constants::INF);
    for( int p : X ){
//        score[p] = gen.rand() % (  1 + iterationsDone - lastTimeVisit[p] );
//        score[p] = gen.rand() % ( (int)ceil( 1 + sqrt( iterationsDone - lastTimeVisit[p] ) ) );
//        score[p] = gen.rand() % ( (int)ceil( 1 + log ( iterationsDone - lastTimeVisit[p] ) ) );

        score[p] = iterationsDone - lastTimeVisit[p] + gen.rand() % (  1 + iterationsDone - lastTimeVisit[p] );

//        score[p] = iterationsDone - lastTimeVisit[p];
//        score[p] = sqrt( iterationsDone - lastTimeVisit[p] );
//        score[p] = log( iterationsDone - lastTimeVisit[p] );

//        score[p] -= V[p].size();
        for( int d : V[p] ) score[p] += V[d].size();
    }

    sort( ALL(fillOrder),  [&score](int a, int b){ return score[a] > score[b]; } ); // filling nodes, starting with that of greatest score.
//    reverse( ALL(fillOrder) );

//    if( !supressAllLogs ){
//        for( int i=0; i<fillOrder.size(); i++ ) cerr << score[ fillOrder[i] ] << " "; cerr << endl;
//    }

    Ix = VCUtils::fillRandomToMaximalIS( V,fillOrder, VI() );

    for( int p : Ix ) lastTimeVisit[p] = iterationsDone;

    VI removedNodes, addedNodes; // nodes that are removed or added in each swap operation
    VB bipartition( V.size(), true );
    for(int p : Ix) bipartition[p] = false;


    VVI SIxStructure( V.size() ); // this is just vector V, but only with edges in the bipartite graph (S,Ix)
    VB inS( V.size(), false );
    for(int d : S) inS[d] = true;
    for( int p : Ix ){
        for(int d : V[p]){
            if( inS[d] ){
                SIxStructure[p].push_back(d);
                SIxStructure[d].push_back(p);
            }
        }
    }

    VI degreeInIx( V.size(),0 ); // degreeInIx[v] is the number of neighbors of v in the set Ix (in the original graph);
    for( int p : Ix ){
        for( int d : V[p] ){
            if( !inS[d] ) degreeInIx[d]++;
        }
    }

    MaxMatchBipartite matcher;
    VI matching; // = matcher.getMaximumMatchingInBipartition(SIxStructure, bipartition, true);

    VB inIx(V.size(),false);
    for(int d : Ix) inIx[d] = true;

    candidatesChecked++; // this is only for statistics to compare two nextIteration methods


//    DEBUG(Ix);
//    DEBUG(SIxStructure);

    //********************* HERE I CHECK WHETHER THERE IS A HALL VIOLATOR in the first found randomly set Ix. If so, then i apply it and do not proceed to swapping
    VI swapCandidate;
    if( params.useSmallHallViolatorFinder && (iterationsDone & 1) ){
        SmallHallViolatorFinder finder( V,S );
//        bool useEqualizers = true;
        swapCandidate = finder.findSwapCandidate( params.smallHallViolatorMaxDegree, params.useSmallHallViolatorEqualizers );

//        cerr << endl << "RETURNED!!" << endl;

        if( !swapCandidate.empty() && !supressAllLogs ) cerr << "FOUND SMALL HALL VIOLATOR / EQUALIZER of size " << swapCandidate.size() << endl;

        if( !VCUtils::isIndependentSet( V,swapCandidate ) ){
            ERROR( swapCandidate, "NOT A MIS IN V!" );
        }
//        DEBUG( swapCandidate );
//        ERROR(swapCandidate, "swap candidate in SwapVC found");
    }else{
        matching = matcher.getMaximumMatchingInBipartition(SIxStructure, bipartition, true);
        swapCandidate = findSwapCandidatePermutingMIS(SIxStructure, matcher, matching, bipartition, addedNodes);
    }


    VI perm = X; // set of elements in X in random order
    random_shuffle(ALL(perm));

    if( !swapCandidate.empty() ){
        Ix.clear();
        for( int i=0; i<V.size(); i++ ){
            if( inIx[i] ) Ix.push_back(i);
        }

        applySwapCandidate(swapCandidate, false, SIxStructure);

        if( params.useLocalIterations == false ){
            if( currentSolution != nullptr ) delete currentSolution;
            currentSolution = new SolutionVC( X );

            return;
        }else{

            modifyDataAfterApplyingSwapCandidate( SIxStructure,swapCandidate,inIx,inS,degreeInIx,matching,bipartition );
            perm = X;
            random_shuffle(ALL(perm));

        }
    }

    if( params.usePermMISIterations == false ) return;

    int localIterations = 0; // this is to count iterations - since in permuting mis iterations are done locally, i need another counter


    //******************** NOW WE ARE READY FOR ITERATIONS!!! Caution - Ix will not be modified, because every erase in a vector takes linear time, would be to expensive...
    for( int i=0; i<perm.size(); i++ ){
        int p = perm[i];

        cerr << "\r\t\t" << ((double)100*i / perm.size()) << "%   |VC| = " << X.size() << endl;
        if( !supressAllLogs ) cerr << "\r\t\t" << ((double)100*i / perm.size()) << "%   |VC| = " << X.size() << flush;

        if(runTimeExceeded()) break;

        if( params.useHallEqualizerOnlyAfterIteration && i >= 0.75 * (perm.size()-1) ) params.useHallEqualizers = true;
        else if( params.useHallEqualizerOnlyAfterIteration ) params.useHallEqualizers = false;

        if( params.useLocalIterations == false ){
            if( inIx[p] ) continue;
        }else{

            if( inIx[p] ){
                if( startNextLocalIteration( perm,i,localIterations ) ) continue;
                else break;
            }
        }



        candidatesChecked++; // this is only for statistics to compare two nextIteration methods
        addedNodes.clear();


        //  FIRST ADD p TO SIxStructure, and remove its neighbors
        removedNodes = moveNodeToSIxStructure(SIxStructure, p, degreeInIx, inS, inIx);


        bipartition[p] = false;
        for( int d : removedNodes ){
            bipartition[d] = true; // this here should not make any difference (since removed nodes have no neighbors), but to be on the safe side, simulate to put it into S

            for( int dd : V[d] ){
                if( !inS[dd] ) degreeInIx[dd]--;
            }
        }

        for( int d : V[p] ){
            if( !inS[d] ) degreeInIx[d]++;
        }


        fillIxToMIS( SIxStructure,removedNodes,addedNodes,matching,degreeInIx,inS,inIx,bipartition,p );


        // NOW Ix should be a maximal independent set (though the structure in vector Ix is not changed, it is in inIx stored).

        VI swapCandidate = findSwapCandidatePermutingMIS(SIxStructure, matcher, matching, bipartition, addedNodes);


        if( !swapCandidate.empty() ){
            Ix.clear();
            for( int j=0; j<V.size(); j++ ){
                if( inIx[j] ) Ix.push_back(j);
            }

            applySwapCandidate(swapCandidate, false, SIxStructure);



            if( params.useLocalIterations == false ){
                break;
            }else {

                  modifyDataAfterApplyingSwapCandidate( SIxStructure,swapCandidate,inIx,inS,degreeInIx,matching,bipartition );
                  perm = X;
                  random_shuffle(ALL(perm));

                  if( startNextLocalIteration( perm,i,localIterations ) ) continue;
                  else break;


            }

        }

        if( params.useLocalIterations ) {

            if( startNextLocalIteration( perm,i,localIterations ) ) continue;
            else break;

        }



    }

    if( currentSolution != nullptr ) delete currentSolution;
    currentSolution = new SolutionVC( X );

}

bool SwapVC::startNextLocalIteration(VI &perm, int &i, int &localIterations) {
    if( i == perm.size()-1 ){
        if( !supressAllLogs ) {
            cerr << "   local iterations = " << localIterations << "   perm-mis-local   perm.size() = " << perm.size() << "   candidates checked: "
                 << candidatesChecked << "   deg(S): " << getSumOfDegreesOfNodesInS() << "   VC.size(): " << X.size() << endl;
        }
        random_shuffle(ALL(perm));
        i = -1;
        localIterations++;
        if( localIterations >= getMaxIterations() || runTimeExceeded() ){
            setMaxIterations(1);
            return false;
        }
        return true;
    }

    return true;
}


VI SwapVC::findSwapCandidatePermutingMIS(VVI &SIxStructure, MaxMatchBipartite &matcher, VI &matching, VB &bipartition, VI &addedNodes) {
    VI wasPathHelper(V.size(),0); // helper for finding augmenting paths
    int currentWas = 1; // helper for finding augmenting paths


    for( int v : addedNodes ){
        VI augmentingPath;
        if( matching[v] == -1 && matcher.findAugmentingPath( SIxStructure, v, bipartition,matching,augmentingPath,wasPathHelper,currentWas ) ){
            matcher.applyAugmentingPath( matching, augmentingPath );


            currentWas++;
        }
    }

    // this here is asymptotically faster, but for just a few nodes added it is probably slower than the dfs version
    /*VVI augmentingPaths;
    while( true ){
        augmentingPaths = matcher.getMaximalSetOfDisjointAugmentingPaths( SIxStructure,bipartition, matching );
        if( augmentingPaths.empty() ) break;
        else{
            for( VI & path : augmentingPaths ) matcher.applyAugmentingPath( matching, path );
        }
    }*/




    VI hallViolator;
    if( findMinWeighedVC == false ) hallViolator  = matcher.getMaximumHallViolator( SIxStructure, bipartition, matching );


    if( !hallViolator.empty() ){

        VI neighborhood = GraphUtils::getNeighborhood( SIxStructure, hallViolator );

        if( !supressAllLogs ) cerr << "-------------->  HALL VIOLATOR found! |hallViolator| = " << hallViolator.size() << endl << endl;

        return hallViolator;

    }else{
        VI nodeDegrees( SIxStructure.size() );
        for( int i=0; i<SIxStructure.size(); i++ ){
            if( findMinWeighedVC ) nodeDegrees[i] = nodeWeights[i];
            else nodeDegrees[i] = V[i].size();
        }
        TimeMeasurer::startMeasurement("equalizer");
        VI equalizer = findHallEqualizer(SIxStructure, matching, bipartition, nodeDegrees );
        TimeMeasurer::stopMeasurement("equalizer");
        if( !equalizer.empty() ){
            VI neighborhood = GraphUtils::getNeighborhood( SIxStructure, equalizer );
            if( !supressAllLogs ) cerr << "-----------------> Found EQUALIZER!  |equalizer| = " << equalizer.size() << endl << endl;
//            DEBUG(SIxStructure);
//            DEBUG(matching);
//            DEBUG(equalizer);


            return equalizer;
        }
    }


    return VI();
}

VI SwapVC::moveNodeToSIxStructure(VVI &SIxStructure, int v, VI &degreeInIx, VB &inS, VB &inIx) {
    VI removedNodes;

    for( int d : V[v] ){
        if( inIx[d] ){ // if d is in Ix, then i must remove it
            removedNodes.push_back(d);
            inIx[d] = false;

            for( int dd : SIxStructure[d] ){
                for( int i=0; i<SIxStructure[dd].size(); i++ ){
                    if( SIxStructure[dd][i] == d ){
                        swap( SIxStructure[dd][i], SIxStructure[dd].back() );
                        SIxStructure[dd].pop_back();
                        break;
                    }
                }
            }


            SIxStructure[d].clear();

        }
    }

    for( int d : V[v] ){
        if( inS[d] ){ // i must add to SIxStructure all neighbors of v that are in S
            SIxStructure[v].push_back(d);
            SIxStructure[d].push_back(v);
        }
    }

    inIx[v] = true;

    return removedNodes;
}

void SwapVC::modifyDataAfterApplyingSwapCandidate(VVI &SIxStructure, VI &swapCandidate, VB &inIx, VB &inS, VI &degreeInIx, VI &matching, VB &bipartition) {

    TimeMeasurer::startMeasurement( "modify_data_perm_mis_local" );

    // here sets S and X are already changed (in applySwapCandidate)
    // i need to change rest of the data

    //  RESET MATCHING DATA. I reset all nodes that were matched in swapCandidate
    for( int d : swapCandidate ){
        if( matching[d] != -1 ){
            matching[ matching[d] ] = -1;
            matching[d] = -1;
        }
    }

    //  CHANGING inS and inIx - adding swapCandidate (T) to S, removing N(T) from S and removing T from Ix
    for( int i=0; i<inS.size(); i++ ) inS[i] = false;
    for(int d : S){
        inS[d] = true;
        inIx[d] = false;
    }


    //  CHANGING degreeInIx values for neighbors of removed nodes - here removed are all from swapCandidate
    VI removedNodes = swapCandidate;
    for( int d : removedNodes ){
        bipartition[d] = true; // this here should not make any difference (since removed nodes have no neighbors), but to be on the safe side, simulate to put it into S

        for( int dd : V[d] ){
            if( !inS[dd] ) degreeInIx[dd]--;
        }
    }


    // CHANGING SIxStructure
    VI neighborhoodT = GraphUtils::getNeighborhood( SIxStructure, swapCandidate ); // this is the neighborhood of T in S, where T is just swapCandidate (but shorter name).   N(T) \cup S
    VI neighborhoodNT = GraphUtils::getNeighborhood( SIxStructure, neighborhoodT ); // and this is N( N(T) \cup S )


    // I FILL Ix to random MIS
    VI addedNodes;
    fillIxToMIS( SIxStructure, removedNodes, addedNodes,matching,degreeInIx,inS,inIx,bipartition );


    // FINALLY I EXTEND matching to the matching saturating Ix.
    MaxMatchBipartite matcher;
    VI swpCand = findSwapCandidatePermutingMIS( SIxStructure, matcher,matching,bipartition,addedNodes );

    TimeMeasurer::stopMeasurement( "modify_data_perm_mis_local" );

    candidatesChecked++; // this is only for statistics to compare two nextIteration methods
    if( !swpCand.empty() ){ // If after change there is yet another swap candidate, then i change it again.
        applySwapCandidate(swpCand, false, SIxStructure);
        modifyDataAfterApplyingSwapCandidate( SIxStructure,swpCand,inIx,inS,degreeInIx,matching,bipartition );
    }
}

void SwapVC::fillIxToMIS(VVI &SIxStructure, VI &removedNodes, VI &addedNodes, VI &matching, VI &degreeInIx, VB &inS, VB &inIx, VB &bipartition, int pivotNode) {

    VI fillToMisCandidates; // the only nodes that can be added to Ix now are in the set N( N(V) ) \ N[v] and must have degreeInIx equal to 0.
    for( int d : removedNodes ){
        for( int dd : V[d] ){
            if( dd != pivotNode && !inS[dd] && degreeInIx[dd] == 0 ) fillToMisCandidates.push_back(dd);
        }

        if( matching[d] != -1 ){ // this must be checked, because the first set Ix can contain hall violator, but is not considered until after the first swap. If such, it would have matching[d] == -1 and cause SIGSEGV
            matching[ matching[d] ] = -1; // resetting matchings of removed nodes and their matched partners in S
            matching[d] = -1;
        }
    }

    if( !fillToMisCandidates.empty() ){
        sort( ALL(fillToMisCandidates) );
        fillToMisCandidates.resize( unique( ALL(fillToMisCandidates) ) - fillToMisCandidates.begin() );
    }


    for( int d : fillToMisCandidates ){ // filling to MIS
        if( d == pivotNode || degreeInIx[d] != 0 ) continue; // this is just to check each edge once instead of twice

        bool canBeAdded = true;
        for( int dd : V[d] ){
            if( inIx[dd] ){
                canBeAdded = false;
                break;
            }
        }
        if(canBeAdded){ // here i add d to Ix
            inIx[d] = true;
            bipartition[d] = false;
            for( int dd : V[d] ){
                if( !inS[dd] ) degreeInIx[dd]++;
                if( inS[dd] ){ // i add d to SIxStructure, so i must add each edge (d,dd) with dd in S
                    SIxStructure[d].push_back(dd);
                    SIxStructure[dd].push_back(d);
                }
            }
            addedNodes.push_back(d);
        }
    }

    if( pivotNode != -1 ) addedNodes.push_back(pivotNode);
}

int SwapVC::getSumOfDegreesOfNodesInS() {
    int res = 0;
    for(int d : S) res += V[d].size();
    return res;
}




void SwapVC::run() {
    InducedGraph kernelizedV;
    pair< VI, VPII > kernel;
    VVI oldV;

    if( params.useKernelization ){
        KernelizerVC kernelizer;
        GraphInducer inducer;
        oldV = V;


        kernel = kernelizer.initialKernelization(V);

        if( !supressAllLogs ) cerr << "Found kernel of size " << kernel.first.size() << " out of altogether " << V.size() << " nodes " << flush;



//        cerr << "Found kernel of size " << kernel.first.size() << " out of altogether " << V.size() << " nodes " << flush;

//        for( int d : kernel.first ) GraphUtils::removeNodeFromGraph( V,d );

//        cerr << "\tThere are " << GraphUtils::countNodesWithDegree(V,1,V.size()) << " nonisolated vertices after kernelization" << endl;

        VI nodes;
        for( int i=0; i<V.size(); i++ ){
            if( V[i].size() >= 1 ) nodes.push_back(i);
        }

        if( !supressAllLogs ) cerr << "    Subgraph is now induced by " << nodes.size() << " nodes" << endl;

//        DEBUG(nodes.size());
        if( nodes.empty() ){
            currentSolution = new SolutionVC( kernel.first );
            newBestSolutionFound();
            updateBestSolution(currentSolution);
            V = oldV;
            return;
        }



//        DEBUG(V.size());
//        DEBUG(nodes.size());
        random_shuffle(ALL(nodes));
        kernelizedV = inducer.induce(V,nodes);



//        DEBUG(kernelizedV.perm.size());
//        DEBUG(kernelizedV.V);

//        if( currentSolution != nullptr ){
//            delete currentSolution;
//            currentSolution = nullptr;
//        }

        if( currentSolution != nullptr ){
            VI vc =  ( (SolutionVC*)currentSolution )->getVC();

            sort(ALL(vc));
            sort(ALL(kernel.first));

//            DEBUG(bullsToChange.size());
//            DEBUG(bullsToChange);

            VI vc2;

//            DEBUG(vc2.size());
//            DEBUG(vc2);

            for( int d : vc ){
                if( kernelizedV.perm.find(d) != kernelizedV.perm.end() ) vc2.push_back( kernelizedV.perm[d] );
            }

//            DEBUG(vc2.size());
//            DEBUG(vc2);

//            DEBUG(kernelizedV.perm);
//            DEBUG(kernelizedV.perm.size());

            delete currentSolution;
            currentSolution = new SolutionVC(vc2);
            updateBestSolution(currentSolution);
        }

//        DEBUG(kernelizedV.V);
        V = kernelizedV.V;
    }

//    if( !supressAllLogs )  cerr << "BEFORE RUN" << endl << V << endl;
//    setV(V);
    if( !V.empty() ) StochasticApproximation::run();

//    if( !supressAllLogs )  cerr << "AFTER RUN" << endl;

    if( params.useKernelization ){
        V = oldV;

        VI vc = ( (SolutionVC*) bestSolution)->getVC();
        for( int & d : vc ) d = kernelizedV.nodes[d];
        vc.insert( vc.end(), ALL(kernel.first) );
        sort(ALL(vc));

        delete bestSolution;
        bestSolution = new SolutionVC(vc);
    }
}


















void SwapVC::test() {

    bool automaticParameterTest = false;

    VVI V;
    SwapVC swapvc(V);

    // ********************  THIS IS JUST AUTOMATIC TESTING OF SwapVCParams PARAMETERS
    for( int j=0; j<=1; j++ ){
        if( automaticParameterTest ) swapvc.getParams().useHallEqualizers = j;

        for( int l=0; l<=1; l++ ){
            if( automaticParameterTest ){
                swapvc.getParams().useHallEqualizerOnlyAfterIteration = l;
                if( !swapvc.getParams().useHallEqualizers && swapvc.getParams().useHallEqualizerOnlyAfterIteration ) continue;
            }

            for( int i=0; i<=1; i++ ){
                if( automaticParameterTest ) swapvc.getParams().useNextIterationPermutingMis = i;

                for(int k=0; k<=1; k++){


                    for( int t=0; t<=1; t++ ){
                        if( automaticParameterTest )  swapvc.getParams().minimizeWeightOfMIS = t;

        //*********************  THIS IS JUST AUTOMATIC TESTING OF SwapVCParams PARAMETERS

                        swapvc.getParams().useSmallHallViolatorFinder = true;

                        string testName;

                        bool useRandomGraph = false;
                        if(useRandomGraph){
//                            if( V.empty() ) V = GraphGenerator::getRandomGraph( 30,70   );
//                            if( V.empty() ) V = GraphGenerator::getRandomGraph( 4'000, 80'000   ); //  (4'000,80'000) --> |VC*| <= 3466
//                            if( V.empty() ) V = GraphGenerator::getRandomGraph( 4'000, (double)0.1   );
//                            if( V.empty() ) V = GraphGenerator::getRandomGrid( 51,20 );
//                            if( V.empty() ) V = GraphGenerator::getRandomBipartiteGraph( 100, 50, 0.42 );

                                    double N = 4000;
                                    double AVG_DEG = 20;
                                    if( V.empty() ) V = GraphGenerator::getRandomGraph( (int)4'000, AVG_DEG / N  );
                                    double EXPECTED_INDEPENDENCE = 2*N / AVG_DEG * ( log( AVG_DEG ) - log( log(AVG_DEG) ) + 1 );
                                    DEBUG(EXPECTED_INDEPENDENCE);

//                            DEBUG(V);
                        }else{
                            V = GraphReader::readGraphDIMACSWunweighed(cin);
//                            V = GraphReader::readGraphStandardEdges();
                        }

                        int N = V.size();
                        int M = 0; for(auto v : V) M += v.size();
                        M >>= 1;

                        bool findMinWeighedVC = false;
                        VI weights(V.size(),1);

                        if(useRandomGraph){
    //                        cerr << "Random graph with N = " << V.size() << " and M = " << M/2 << endl;
                            testName = "random_" + to_string(N) + "-" + to_string(M);
                            for( int i=0; i<V.size(); i++ ) weights[i] = (i+1)%200;
                        }else{
                            testName = "input_" + to_string(N) + "-" + to_string(M);
                        }

                        cerr << endl << testName << endl;

//                            for( int i=0; i<V.size(); i++ ) weights[i] = (i+1)%200;

                        bool useKernelization = true;
                        KernelizerVC kernelizer;
                        pair< VI, VPII > kernel;
                        GraphInducer inducer;
                        InducedGraph kernelizedV;
                        if(useKernelization){
                            cerr << "before removing kernel, graph has " << GraphUtils::countNodesWithDegree(V,1,V.size()) << " nonisolated nodes" << endl;

                            kernel = kernelizer.initialKernelization(V);
//                            kernel = kernelizer.lpDecomposition(V);

                            cerr << "found kernel of size " << kernel.first.size() << endl;
                            for( int d : kernel.first ) GraphUtils::removeNodeFromGraph(V, d);
                            cerr << "after removing kernel, graph has " << GraphUtils::countNodesWithDegree(V,1,V.size()) << " nonisolated nodes" << endl;
//                            exit(1);
                            VI nodes;
                            for( int i=0; i<V.size(); i++ ){
                                if( V[i].size() >= 1 ) nodes.push_back(i);
                            }
                            kernelizedV = inducer.induce(V,nodes);
                            V = kernelizedV.V;
                        }

                        swapvc.setV(V);
                        if( findMinWeighedVC ){
                            swapvc.setFindMinWeighedVC( true,weights );
                        }

//                            long long ITERATIONS = V.size() * 20;
//                            swapvc.setMaxIterations(ITERATIONS);

                        int SECONDS = 300;
                        swapvc.setMaxRunTime(SECONDS * 1000);

                        swapvc.run();



                        if( automaticParameterTest ) swapvc.getParams().useHallEqualizers = j; // this needs to be here, because perm-mis-local modifies it, and can leave with other state.

                        cerr << "Time spent: " << ((double)swapvc.getExecutionTime() / 1000) << endl;
                        cerr << "Best solution discovered after " << ((double)swapvc.getBestSolutionDiscoveryTime() / 1000) << endl;
                        SolutionVC *s = (SolutionVC*) swapvc.getBestSolution();
                        cerr << "vertex cover of size " << s->size() << endl;

                        cerr << "inducing graphs time: " << TimeMeasurer::getMeasurementTimeInSeconds( "inducing graphs" ) << endl;
                        cerr << "looking for equalizers time: " << TimeMeasurer::getMeasurementTimeInSeconds( "equalizer" ) << endl;
                        cerr << "Checking if VC is correct time: " << TimeMeasurer::getMeasurementTimeInSeconds( "VC_check" ) << endl;


                        bool writeResultsInCsv = false;

                        if( writeResultsInCsv ){
                            std::ofstream myfile;
                            myfile.open ( testName + ".csv", std::ios_base::app        );
                            myfile << endl;
                            myfile << testName << endl;
                            myfile << "N," << V.size() << endl;
                            myfile << "M," << M << endl;
                            myfile << "resVC," << s->size() << endl;
                            myfile << "candidates," << swapvc.candidatesChecked << endl;
                            myfile << "deg(S)," << swapvc.getSumOfDegreesOfNodesInS() << endl;
                            myfile << "time," << ((double)swapvc.getExecutionTime() / 1000) << endl;
                            myfile << "best solution discovery," << ((double)swapvc.getBestSolutionDiscoveryTime() / 1000) << endl;
                            myfile << "swapVC variant," << ( swapvc.getParams().useNextIterationPermutingMis ? ( swapvc.getParams().useLocalIterations ? "permuting_mis_local" : "permuting_mis" ) : "coloring" ) << endl;
                            myfile << "use equalizers," << to_string(swapvc.getParams().useHallEqualizers) << endl;
                            if( swapvc.getParams().useHallEqualizers ){
                                myfile << "minimize degree of S," << swapvc.getParams().minimizeWeightOfMIS << endl;
                                myfile << "use hall equalizer only after iteration," << swapvc.getParams().useHallEqualizerOnlyAfterIteration << endl;
                            }
                            myfile << "inducing graphs time," << TimeMeasurer::getMeasurementTimeInSeconds( "inducing graphs" ) << endl;
                            myfile << "equalizer search time," << TimeMeasurer::getMeasurementTimeInSeconds( "equalizer" ) << endl;
                            myfile.close();


                        }else{

                                VI vc = s->getVC();
                                if( useKernelization ){
                                    for( int & d : vc ) d = kernelizedV.nodes[d];
                                    vc.insert( vc.end(), ALL(kernel.first) );
                                    sort(ALL(vc));
                                }
                                cerr << "s S " << N << " " << vc.size() << endl;
                                cout << "s S " << N << " " << vc.size() << endl;


//                                for( int p : bullsToChange ) cout << p+1 << endl;

                        }

                        TimeMeasurer::resetOption("inducing graphs");
                        TimeMeasurer::resetOption("equalizer");
                        TimeMeasurer::resetOption("VC_check");

    //*********************  THIS IS JUST AUTOMATIC TESTING OF SwapVCParams PARAMETERS
                        if( automaticParameterTest == false ) break;
                        else if( swapvc.getParams().useHallEqualizers == false ) break;
                    }

                    if( automaticParameterTest ){
                        if(swapvc.getParams().useNextIterationPermutingMis) swapvc.getParams().useLocalIterations = !swapvc.getParams().useLocalIterations;
                        else break;
                    }else break;
                }

                if( automaticParameterTest == false ) break;
            }

            if( automaticParameterTest == false ) break;
        }

        if( automaticParameterTest == false ) break;
    }
    //*********************  THIS IS JUST AUTOMATIC TESTING OF SwapVCParams PARAMETERS


}



