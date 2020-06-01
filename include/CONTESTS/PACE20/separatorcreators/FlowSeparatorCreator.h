//
// Created by sylwester on 3/22/20.
//

#ifndef ALGORITHMSPROJECT_FLOWSEPARATORCREATOR_H
#define ALGORITHMSPROJECT_FLOWSEPARATORCREATOR_H

#include <CONTESTS/PACE20/separatorminimizers/SeparatorMinimizer.h>
#include "../Separator.h"
#include "SeparatorCreator.h"

class FlowSeparatorCreator : public SeparatorCreator{


public:
    FlowSeparatorCreator( SeparatorMinimizer* minimizer = nullptr );


    vector<Separator> createSeparators(VVI& V, int reapeats = 1);

    /**
     * Function finds maximum number of vertex-disjoint paths (apart from sources and ends) from sources to ends.
     * Then vertices in those paths are taken as a separator and that separator is minimized using some separator minimizer.
     * @param V
     * @param sources
     * @param ends
     * @return
     */
    Separator getSeparatorForFlow( VVI& V, VI sources, VI ends );

    /**
     * Almost the same as above, but as sources we take all vertices that are at distance at most @{dist} from any source node, and as ends we take all vertices that
     * are at distance at most @{dist}
     * @param V
     * @param sources
     * @param ends
     * @param dist
     * @return
     */
    Separator getSeparatorForFlow(VVI &V, VI sources, VI ends, int dist);



    static void test();


    SeparatorMinimizer* minimizer;

};

#endif //ALGORITHMSPROJECT_FLOWSEPARATORCREATOR_H
