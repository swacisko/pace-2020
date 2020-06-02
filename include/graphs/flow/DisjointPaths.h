/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_DISJOINTPATHS_H
#define ALGORITHMSPROJECT_DISJOINTPATHS_H

#include "Makros.h"
#include "Constants.h"


namespace DisjointPaths{


    /**
     * Function finds and returns a maximum set of vertex-disjoint (apart from sources and ends, which can be common) paths from given set of sources to given set of ends.
     * CAUTION! If there is an edge (s,e) in the graph where s is a source and e is an end, the path s->e will also be returned
     * (no no-source-no-end vertices will repeat on paths, sources and ends can repeat)
     * MAKE SURE THAT SOURCES AND ENDS ARE DISJOINT SETS OF NODES
     * @param sources
     * @param ends
     * @param maxPathsBound function terminates work as soon as maxPathsBound number of disjoint paths are reached (or sooner if such limit cannot be reached), If -1, then returns maximum
     * @return
     */
    extern VVI getSetOfDisjointPaths(VVI& V, VI sources, VI ends, int maxPathsBound = -1, bool useUnitFlow = false );



}

#endif //ALGORITHMSPROJECT_DISJOINTPATHS_H
