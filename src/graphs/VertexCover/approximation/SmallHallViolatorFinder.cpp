/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <graphs/VertexCover/approximation/SmallHallViolatorFinder.h>
#include <graphs/GraphInducer.h>
#include <graphs/VertexCover/SolutionVC.h>
#include <graphs/GraphUtils.h>
#include <combinatorics/CombinatoricUtils.h>
#include <utils/RandomNumberGenerators.h>
#include "graphs/VertexCover/approximation/SwapVC.h"

SmallHallViolatorFinder::SmallHallViolatorFinder(VVI &V, VI &S) {
    this->V = V;
    this->S = S;

    makeInitialStuff();

}

void SmallHallViolatorFinder::makeInitialStuff() {

    X = VI( V.size() - S.size() );
    sort( ALL(S) );
    VI perm(V.size());
    iota(ALL(perm),0);
    set_difference( ALL(perm), ALL(S), X.begin() );

    sort(ALL(X)); // this should be dispensible

    degreeInS2 = VI(V.size(),0);

    inS = VB(V.size(),false);
    for(int p : S) inS[p] = true;

    inX = VB(V.size(),false);
    for(int p : X) inX[p] = true;

    for( int p : S ){
        for( int d : V[p] ){
            degreeInS2[d]++;
        }
    }

    admissibleNodesInXForIndependentSet = VB( V.size(),true );

}


VI SmallHallViolatorFinder::findSwapCandidate(int centerNodeMaxDegree, bool admitHallEqualizers) {

    fill( ALL(admissibleNodesInXForIndependentSet),true );

    unordered_set<int> swapCandidate;
    subsetsOfSConsidered.clear();

    VI perm = CombinatoricUtils::getRandomPermutation(V.size());
    int cnt = 0;
    for( int i : perm ){

//        cerr << "\rlooking for small HV / HEQ: " << ((double)100*cnt++ / perm.size() ) << "          " << flush;

        if( !inX[i] || admissibleNodesInXForIndependentSet[i] == false ) continue;

        int degreeInS = 0;
        for( int d : V[i] ){
            if( inS[d] ) degreeInS++;
        }

        if( degreeInS <= centerNodeMaxDegree ){

//            ENDL(2);
//            cerr << "looking for local swap candidate" << endl;

            VI localSwapCandidate;
            if( degreeInS > 1 ) localSwapCandidate = findCenterNodeHallViolator( i,admitHallEqualizers );
            else localSwapCandidate = findDeg1CenterNodeHallViolator( i,admitHallEqualizers );


            swapCandidate.insert( ALL(localSwapCandidate) ); // adding localCandidate to general candidate.

            for( int p : localSwapCandidate ){ // marking neighbors of nodes in that candidate as NOT ADMISSIBLE for further violators

                    admissibleNodesInXForIndependentSet[p] = false;

                for( int d : V[p] ){


                    if( inX[d] ){
                        admissibleNodesInXForIndependentSet[d] = false;
                    }
                }
            }

//            DEBUG(localSwapCandidate);






//            VI mis = VI( ALL(swapCandidate) );
//            VI neighborhoodInS = GraphUtils::getNeighborhoodOfAInB(V,mis,S,false);
//            if( neighborhoodInS.size() > mis.size() ){
//                DEBUG(neighborhoodInS);
//                DEBUG(mis);
//                DEBUG(localSwapCandidate);
//                DEBUG( GraphUtils::getNeighborhoodOfAInB(V,localSwapCandidate,S) );
//                ERROR("","IN THE MIDDLE, NOT A VIOLATOR");
//            }






        }

    }

//    ENDL(2);
//    DEBUG(swapCandidate);

    VI mis = VI( ALL(swapCandidate) );
    VI neighborhoodInS = GraphUtils::getNeighborhoodOfAInB(V,mis,S,false);


//        cerr << "SIEMANKO" << endl;
//        DEBUG(neighborhoodInS);
//        DEBUG(mis);
//        DEBUG(admitHallEqualizers);


    if( neighborhoodInS.size() > mis.size() ){
        DEBUG(neighborhoodInS);
        DEBUG(mis);
        ERROR("","NOT A VIOLATOR");
    }
    if( !VCUtils::isIndependentSet( V,mis ) ){
        DEBUG(neighborhoodInS);
        DEBUG(mis);
        ERROR( mis, "NOT A MIS IN V!" );
    }


    return mis;

}

