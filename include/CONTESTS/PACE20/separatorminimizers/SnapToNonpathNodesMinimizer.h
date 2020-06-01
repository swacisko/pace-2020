//
// Created by sylwester on 4/3/20.
//

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
