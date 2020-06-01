//
// Created by sylwester on 8/26/19.
//

#ifndef ALGORITHMSPROJECT_BFS_H
#define ALGORITHMSPROJECT_BFS_H

#include <Constants.h>
#include "Makros.h"

namespace BFS{


    /**
     * Function performs BFS search
     * @param V structure of the graph
     * @param dist dist[i] will contain the distance from node @{beg} to node i in 'bfs layers'.
     * @param beg beginning node of the search
     * @param haltCondition function that should return true after visiting a node
     * @return true if at any moment @{haltCondition} returned true. This may be used to terminate as soon as some solution is found.
     */
    extern bool bfs( VVI & V, VI & dist, int beg, function< bool (int num) > haltCondition = [](int a){return false;}, function< void (int num) > inorderFunction = [](int a){} );


    extern VVI getBfsLayers(VVI & V, VI sources);

    extern VVI getBfsLayers(VVI & V, int source);



}

/*class BFS {

};*/


#endif //ALGORITHMSPROJECT_BFS_H
