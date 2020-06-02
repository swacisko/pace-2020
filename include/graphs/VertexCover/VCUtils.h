/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_VCUTILS_H
#define ALGORITHMSPROJECT_VCUTILS_H

#include "Makros.h"

class VCUtils{
public:

    static VI getRandomPermutation( int N );
    /**
     * Given some nodes in vc set, this function returns random minimal vertex cover of V that contains vc
     * @param V
     * @param vc
     * @return
     */
    static VI fillRandomToMinimalVC( VVI V, VI vc );


    /**
     *
     * @param V
     * @return random minimal vertex cover of V
     */
    static VI getRandomMinimalVC( VVI & V );



    /**
     *
     * @param V
     * @return random maximal independent set of @V
     */
    static VI getRandomMIS( VVI & V );

    /**
     * Similar as fillRandomToMinimalVC, but fills to maximal IS, not minimal VC.
     * @param V
     * @param mis
     * @return maximal IS that contains @mis
     */
    static VI fillRandomToMaximalIS( VVI & V, VI mis );

    /**
     * Function fills nodes from given set @fillFrom of nodes to a mis, that contains given @mis.
     * This function may be used to get a MIS in any induced subgraph (by nodes in fillFrom) of given graph V.
     * @param V
     * @param fillFrom
     * @param mis
     * @return independent set in V, maximal with regard to nodes in fillFrom, that contains mis.
     */
    static VI fillRandomToMaximalIS( VVI & V, VI fillFrom, VI mis );

    /**
     *
     * @param V
     * @param vc
     * @return true if @vc is a vertex cover of @V.
     */
    static bool isVertexCover( VVI & V, VI & vc );

    /**
     *
     * @param V
     * @param S
     * @return true if S is an independent set in V
     */
    static bool isIndependentSet( VVI & V, VI & S );

    /**
     * Functions sorts nodes with regard to the highest degree, then adds in that order.
     * @param V
     * @return vertex cover of V
     */
    static VI getVCGreedyMaxDegree( VVI & V );

    /**
     * Function chooses node with grates degree, adds it to V, then removes it from V and reapeats procedure.
     * Implemented in O( E + N*lgN )
     * @param V
     * @return
     */
    static VI getVCGreedyMaxItarativeDegree(VVI & V);

};




#endif //ALGORITHMSPROJECT_VCUTILS_H
