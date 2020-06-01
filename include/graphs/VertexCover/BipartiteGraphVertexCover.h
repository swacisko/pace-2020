//
// Created by sylwester on 3/16/20.
//

#ifndef ALGORITHMSPROJECT_BIPARTITEGRAPHVERTEXCOVER_H
#define ALGORITHMSPROJECT_BIPARTITEGRAPHVERTEXCOVER_H

#include "graphs/matching/MaxMatchBipartite.h"

namespace BipartiteGraphVertexCover{


    extern VI getMinVCForBipartition(VVI &G, VI &matching, VB &bipartition);

    extern VI getVertexCoverOfBipartiteGraph(VVI & V, VB & bipartition);
}

#endif //ALGORITHMSPROJECT_BIPARTITEGRAPHVERTEXCOVER_H
