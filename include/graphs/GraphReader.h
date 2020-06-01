//
// Created by sylwester on 8/8/19.
//

#ifndef ALGORITHMSPROJECT_GRAPHREADER_H
#define ALGORITHMSPROJECT_GRAPHREADER_H

#include "Makros.h"

/**
 * Class responsible for reading graphs. It reads graphs from different formats and returns appropriate structure
 */
namespace GraphReader {
    /**
     * Function reads graph from the format
     * N M
     * a1 b1
     * a2 b2
     * ...
     *
     * Edges should be numbered from 1 to N
     * @return graph with its adjacency list structure. CAUTION! Nodes are numbered from 0 to N-1, even though in the input they are from 1 to N
     */
    extern VVI readGraphStandardEdges(istream &cin, bool directed = false);
    /**
     * Reads graph from dimacs format and returns structure. This is basically the same as @readGraphStandardEdges but requires parsing
     * @return structure of the graph
     */
    extern VVI readGraphDIMACSWunweighed(istream &cin, bool edgeFoolowE = false);


};


#endif //ALGORITHMSPROJECT_GRAPHREADER_H
