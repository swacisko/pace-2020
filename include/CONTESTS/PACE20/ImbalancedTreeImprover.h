//
// Created by sylwester on 4/24/20.
//

#ifndef ALGORITHMSPROJECT_IMBALANCEDTREEIMPROVER_H
#define ALGORITHMSPROJECT_IMBALANCEDTREEIMPROVER_H

#include "DepthTree.h"

class ImbalancedTreeImprover{
public:

    /**
     *
     * @param dt
     * @return
     */
    DepthTree improve( DepthTree &t );

    /**
     *
     * @param dt
     * @return true if given depth tree is imbalanced, false otherwise
     */
    static bool isImbalanced( DepthTree& dt );


    static void test();

    /**
     * In this case the root separator is made a clique, all small subtrees are removed and largest subtree with the 'closed' separator clique is again subjected
     * to DepthTreeCreator
     */
    static const int SEPARATOR_CLIQUE = 1;

    int IMPROVEMENT_MODE = 1;

private:
    DepthTree* dt;

};

#endif //ALGORITHMSPROJECT_IMBALANCEDTREEIMPROVER_H
