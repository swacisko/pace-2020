/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/separatorcreators/FlowSeparatorCreator.h>
#include <graphs/GraphReader.h>
#include <graphs/GraphUtils.h>
#include <graphs/flow/DisjointPaths.h>
#include <combinatorics/CombinatoricUtils.h>
#include <graphs/graphtraversals/BFS.h>
#include <graphs/landmarks/LandmarkCreator.h>
#include <CONTESTS/PACE20/Pace20Params.h>
#include "CONTESTS/PACE20/separatorcreators/FlowSeparatorCreator.h"
#include "CONTESTS/PACE20/separatorminimizers/GreedyNodeEdgeMinimizer.h"
#include "utils/TimeMeasurer.h"


FlowSeparatorCreator::FlowSeparatorCreator(SeparatorMinimizer *minimizer) {
    this->minimizer = minimizer;
}

vector<Separator> FlowSeparatorCreator::createSeparators(VVI &V, int repeats) {

    if( V.size() < 30 ) return vector<Separator>();
//    if( V.size() < 50 ) return vector<Separator>();

//    GreedyNodeEdgeMinimizer minim(GreedyNodeEdgeMinimizer::MINIMIZE_EDGES);
    LandmarkCreator lcr;
    int LANDMARKS = min( (int)V.size()-1, 30);
//    int LANDMARKS = min( (int)V.size()-1, 50);




    VI landmarks = lcr.getLandmarks( V, 0, LANDMARKS, 10 );
//    cerr << "landmarks created" << endl;

//    LANDMARKS = 29; // here adding some random landmarks
//    while( landmarks.size() < LANDMARKS ){
//        int a = rand() % V.size();
//        if( find( ALL(landmarks),a ) == landmarks.end() ) landmarks.push_back(a);
//    }


    vector<Separator> res;
    for( int i=0; i < min( repeats, LANDMARKS * (LANDMARKS-1) / 2 ); i++ ){
        if( Pace20Params::tle ) break;
//        VI s = CombinatoricUtils::getRandomSubset( LANDMARKS-1,2 );
//        s[0] = landmarks[s[0]];  s[1] = landmarks[s[1]];

        VI s = CombinatoricUtils::getRandomSubset( LANDMARKS-1,4 );
        random_shuffle(ALL(s));
        for( int& d : s ) d = landmarks[d];

        VI sources = { s[0], s[1] };
        VI ends = {s[2], s[3]};

        int dist = (i+1) % 4;
//        Separator sep = getSeparatorForFlow( V,{s[0]} , {s[1]}, dist );
        Separator sep = getSeparatorForFlow( V, sources , ends, dist );
        res.push_back(sep);
    }

    return res;
}


Separator FlowSeparatorCreator::getSeparatorForFlow(VVI &V, VI sources, VI ends) {
    VVI paths = DisjointPaths::getSetOfDisjointPaths( V, sources, ends, true ); // using unit flow
//    VVI paths = DisjointPaths::getSetOfDisjointPaths( V, sources, ends, false ); // using standard flow


//    DEBUG(sources);
//    DEBUG(ends);
    DEBUG(paths.size());

    VI nodes;
    nodes.reserve( accumulate( ALL(paths), 0, []( int s, VI& pth ){ return s + pth.size(); } ) );
//    nodes.insert(nodes.end(), ALL(sources));
//    nodes.insert(nodes.end(), ALL(ends));
    VB was(V.size(),false);
    for( VI& v : paths ){
//        for(int i=1; i<(int)v.size()-1; i++) nodes.push_back(v[i]);
        for(int i=0; i<(int)v.size(); i++){
            if( !was[v[i]] ){
                nodes.push_back(v[i]);
                was[v[i]] = true;
            }
        }
    }


    Separator sep(V,nodes);
    sep.createSeparatorStats();


    if( minimizer != nullptr && sep.nodes.size() <= Pace20Params::maxSeparatorSizeForFlowMinimizer ) sep = minimizer->minimizeSeparator(sep);



    return sep;
}

Separator FlowSeparatorCreator::getSeparatorForFlow(VVI &V, VI sources, VI ends, int dist) {
    VVI sLayer = BFS::getBfsLayers(V,sources);

//    DEBUG(sources);
//    DEBUG(ends);

    VI S;
    for( int i=0; i < min( dist+1, (int) sLayer.size() ); i++ ){
        S.insert( S.end(), ALL(sLayer[i]) );
    }

//    DEBUG(S);

    VVI eLayer = BFS::getBfsLayers(V,ends);
    VI E;
    for( int i=0; i < min( dist+1, (int) eLayer.size() ); i++ ){
        E.insert( E.end(), ALL(eLayer[i]) );
    }

    unordered_set<int> inS(ALL(S));
    for(int i = (int)E.size()-1; i>=0; i--){
        if( inS.count(E[i]) ){
            swap(E[i], E.back());
            E.pop_back();
        }
    }

//    DEBUG(E);

    if( E.empty() ){
//        cerr << "sources contain all ends, restoring initial" << endl;
        S = sources;
        E = ends;
    }

    auto res = getSeparatorForFlow( V, S,E );

    return res;
}

