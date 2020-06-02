/*
 * Copyright (c) 2020, Sylwester Swat
*/
#include <graphs/VertexCover/approximation/PartitionSVC.h>
//#include <graphs/VertexCover/approximation/NuMVC/numvc.h>
#include <graphs/VertexCover/approximation/SwapVC.h>
#include <graphs/VertexCover/SolutionVC.h>
#include <combinatorics/CombinatoricUtils.h>
#include <utils/RandomNumberGenerators.h>
#include <graphs/generators/GraphGenerator.h>
#include <graphs/GraphReader.h>
#include <graphs/GraphUtils.h>
#include <graphs/VertexCover/kernelization/KernelizerVC.h>
#include <datastructures/FAU.h>
#include <utils/TimeMeasurer.h>


PartitionSVC::PartitionSVC(VVI &V) : SwapVC(V) {
}


Solution *PartitionSVC::getInitialSolution() {


    SwapVC swapvc(V);
    swapvc.setMaxIterations( svcParams.initialSolutionIterations );
    swapvc.getParams().useSmallHallViolatorFinder = false;
    swapvc.setSupressAllLogs(true);
    swapvc.run();
    return new SolutionVC( ((SolutionVC*) swapvc.getBestSolution())->getVC() );
}

void PartitionSVC::makeInitialWork() {
    X = ((SolutionVC*)currentSolution)->getVC();
    S = CombinatoricUtils::getFullSetDifference( V.size(),X );

    sort(ALL(X));
    sort(ALL(S));

}

void PartitionSVC::nextIteration() {


    VI S1 = getRandomSetS1();
    VI W = getSetWForS1(S1);




//     just statistics.
//    ENDL(3);
//    VI tempDeg(V.size(),0);
//    for(int d : W){
//        VI tempW = VI(1,d);
//        tempDeg[ GraphUtils::getNeighborhoodOfAInB( V,tempW,S1 ).size() ]++;
//    }
////
//    for( int i=0; i<12; i++ ){
//        cerr << "There are " << tempDeg[i] << " nodes in W with degree " << i << " in S1" << endl;
//    }


    VI nodes = S1;
    nodes.insert( nodes.end(), ALL(W) );


    InducedGraph GW = GraphInducer::induce( V,nodes );


//    ENDL(5);
//    for( int i=0; i<40; i++ ){
//        cerr << "Nodes in GW.V with deg " << i << ":  " << GraphUtils::countNodesWithDegree(GW.V,i,i) << endl;
//    }
//    cerr << "Nodes in GW.V: " << GW.V.size() << "   and with deg from 0 to GW.V.size() " << GraphUtils::countNodesWithDegree(GW.V,0,GW.V.size()) << endl;
//    ENDL(1);


    /*if( (1+iterationsDone) % svcParams.localOptimumViolationFrequency == 0 ){
        forceGettingOutOfLocalOptimum();

        if( currentSolution != nullptr ) delete currentSolution;
        currentSolution = new SolutionVC( X );

        return;
    }*/



    bool use_exact_solver = false;
    if( (1+iterationsDone) % svcParams.localOptimumViolationFrequency == 0 ) use_exact_solver = false;

    VI vcW;

   {

        SwapVC swapvc( GW.V );

        if( svcParams.setInitialSolutionForSubgraph ){
            VI initSol = W;
            for(int & p : initSol) p = GW.perm[p];
            swapvc.setInitialSolution( new SolutionVC( initSol ) );
        }

        swapvc.setMaxIterations( svcParams.iterationsPerSubgraph );
        swapvc.setMaxRunTime( min( svcParams.maxRunTimeForSubgraph,  maxRunTime - getTimeFromBegin() ) );


        swapvc.getParams().useKernelization = true;
        if( (1+iterationsDone) % svcParams.localOptimumViolationFrequency == 0 ) swapvc.getParams().useKernelization = false;

        swapvc.setTakeFirstBestSolution(false);
        swapvc.setSupressAllLogs(true);

        swapvc.getParams().useSmallHallViolatorFinder = true;
        swapvc.getParams().smallHallViolatorMaxDegree = 1;
        swapvc.getParams().useSmallHallViolatorEqualizers = false;

        swapvc.run();
        vcW = ( (SolutionVC*) swapvc.getBestSolution()  )->getVC();
    }


    for( int & p : vcW ) p = GW.nodes[p];


//    DEBUG(W.size());
//    DEBUG(vcW.size());


    VI swapCandidate;
    sort(ALL(W));
    sort(ALL(vcW));
    set_difference( ALL(W), ALL(vcW), back_inserter(swapCandidate) );


    int sizeDiff = (int) vcW.size() - (int)W.size();
    int maxDeviation = svcParams.maxDeviationFromBestResult;

    bool makeSwap = true;
    if( currentSolution != nullptr && bestSolution != nullptr ){
        makeSwap = (     (int)( (SolutionVC*) currentSolution )->getVC().size() + sizeDiff <= (int)( (SolutionVC*) bestSolution )->getVC().size() + maxDeviation  );
    }

    if(!makeSwap) ERROR("makeSwap", "make swap = false");

    if( makeSwap ){
        if( vcW.size() > W.size() ){
            if( !supressAllLogs ){
                cerr << "getting out of local minimum" << endl;
            }
        }

        if( !supressAllLogs ) cerr << "   found HALL VIOLATOR / EQUALIZER of size " << swapCandidate.size() << endl;
        params.useSmallHallViolatorFinder = true; // this must be set to true to get CORRECT NEIGHBORHHOD in applySwapCandidate.
        VVI anySet;
        applySwapCandidate( swapCandidate, false, anySet ); // i will get neighborhood based on V, so i can apss any set here, it wont be used.

        if( currentSolution != nullptr ) delete currentSolution;
        currentSolution = new SolutionVC( X );
    }



//    DEBUG(S);
//    DEBUG(S1);
//    DEBUG(X);
//    DEBUG(W);


    if( !supressAllLogs ) cerr << "ITER #" << iterationsDone << "  --------------------->     PartitionSVC, |VC| = " << X.size() << endl;
    if( !supressAllLogs ) cerr << "\rS.size: " << S.size() << "     S1.size:   " << S1.size() << "    W.size: " << W.size() << "    " << endl;

}



