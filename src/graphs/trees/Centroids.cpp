/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/
#include "graphs/trees/Centroids.h"
#include "graphs/GraphReader.h"
#include "graphs/GraphUtils.h"

namespace Centroids{



    int getCentroid( VVI& V, int v, VB& was, VI& subtreeSize ){
        if( was[v] ){
            cerr << "getting centroid for not valid v" << endl;
            exit(1);
        }

        function< int(int,int) > sizeDfs = [&V, &was, &subtreeSize, &sizeDfs]( int num, int par ){
            subtreeSize[num] = 1;
            for( int d : V[num] ){
                if(was[d]) continue;
                if( d != par ) subtreeSize[num] += sizeDfs(d,num);
            }
            return subtreeSize[num];
        };

        int totalSize = sizeDfs(v,v);

//        DEBUG(subtreeSize);

        int centroid = -1;
        int bestValue = V.size();

        function< void(int,int) > findCentroid = [&V, &was, &subtreeSize, &totalSize, &findCentroid, &centroid, &bestValue]( int num, int par ){
            int m = totalSize - subtreeSize[num];

            for( int d : V[num] ){
                if( !was[d] && d != par ) {
                    m = max(m, subtreeSize[d]);
                    findCentroid(d,num);
                }

            }

            if( m < bestValue ){
                centroid = num;
                bestValue = m;
            }

//            DEBUG(num);
//            DEBUG(m);
//            DEBUG(bestValue);
//            ENDL(1);
        };

        findCentroid(v,v);
//        DEBUG(centroid);
//        DEBUG(bestValue);
//        exit(1);

        return centroid;
    }

    int getCentroid(VVI& V){
        int N = V.size();
        VB was(N,false);
        VI subtreeSizes(N);
        return getCentroid( V,0,was,subtreeSizes );
    }

    VI getCentroidDecomposition(VVI& V){
        int N = V.size();
        VI par(N,-1);
        VB was(N,false);
        VI subtreeSizes(N);

        VPII neigh = { {0,-1} };

        for(int i=0; i<neigh.size(); i++){
            PII p = neigh[i];
            int v = p.first;

            int centr = getCentroid( V,v,was, subtreeSizes );
            was[centr] = true;
            par[centr] = p.second;

//            cerr << "Setting par of " << centr << " to " << p.second << endl;

            for( int d : V[centr] ){
                if( !was[d] ) neigh.push_back( {d,centr} );
            }
        }

//        DEBUG(par);

        return par;

    }



    void test(){
        VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
        DEBUG(V.size());
        DEBUG( GraphUtils::countEdges(V) );

        VI centr = getCentroidDecomposition(V);
        DEBUG(centr);



        exit(1);
    }


}