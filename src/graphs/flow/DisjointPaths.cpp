/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <graphs/flow/DisjointPaths.h>
#include <graphs/flow/UnitFlow.h>
#include "graphs/flow/MaxFlow.h"
#include "graphs/GraphUtils.h"

namespace DisjointPaths{


    VVI getSetOfDisjointPaths(VVI &V, VI sources, VI ends, int maxPathsBound, bool useUnitFlow) {
        int N = V.size();
        int INF = Constants::INF;

        VB isSource(N,false);
        VB isEnd(N,false);

        for(int s : sources) isSource[s] = true;
        for(int e : ends) isEnd[e] = true;

        if( maxPathsBound == -1 ) maxPathsBound = N;

        VVI bipGraph(2*N + 2);
        vector< unordered_map<int,int> > capacities(2*N+ 2);

        for( int i=0; i<N; i++ ){
            for( int d : V[i] ){
                bipGraph[i].push_back(N+d);
                capacities[i][N+d] = 1;
            }
            bipGraph[N+i].push_back(i);
            capacities[N+i][i] = 1;
        }

        // nodes 2N if artificial source that connects to all sources
        for(int s : sources){
            bipGraph[2*N].push_back(s);
            capacities[2*N][s] = INF;
        }

        // node 2N+1 is artificial end that is reachable from all ends
        for( int e : ends ){
            bipGraph[e].push_back( 2*N+1 );

            capacities[N+e][e] = INF;
            capacities[e][ 2*N+1 ] = INF;
        }

//        ENDL(2);
//        GraphUtils::writeGraphHumanReadable(bipGraph);
//        ENDL(2);
//        cerr << "capacities" << endl;
//        for(int i=0; i<capacities.size(); i++) cerr << i << ": " << capacities[i] << endl;




        vector< unordered_map<int,int> > saturation;
        if(useUnitFlow == false){
            MaxFlow flow(bipGraph,capacities);
            flow.calculateMaxFlow( 2*N, 2*N+1, maxPathsBound );
            saturation = flow.getEdgeSaturation();
//            DEBUG(flow.getMAX_FLOW());
        }else{
            UnitFlow flow(bipGraph);
            VI newEnds = ends; for(int& d : newEnds) d += N;
            auto flowVal = flow.calculateFlow( sources,newEnds, maxPathsBound );
            VVI sat = flow.getEdgeSaturation();
            saturation = vector< unordered_map<int,int> > (sat.size());
            for( int i=0; i<sat.size(); i++ ) for(int d : sat[i]) saturation[i][d] = 1;
//            DEBUG( flowVal );
        }


//        ENDL(2);
//        ENDL(2);
//        cerr << "saturation" << endl;
//        for(int i=0; i<saturation.size(); i++) cerr << i << ": " << saturation[i] << endl;


        VI next(N,-1);

        for( int i=0; i<N; i++ ){
            if(isEnd[i]) continue;

            for( int d : bipGraph[i] ){
                if( d != 2*N+2 ){
                    if( saturation[i][d] > 0 ){
                        if( !isSource[i] && next[i] != -1 ){
                            cerr << "next[i] != -1 but there should be only 1 edge saturated outgoing from non-source vertex " << i << endl;
                            exit(3);
                        }
                        next[i] = d-N;
                    }
                }
            }
        }

//        ENDL(2);
//        for( int i=0; i<next.size(); i++ ) cerr << "next[" << i << "] = " << next[i] << endl;

        VVI paths;
        VB was(N,false);

//        DEBUG(bipGraph);

        for( int s : sources ){
            for( int d : bipGraph[s] ){
                int p = d-N;

                if( ( was[p] && !isEnd[p] ) || ( next[p] == -1 && isEnd[p] == false ) || isSource[p] ) continue;
//                was[p] = true;

                VI path(1,s);
//                cerr << "starting paths at node " << s << "   p = " << p << endl;

                path.push_back(p);

                bool repet = false;

                while( isEnd[p] == false ){
                    if( was[p] ){ repet = true; break;};
//                    cerr << "p = " << p << endl;
                    was[p] = true;
                    p = next[p];
                    path.push_back(p);
                }


                if(repet) continue;

                paths.push_back(path);
            }
        }

//        DEBUG(paths);

        return paths;

    }
}