/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_BIPARTITEGRAPHVERTEXCOVER_H
#define ALGORITHMSPROJECT_BIPARTITEGRAPHVERTEXCOVER_H

#include "graphs/matching/MaxMatchBipartite.h"

namespace BipartiteGraphVertexCover{


    extern VI getMinVCForBipartition(VVI &G, VI &matching, VB &bipartition);

    extern VI getVertexCoverOfBipartiteGraph(VVI & V, VB & bipartition);
}

#endif //ALGORITHMSPROJECT_BIPARTITEGRAPHVERTEXCOVER_H
