//
// Created by sylwester on 3/16/20.
//

#ifndef ALGORITHMSPROJECT_SEPARATORSTATS_H
#define ALGORITHMSPROJECT_SEPARATORSTATS_H

#include <Constants.h>
#include "Makros.h"

class Separator;

class SeparatorStats{

public:

    SeparatorStats(){
        maxCompSize = 0;
        numberOfComponents = 0;
        compsSizeSquares = 0;
        maxCompEdges = 0;
        maxCompDensity = 0;
        maxCompAvgDeg = 0;
        size = 0;
        originalGraphEdges = 0;
        originalGraphSize = 0;

        approximateMaxTreeHeightNode = -1;
        approximateMaxTreeHeightEdge = -1;
    }

    Separator* sep;

    int size;

    int originalGraphEdges;
    int originalGraphSize;

    // size of largest component in graph, after separator is removed
    int maxCompSize;

    /**
     * Maximum number of edges in a component
     */
    int maxCompEdges;

    /**
     * Maximum average degree in a component.
     */
    double maxCompAvgDeg;

    /**
     * Maximum density of a connected component, that is number of edges / size^2
     */
    double maxCompDensity;

    // number of connected components in graph, after separator is removed
    int numberOfComponents;

    // sum of square of component sizes
    long long compsSizeSquares;

    /**
     * maximal value of a treedepth for components of G \ S, calculated by some heuristic, DEFINITELY NOT OPTIMAL.
     */
    double approximateMaxTreeHeightNode;

    double approximateMaxTreeHeightEdge;

    friend ostream& operator<<(ostream& str, SeparatorStats& stats);


};



#endif //ALGORITHMSPROJECT_SEPARATORSTATS_H