VI PartitionSVC::getRandomSetS1() {
    VI res;
    UniformDoubleGenerator gen(0,1);

    double alpha = svcParams.alpha;

    if( (1+iterationsDone) % svcParams.localOptimumViolationFrequency == 0 ){
        alpha = 0.5;
        DEBUG(iterationsDone);
        DEBUG(alpha);
        ENDL(3);
    }

//    if( (iterationsDone % 200) == 0 ){
////        cerr << "changing alpha" << endl;
//        alpha = 1 - (1-alpha)/2; // this here is to make svcParams.alpha small for large wuantity of changes and to still be able to get out of local minimum
//    }
//    DEBUG(alpha);

    for( int d : S ){
        if( gen.rand() < alpha ){
            res.push_back(d);
        }
    }

    return res;
}

VI PartitionSVC::getSetWForS1(VI &S1) {
    sort(ALL(S1));
    VI S2;
    set_difference( ALL(S), ALL(S1), back_inserter(S2) );

    VB neighOfS2(V.size(),false);
    for( int p : S2 ){
        for( int d : V[p] ){
            neighOfS2[d] = true;
        }
    }

    VB inX(V.size(),false);
    for(int p : X) inX[p] = true;

    VI W;
    for( int i=0; i<V.size(); i++ ){
        if( inX[i] && !neighOfS2[i] ) W.push_back(i);
    }
    return W;
}



void PartitionSVC::forceGettingOutOfLocalOptimum() {

    S = VCUtils::getRandomMIS(V);
    sort(ALL(S));
    X = CombinatoricUtils::getFullSetDifference(V.size(),S);
    sort(ALL(X));
    return;







    random_shuffle(ALL(S));
    unordered_set<int> neighS;

    VI removedNodes;
//    cerr << "before filling   ";
//    DEBUG(S.size());
//    DEBUG(S);

//    int oldSize = S.size();
    for( int i=0; i< min( (int)S.size(), svcParams.localOptimumViolationDegree ); i++ ){
        removedNodes.push_back( S.back() );

        for( int d : V[ S.back() ] ) neighS.insert(d);

        X.push_back(S.back());
        S.pop_back();
    }


//    DEBUG(removedNodes.size());
//    DEBUG(removedNodes);
//    DEBUG(neighS);
    S = VCUtils::fillRandomToMaximalIS( V, VI( ALL(neighS) ), S ); // filling S to maximal independent set
//    cerr << "after filling    ";
//    DEBUG(oldSize);
//    DEBUG(S.size());
//    DEBUG(S);
//    int sizeDiff = oldSize - (int)S.size();
//    DEBUG(sizeDiff);
//    VI addedNodes( S.end()  - (sizeDiff > 0 ? sizeDiff : 0), S.end() );
//    DEBUG(addedNodes);

    sort(ALL(S));
    X = CombinatoricUtils::getFullSetDifference( V.size(),S );
    sort(ALL(X));

    if( !supressAllLogs ){
        cerr << "FORCED OUT OF LOCAL OPTIMUM" << endl;
        ENDL(10);
//        exit(1);
    }
}

PartitionSVCParams &PartitionSVC::getSvcParams() {
    return svcParams;
}














void PartitionSVC::test() {
    VVI V;

    bool useRandomGraph = false;
    if(useRandomGraph){
//          if( V.empty() ) V = GraphGenerator::getRandomGraph(  500,1000   );
        if( V.empty() ) V = GraphGenerator::getRandomGraph( 1'000, 12'000   ); //  (4'000,80'000) --> |VC*| <= 3463

    }else{
        V = GraphReader::readGraphDIMACSWunweighed(cin,true);
//        V = GraphReader::readGraphStandardEdges(cin);
    }


    int N = V.size();
    int M = 0; for(auto v : V) M += v.size();
    M >>= 1;

    cerr << "N = " << N << "   M = " << M << endl;


    PartitionSVC partSVC(V);



    partSVC.svcParams.alpha = 0.75;

    partSVC.svcParams.initialSolutionMaxRunTime = 5'000;
    partSVC.svcParams.maxRunTimeForSubgraph = 1'000;

    partSVC.svcParams.iterationsPerSubgraph = 100;

    partSVC.svcParams.setInitialSolutionForSubgraph = false;

    partSVC.svcParams.maxDeviationFromBestResult = 2000;
//    partSVC.svcParams.localOptimumViolationDegree = 2;
    partSVC.svcParams.localOptimumViolationFrequency = 100;

    partSVC.setTakeFirstBestSolution(false);

    int SECONDS = 30;
    partSVC.setMaxRunTime(SECONDS * 1000);
    partSVC.svcParams.initialSolutionIterations = 1;
    partSVC.run();

    SolutionVC *s = (SolutionVC*) partSVC.getBestSolution();
    VI vc = s->getVC();
    cerr << "s S " << N << " " << vc.size() << endl;
    cout << "s S " << N << " " << vc.size() << endl;




//                                    for( int p : bullsToChange ) cout << p+1 << endl;
}



