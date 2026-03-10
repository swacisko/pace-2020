/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_REMOVALORDERCREATOR_H
#define ALGORITHMSPROJECT_REMOVALORDERCREATOR_H

#include "Makros.h"
#include "../Separator.h"

class RemovalOrderSeparatorCreator{

public:

    RemovalOrderSeparatorCreator( VVI& V );

    /**
     * We remove from V nodes in given order, order[0], order[1], ....
     * In this way we will obtain many separators.
     *
     * CAUTION!
     * eval must evaluate separators based only on sep.stats.{ size, maxCompSize, maxCompEdges }. Other parameters are not considered!!
     * CAUTION!
     *
     * @param order
     * @return
     */
    Separator getBestSeparatorForRemovalOrder( VI order, function< bool(Separator&, Separator&) >  eval );


    /**
     *
     * @param V
     * @return nodes sorted in nonincreasing order
     */
    static VI getGreatestDegreeOrder(VVI &V);

    /**
     * (i+1)-th node is the node with greatest degree in the graph with first i nodes removed
     * we iterative select node with greatest degree and remove the one with greatest degree
     * @param V
     * @return
     */
    static VI getGreatestDegreeIterativeOrder(VVI &V);


    static void test();

private:

    function< bool(Separator&, Separator&) > sepEval;
    VVI V;


};

#endif //ALGORITHMSPROJECT_REMOVALORDERCREATOR_H
