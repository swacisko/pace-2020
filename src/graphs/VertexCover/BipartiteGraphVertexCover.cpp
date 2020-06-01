//
// Created by sylwester on 3/27/20.
//

#include "graphs/VertexCover/BipartiteGraphVertexCover.h"


namespace BipartiteGraphVertexCover {


    VI getMinVCForBipartition(VVI &G, VI &matching, VB &bipartition) {
//    cerr << "Determining minimal VC for bipartition 0:   "; for(int i=0; i<G.size(); i++) if( bipartition[i] == false ) cerr << i << " "; cerr << endl;

        VI U; // vector of all unmatched vertices of G, that are in bipartition 0
        for (int i = 0; i < G.size(); i++) {
            if (matching[i] == -1 && bipartition[i] == false) U.push_back(i);
        }

//    cerr << "Set U of unmatched nodes in bipartition 0:  "; for(int p : U) cerr << p << " "; cerr << endl;


        VI Z; // vector of all vertices that are either in U or are connected to U by an alternating path.
        VB was(G.size(), false);
        VI neigh = U; // this if for bfs to mark all alternating paths.
        for (int i = 0; i < neigh.size(); i++) {
            int d = neigh[i];
            if (was[d]) continue;
            was[d] = true;
//        cerr << "d = " << d << "   adding to Z" << endl;
            Z.push_back(d);

            for (int p : G[d]) {
                if (was[p] || bipartition[p] == bipartition[d]) continue;
//            cerr << "\tadding p = " << p << " to Z" << endl;
                Z.push_back(p);
                neigh.push_back(matching[p]);

                was[p] = true;
            }
        }

//    cerr << "Set Z of all vertices that are in U or are connected by alternating path to U:  "; for(int p : Z) cerr << p << " "; cerr << endl;

        VB Z2(G.size(), false); // Z2 is just Z but in other representation
        for (int p : Z) Z2[p] = true;

        VI K; // K should be ( L \ Z ) + ( R & Z ), where L is the set of all nodes with bipartition 0 and R with bipartition 1

        for (int i = 0; i < G.size(); i++) {
            if (G[i].size() == 0) continue;

            if (bipartition[i] == false && !Z2[i]) {
                K.push_back(i);
            } else if (bipartition[i] == true && Z2[i]) {
                K.push_back(i);
            }
        }

//    cerr << "minimal vertex cover K:  "; for(int p : K) cerr << p << " "; cerr << endl;

        // NPW K should be a minimum vertex cover of all the edges between bipartition of the graph.
        return K;
    }


    VI getVertexCoverOfBipartiteGraph(VVI &V, VB &bipartition) {
        MaxMatchBipartite matcher;
        VI matching = matcher.getMaximumMatchingInBipartition(V, bipartition);
        return getMinVCForBipartition(V, matching, bipartition);
    }

}