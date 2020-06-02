/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_BRIDGESANDARTPOINTS_H
#define ALGORITHMSPROJECT_BRIDGESANDARTPOINTS_H

#include "Makros.h"

namespace BridgesAndArtPoints{

    /**
     * @return res.first is a vector containing articulation points of given graph, res.second contains edges that are bridges in given graph
     */
    pair< VI, VPII > getBridgesAndArtPoints(VVI & V);


};

#endif //ALGORITHMSPROJECT_BRIDGESANDARTPOINTS_H
