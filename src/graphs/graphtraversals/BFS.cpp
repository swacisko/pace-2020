//
// Created by sylwester on 8/26/19.
//

#include <graphs/graphtraversals/BFS.h>

#include "graphs/graphtraversals/BFS.h"

namespace BFS{

    bool bfs( VVI & V, VI & dist, int beg, function< bool (int num) > haltCondition, function< void (int num) > inorderFunction  ){
        int INF = Constants::INF;
        VI neigh(1,beg);
        fill( ALL(dist), INF );
        dist[beg] = 0;


        for( int i=0; i<neigh.size(); i++ ){
            int p = neigh[i];

            inorderFunction(p);
            if( haltCondition(p) ) return true;

            for( int d : V[p] ){
                if( dist[d] == INF ){
                    dist[d] = dist[p]+1;
                    neigh.push_back(d);
                }
            }
        }

        return false;
    }

    VVI getBfsLayers(VVI &V, VI sources) {
        int N = SIZE(V);

        VB was(N,false);
        for( int beg : sources ) was[beg] = true;
        VI layer = sources;
        VI temp;

        VVI res;

        do{
            res.PB( layer );

            REP( i, SIZE(layer) ){
                int n = layer[i];


                REP( k, SIZE( V[n] ) ){
                    int d = V[n][k];
                    if( !was[d] ){
                        was[d] = true;
                        temp.PB(d);
                    }
                }
            }

            swap( layer,temp );
            temp.clear();

        }while( !layer.empty() );

        return res;
    }

    VVI getBfsLayers(VVI & V, int source){ return getBfsLayers(V, VI({source})); }


}