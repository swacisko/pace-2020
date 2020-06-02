/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_SNAPTONONPATHNODESMINIMIZER_H
#define ALGORITHMSPROJECT_SNAPTONONPATHNODESMINIMIZER_H

#include "SeparatorMinimizer.h"

class SnapToNonpathNodesMinimizer : public SeparatorMinimizer{
public:

    Separator minimizeSeparator( Separator sep );

    /**
     *
     * @return set of paths in the form S -> v0 -> ... -> vk -> T, where S and T have degree > 2 (or S=T if they are in the same cycle) and vi has degree 2
     */
    static VVI findPaths(VVI& V);

    static void test();
};

#endif //ALGORITHMSPROJECT_SNAPTONONPATHNODESMINIMIZER_H
