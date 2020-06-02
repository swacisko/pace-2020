/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/
#include <graphs/VertexCover/VCUtils.h>
#include <Constants.h>

#include "graphs/VertexCover/VCUtils.h"

VI VCUtils::getRandomPermutation( int N ){
    VI perm(N);
    iota(ALL(perm),0);
    random_shuffle(ALL(perm));
    return perm;
}

/**
 * Given some nodes in vc set, this function returns random minimal vertex cover of V that contains vc
 * @param V
 * @param vc
 * @return
 */
VI VCUtils::fillRandomToMinimalVC( VVI V, VI vc ){
    vector<bool> inVC( V.size(),false );
    for(auto p : vc) inVC[p] = true;

    VI perm = getRandomPermutation( V.size() );
    for( int p : perm ){
        if( inVC[p] ) continue;

        for( int d : V[p] ){
            if( !inVC[d] ){
                inVC[d] = true;
                vc.push_back(d);
            }
        }
    }

    return vc;
}


/**
 *
 * @param V
 * @return random minimal vertex cover of V
 */
VI VCUtils::getRandomMinimalVC( VVI & V ){
    VI vc;
    return fillRandomToMinimalVC( V, vc );
}



/**
 *
 * @param V
 * @return random maximal independent set of @V
 */
VI VCUtils::getRandomMIS( VVI & V ){
    VI mis;
    return fillRandomToMaximalIS( V,mis );
}

/**
 * Similar as fillRandomToMinimalVC, but fills to maximal IS, not minimal VC.
 * @param V
 * @param mis
 * @return maximal IS that contains @mis
 */
VI VCUtils::fillRandomToMaximalIS( VVI & V, VI mis ){
//        cerr << "filling " << mis << " to random MIS" << endl;
    VI perm = getRandomPermutation( V.size() );
    mis = fillRandomToMaximalIS( V, perm, mis );


//    VB inMis(V.size(),false);
//    for(int p : mis) inMis[p] = true;

//    for( int p : perm ){
//        if( inMis[p] ) continue;
//        bool hasNeighborInMis = false;
//        for( int d : V[p] ){
//            if( inMis[d] ){
//                hasNeighborInMis = true;
//                break;
//            }
//        }
//
//        if( !hasNeighborInMis ){
//            inMis[p] = true;
//            mis.push_back(p);
//        }
//    }
    return mis;
}

VI VCUtils::fillRandomToMaximalIS(VVI &V, VI fillFrom, VI mis) {
//    cerr << "entering fill random to mis" << endl;

    VB inMis(V.size(),false);
    for(int p : mis) inMis[p] = true;

    random_shuffle(ALL(fillFrom));
    for( int p : fillFrom ){
        if( inMis[p] ) continue;
        bool hasNeighborInMis = false;
        for( int d : V[p] ){
            if( inMis[d] ){
                hasNeighborInMis = true;
                break;
            }
        }

        if( !hasNeighborInMis ){
            inMis[p] = true;
            mis.push_back(p);


//            cerr << "filling " << p << endl;
        }
    }


    return mis;
}

/**
 *
 * @param V
 * @param vc
 * @return true if @vc is a vertex cover of @V.
 */
bool VCUtils::isVertexCover( VVI & V, VI & vc ){
    VB inVC(V.size(),false);
    for(int p : vc) inVC[p] = true;
    for( int i=0; i<V.size(); i++ ){
        if(inVC[i]) continue;
        for(int d : V[i]){
            if( !inVC[d] ) return false;
        }
    }
    return true;
}

VI VCUtils::getVCGreedyMaxDegree(VVI &V) {
    VI perm(V.size());
    iota(ALL(perm),0);

    sort( ALL(perm), [&perm, &V]( int a, int b ){ return V[a].size() > V[b].size(); } );
    VB inVC(V.size(),false);
    VI res;
    for( int i=0; i<perm.size(); i++ ){
        int d = perm[i];
        bool addToVC = false;
        for( int p : V[d] ){
            if( !inVC[p] ){
                addToVC = true;
                break;
            }
        }

        if( addToVC ){
            res.push_back(d);
            inVC[d] = true;
        }
    }
    return res;
}

VI VCUtils::getVCGreedyMaxItarativeDegree(VVI &V) {

    cerr << "OOPS" << endl;
    /*MyPQ<int> pq(V.size(), -1);

    VI res;
    for( int i=0; i<V.size(); i++ ) pq.setPriority( i, V[i].size() );
    while( pq.getPriority( pq.top() ) > 0 ){
        int d = pq.top();
        pq.setPriority(d, 0 );
        res.push_back(d);
        for( int p : V[d] ){
            pq.setPriority(p, pq.getPriority(p)-1 );
        }
    }
    return res;*/


    /*priority_queue<PII> pq;
    for( int i=0; i<V.size(); i++ ){
        pq.push( { V[i].size(),i } );
    }

    VB was(V.size(),false);
    VB inVC(V.size(),false);
    VI res;
    VI degrees(V.size());
    for(int i=0; i<V.size(); i++) degrees[i] = V[i].size();

    while( !pq.empty() ){
        PII t = pq.top();
        pq.remove();
        int d = t.second;

        if( was[d] ) continue;
        was[d] = true;


        bool addToVC = false;
        for( int p : V[d] ){
            if( !inVC[p] ){
                addToVC = true;
                break;
            }
        }

        if( addToVC ){
            res.push_back(d);
            inVC[d] = true;
            for( int p : V[d] ){
                degrees[p]--;
                pq.push( {degrees[p],p} );
            }
        }
    }

    return res;*/
}

bool VCUtils::isIndependentSet(VVI &V, VI &S) {
//    cerr << "checking if is independent set" << endl;
    VI perm(V.size());
    iota(ALL(perm),0);
    VI S2 = S;
    sort(ALL(S2));
    VI vc;
    set_difference( ALL(perm), ALL(S2),  std::back_inserter(vc) );
//    cerr << "checking if is bullsToChange" << endl;
    bool res = isVertexCover( V,vc );
//    cerr << "checked" << endl;
    return res;
}


