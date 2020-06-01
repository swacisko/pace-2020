//
// Created by sylwester on 3/16/20.
//

#ifndef ALGORITHMSPROJECT_BFSCREATOR_H
#define ALGORITHMSPROJECT_BFSCREATOR_H

#include "../Separator.h"
#include "SeparatorCreator.h"

class BFSSeparatorCreator : public SeparatorCreator{
public:
    BFSSeparatorCreator(VVI & V);

    vector<Separator> createSeparators(VVI& V, int maxSources=1);

    /**
     *
     * @return vector of separator where each separator is just a layer in bfs
     */
    vector<Separator> createLayerSeparators( VI sources );

    /**
     * Each separator returned by this function is a minimal vertex cover of a bipartite graph where nodes in bipartitions are in i-th and (i+1)-th bfs layer.
     * This function creates all separators with basic SeparatorStats in O( E * sqrt(V) ) time, using two-directional DSU propagation.
     * @param sources
     * @return
     */
    vector<Separator> createInterlayerVCSeparators( VI sources );

    /**
     *
     * @param quantities
     * @return vector containing sets of sources. There will be quantities[i] sets with i elements.
     */
    VVI getRandomSources( int N, VI quantities );


    void setSepEval( SeparatorEvaluator eval ){
        isSetSepEval = true;
        sepEval = eval;
    }

private:
    VVI V;

    function<bool(const Separator&,const Separator&)> sepEval;
    bool isSetSepEval = false;
};

#endif //ALGORITHMSPROJECT_BFSCREATOR_H
