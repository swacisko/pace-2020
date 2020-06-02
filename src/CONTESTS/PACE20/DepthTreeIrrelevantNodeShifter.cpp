/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <CONTESTS/PACE20/DepthTreeIrrelevantNodeShifter.h>
#include <graphs/GraphReader.h>
#include <graphs/GraphUtils.h>


void DepthTreeIrrelevantNodeShifter::shiftIrrelevantNodes(DepthTree &dt) {

    assert( dt.isCorrect() );

    int begHeight = dt.height;

    auto nodeDepths = dt.getNodeDepths();

//    int sumOfDepths = accumulate( ALL(nodeDepths),0, [](int a, PII b){return a+b.second;} );

    auto stdStruct = dt.getStandardStructure();
    VVI* V = dt.V;
    int N = V->size();
    int root = dt.root;

    VI toReattach;
    VI lowestNeighbor(N,-1);

    for( int i=0; i<N; i++ ){

        bool hasNeighDown = false;
        for( int p : (*V)[i] ){
            if( nodeDepths[p] > nodeDepths[i] ){
                hasNeighDown = true;
            }
            if( lowestNeighbor[i] == -1 || nodeDepths[p] > nodeDepths[ lowestNeighbor[i] ] ) lowestNeighbor[i] = p;
        }

        if(!hasNeighDown) toReattach.push_back(i);
    }

    sort( ALL(toReattach), [&nodeDepths](int a, int b){ return nodeDepths[a] < nodeDepths[b]; } );
//    cerr << "Nodes to reattach from topmost to bottommost: " << toReattach << endl;

//    cerr << "dt.par before reattaching: " << dt.par << endl;

    for( int p : toReattach ){
//        cerr << "reattaching " << p << endl;
        VI sons;
        int par = -2;

        for( int d : stdStruct[p] ){
            if( nodeDepths[d] > nodeDepths[p] ) sons.push_back(d);
            else par = d;
        }
        for( int t : sons ){
//            dt.par[t] = par;
//            cerr << "\tassigning par[" << t << "] to " << par << endl;

            dt.par[t] = dt.par[p];
//            cerr << "\tassigning par[" << t << "] to " << dt.par[p] << endl;
        }

        assert( par >= -1 && par < N );
        assert( lowestNeighbor[p] >=0 && lowestNeighbor[p] < N );

//        cerr << "assigning par[" << p << "] to " << lowestNeighbor[p] << endl;
        dt.par[p] = lowestNeighbor[p];
//        ENDL(1);
    }

//    cerr << "dt.par after reattaching: " << dt.par << endl;

    dt.height = dt.calculateHeight();
//    DEBUG(dt.height);

    int endHeight = dt.height;

    assert( endHeight <= begHeight );

//    if( endHeight < begHeight ){
//        cerr << "Better height done in irrelevant node shifter" << endl;
//        cerr << "CAUTION! SLOW CHECK OF isCorrect!" << endl << endl; assert( dt.isCorrect() );
//        DEBUG(begHeight);
//        DEBUG(endHeight);
//        ENDL(1);
//        exit(1);
//    }

//    nodeDepths = dt.getNodeDepths();
//    int sumOfDepths2 = accumulate( ALL(nodeDepths), 0, [](int a, PII b){return a+b.second;} );
//    if( sumOfDepths != sumOfDepths2 ) {
//        DEBUG(sumOfDepths);
//        DEBUG(sumOfDepths2);
//        ENDL(1);
//        exit(1);
//    }

    assert( dt.isCorrect() );
}

void DepthTreeIrrelevantNodeShifter::test(){
//    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
//    DEBUG(V.size());
//    DEBUG( GraphUtils::countEdges(V) );

//    VVI V = { {1,2,16}, {0,3,4}, {0}, {1,7,16,17}, {1,5,6}, {4}, {4}, {3,8,9,10}, {7,11,12}, {7}, {7}, // nodes 0 - 10 inclusive
//              {8}, {8,13,14,15}, {12}, {12}, {12}, {0,19}, {3}, {3}, {16,20,21}, {19}, {19} };

    VVI V = { {1,5}, {0,2,3,4,5}, {1,5}, {1}, {1}, {0,1,2} };

    int N = V.size();
    DepthTree dt(V);
    dt.root = 0;
    dt.height = 11;
    dt.V = &V;

    dt.par = { {0,-1}, {1,0}, {2,1}, {3,2}, {4,3}, {5,4} };

//    dt.par = { {0,-1}, {1,0}, {2,1}, {3,2}, {4,3}, {5,4}, {6,4}, {7,3}, {8,7}, {9,8}, {10,9}, // nodes 0 - 10 inclusive
//               {11,10}, {12,11}, {13,12}, {14,12}, {15,12}, {16,3}, {17,16}, {18,17}, {19,17}, {20,19}, {21,20}
//               };


    shiftIrrelevantNodes(dt);

    assert( dt.isCorrect() );

    exit(1);
}