void FlowSeparatorCreator::test() {
    VVI V = GraphReader::readGraphDIMACSWunweighed(cin,false);
    DEBUG(V.size());
    DEBUG( GraphUtils::countEdges(V) );

    GreedyNodeEdgeMinimizer minim(GreedyNodeEdgeMinimizer::MINIMIZE_EDGES);
    FlowSeparatorCreator fsc(&minim);

    LandmarkCreator lcr;
    int LANDMARKS = 10;
    VI landmarks = lcr.getLandmarks( V, 0, LANDMARKS, 0 );

//    for( int d : VI( {15,18,21} ) ){
//        Separator sep(V,{d});
//        sep.createSeparatorStats();
//        DEBUG(sep);
//    }

    cerr << "landmarks created" << endl;

    TimeMeasurer::startMeasurement( "check" );
    for( int i=0; i < LANDMARKS * (LANDMARKS-1) / 2 ; i++ ){

//        VI s = CombinatoricUtils::getRandomSubset( V.size()-1,2 );

        VI s = CombinatoricUtils::getRandomSubset( LANDMARKS-1,2 );
        s[0] = landmarks[s[0]];  s[1] = landmarks[s[1]];


        VI sources = {s[0]};
        VI ends = {s[1]};

        int dist = (i+1) % 5;
        Separator sep = fsc.getSeparatorForFlow( V, sources , ends, dist );

//        DEBUG(sep);

//        ENDL(1);

    }
    TimeMeasurer::stopMeasurement( "check" );
    TimeMeasurer::writeAllMeasurements();
    exit(1);






    // checking number of paths

    for( int i=0; i<10; i++ ){
        if( V.size() < 4 ) continue;

        VI s = CombinatoricUtils::getRandomSubset( V.size()-1,4 );

//        V = {{1,2}, {0,3}, {0,3}, {1,2}};
//        s = { 0,3 };

        TimeMeasurer::startMeasurement( "UnitFlow disjoint paths" );
        auto paths = DisjointPaths::getSetOfDisjointPaths( V, { s[0], s[1] }, { s[2], s[3] }, -1, true );
        TimeMeasurer::stopMeasurement( "UnitFlow disjoint paths" );

        TimeMeasurer::startMeasurement( "MaxFlow disjoint paths" );
        auto pathsMaxFlow = DisjointPaths::getSetOfDisjointPaths( V,  { s[0], s[1] }, { s[2], s[3] }, -1, false );
        TimeMeasurer::stopMeasurement( "MaxFlow disjoint paths" );

        if( paths.size() != pathsMaxFlow.size() ){

            DEBUG(paths.size());
            DEBUG(pathsMaxFlow.size());

            DEBUG(paths);
            DEBUG(pathsMaxFlow);

            DEBUG(V.size());
//            DEBUG((*V));
            DEBUG(s);
            exit(1);
        }


        cerr << "There are " << paths.size() << " disjoint paths between sources: {" << s[0] << ", " << s[1] << "}   and ends: {" << s[2] << ", " << s[3] << "}" << endl;
//        DEBUG(paths);
//        ENDL(2);
        int L = 4;
//        for(VI& v : paths) L += v.size()-2;
        for(VI& v : paths) L += v.size()-2;
        cerr << "Their total length equals: " << L << endl << endl;

        bool check = true;
        unordered_set<int> zb;
        for( VI& v : paths ){

            if( v[0] != s[0] && v[0] != s[1] ){
                cerr << "path does not begin in a source node" << endl;
                exit(1);
            }

            if( v.back() != s[2] && v.back() != s[3] ){
                cerr << "path does not end in an end node" << endl;
                exit(1);
            }


            for( int j=1; j<v.size()-1; j++ ){
                if( zb.count( v[j] ) ) check = false;
                zb.insert( v[j] );
                for(int k=0; k<4; k++) if( v[j] == s[k] ) exit(17);
            }

//            if(v.size() <= 3) DEBUG(v);

        }

        if( check == false ){
            cerr << "Paths are not disjoint!!" << endl;
            exit(1);
        }

        VB was(V.size(),false);
        for(int d : zb) was[d] = true;
        VI pth;

        function<bool(int,int)> augment = [=,&was,&augment,&s,&pth](int x, int y){
            if( was[x] ) return false;
            if( x == s[0] && y == s[1] ) return false;
            if( x == s[1] && y == s[0] ) return false;

            if( x == s[2] || x == s[3] ){
                pth.push_back(x);
                return true;
            }
            was[x] = true;

            for(int d : (V)[x]){
                if( x == y && (d == s[2] || d == s[3]) ) return false;

                if( augment(d,y) ){
                    pth.push_back(x);
                    return true;
                }
            }
            return false;
        };

        if( augment(s[0], s[0]) || augment(s[1],s[1]) ){
//            DEBUG((*V));
//            DEBUG(paths);
//            reverse(ALL(pth)); DEBUG(pth);
            cerr << "there exists additional disjoint path from sources to ends!" << endl;
            exit(1);
        }

    }

    TimeMeasurer::writeAllMeasurements();
    exit(1);
}



/**

sources: {28, 26, 24, 27, 29}
ends: {3, 4, 5}
paths: {{24, 25, 21, 18, 16, 15, 6, 5}}
nodes: {24, 25, 21, 18, 16, 15, 6, 5}
sep.stats: size: 8 | #comps: 9 | maxSize: 5 (30) | maxEdges: 4 (33) | ahNode: 8 | ahEdge: 8.82759
sep: [[ {21}  |  size: 1 | #comps: 2 | maxSize: 21 (30) | maxEdges: 22 (33) | ahNode: 3.19882 | ahEdge: 2.90909 ]]

 */