VI SmallHallViolatorFinder::findDeg1CenterNodeHallViolator(int centerNode, bool admitHallEqualizer) {
    if( !inX[centerNode] ){/* if given node is not in X i give error*/ ERROR(centerNode,"NOT IN X!"); }

    VI neighInS;
    for( int d : V[centerNode] ){ if( inS[d] ) neighInS.push_back(d); }

    sort( ALL(neighInS) );
    neighInS.resize( unique( ALL(neighInS) ) - neighInS.begin() );
    if( neighInS.empty() ) return VI( 1,centerNode );

    VI W = findSetW( neighInS );
    sort(ALL(W));

    LL hash = getHash( neighInS );
    if( subsetsOfSConsidered.count(hash) ) return VI();
    else subsetsOfSConsidered.insert( hash );

    InducedGraph GW = GraphInducer::induce( V,W );


    VI res;
    for( int i=0; i<GW.V.size(); i++ ){
        if( GW.V[i].size() < GW.V.size()-1 ){ // if node i has an antiedge, than i found an independent set of size 2
            VB neighborhood(GW.V.size(),false);
            for( int d : GW.V[i] ) neighborhood[d] = true; // neighborhood of node i in GW
            for( int j=0; j<GW.V.size(); j++ ){
                if( i != j && !neighborhood[j] ){
                    res.push_back( GW.nodes[j] );
                    break;
                }
            }

            res.push_back( GW.nodes[i] );
            break;
        }
    }

//    if( !res.empty() ){
//        cerr << "FOUND violator for deg1 center node in small hall violator finder" << endl;
//        DEBUG(res.size());
//        DEBUG(res[0]);
//        DEBUG(res[1]);
//        DEBUG(V[res[0]]);
//        DEBUG(V[res[1]]);
//        VI res0(1,res[0]);
//        VI res1(1,res[1]);
//        DEBUG( GraphUtils::getNeighborhoodOfAInB( V, res0,S ) );
//        DEBUG( GraphUtils::getNeighborhoodOfAInB( V, res1,S ) );
//        exit(1);
//    }

    if( res.empty() && admitHallEqualizer ){
        UniformIntGenerator gen(0,W.size()-1);
        res.push_back( GW.nodes[ gen.rand() ] );
    }


    return res;
}

VI SmallHallViolatorFinder::findCenterNodeHallViolator(int centerNode, bool admitHallEqualizer) {
    if( !inX[centerNode] ){// if given node is not in X i give error
        ERROR(centerNode,"NOT IN X!");
    }

    VI neighInS;
    for( int d : V[centerNode] ){
        if( inS[d] ) neighInS.push_back(d);
    }

    sort( ALL(neighInS) );
    neighInS.resize( unique( ALL(neighInS) ) - neighInS.begin() );
    if( neighInS.empty() ) return VI( 1,centerNode );


    VI W = findSetW( neighInS );
    sort(ALL(W));

//    ENDL(3);
//    DEBUG(centerNode);
//    DEBUG(neighInS);
//    DEBUG(W);

//    for( int p : neighInS ){ // restoring values
//        for( int d : V[p] ){
//            degreeInS2[d]++;
//        }
//    }

    LL hash = getHash( neighInS );
    if( subsetsOfSConsidered.count(hash) ) return VI();
    else subsetsOfSConsidered.insert( hash );

    InducedGraph GW = GraphInducer::induce( V,W );

//    DEBUG(GW.V);

//    cerr << "    -> processing subgraph of size " << GW.V.size() << "    " << flush;

    vcFinder = new SwapVC( GW.V );
    vcFinder->setMaxIterations(3);
    vcFinder->getParams().useKernelization = false;
    vcFinder->getParams().useSmallHallViolatorFinder = false;
    vcFinder->setSupressAllLogs(true);
    vcFinder->run();
    VI vcInW = (  (SolutionVC*) vcFinder->getBestSolution()  )->getVC();

    int misInWSize = GW.V.size() - vcInW.size();
    VI res;

//    DEBUG(misInWSize);

    if( ( misInWSize > neighInS.size() ) || ( misInWSize == neighInS.size() && admitHallEqualizer ) ){
        sort( ALL(vcInW) );


        VI misInW; //( GW.V.size() - vcInW.size() );
        VI perm( W.size() );
        iota( ALL(perm),0 );
        set_difference( ALL(perm), ALL(vcInW), back_inserter(misInW) );

//        DEBUG(misInW.size());
//        DEBUG(neighInS.size());
//        ENDL( 1 );

//        if( misInW.size() != misInW.size() ) ERROR("", "misInWSize != misInW.size()");

        for( int & p : misInW ) p = GW.nodes[p]; // mapping independent set from GW to G.
//        DEBUG(misInW);

        res = misInW;
    }
    else{
        res = VI();
    }

//    delete vcFinder->getBestSolution();
    delete vcFinder;
    vcFinder = nullptr;

//    DEBUG(res);

    return res;

}

VI SmallHallViolatorFinder::findSetW(VI & neighInS) {

    VI W;
    for( int p : neighInS ){
        for( int d : V[p] ){
            degreeInS2[d]--;
            if( degreeInS2[d] == 0 && admissibleNodesInXForIndependentSet[d] ) W.push_back(d);
        }
    }


    for( int p : neighInS ){ // restoring values
        for( int d : V[p] ){
            degreeInS2[d]++;
        }
    }

    return W;
}

LL SmallHallViolatorFinder::getHash(VI &subsetOfS) {
    LL hash = 0;
    LL MOD = 1'000'000'000;
    MOD *= MOD;
    MOD++;

    for( int d : subsetOfS ){
        hash += (LL)d * d + d + ( d ^ ( d >> 1 ) );
        if( hash > MOD ) hash %= MOD;
    }
    return hash;
}




