/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <graphs/cliques/CliqueUtils.h>
#include <graphs/GraphUtils.h>

#include "graphs/cliques/CliqueUtils.h"


#include "Makros.h"

namespace CliqueUtils{


    bool isClique( VVI& V, VI& clq){
        unordered_set<int> zb(ALL(clq));
        for( int p : clq ){
            int t = 0;
            for(int d : V[p]){
                if( zb.count(d) ) t++;
            }
            if( t != clq.size()-1 ) return false;
        }
        return true;
    }

    void fillToClique(VVI &V, VI nodes) {
        vector< unordered_set<int> > presEdges(nodes.size());
        for( int i=0; i<nodes.size(); i++ ){
            int p = nodes[i];
            for( int d : V[p] ) if( d > p ) presEdges[i].insert(d);
        }

        for( int i=0; i<nodes.size(); i++ ){
            int p = nodes[i];
            for( int d : nodes ){
                if( d > p && presEdges[i].count(d) == 0 ) GraphUtils::addEdge( V,p,d );
            }
        }

        assert( isClique( V, nodes ) );
    }


}
