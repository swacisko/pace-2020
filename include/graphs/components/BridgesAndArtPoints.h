//
// Created by sylwester on 3/20/20.
//

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
