//
// Created by sylwester on 8/26/19.
//

#ifndef ALGORITHMSPROJECT_DFS_H
#define ALGORITHMSPROJECT_DFS_H

#include "Makros.h"

namespace DFS{


    /**
     * Function performs DFS with beginning in @{num}.
     * @param V
     * @param was array holding values 1 if given node was visited
     * @param num
     * @param par parent of the node @{num}. Invocation should be done with par = num
     * @param edgeFunction function that will be called for each edge in the graph exactly once - for pair (num,par) = (a,b) where visitOrder[a] < visitOrder[b]
     * @param preorderFunction function called when a node is entered
     * @param inorderFunction function invoked BETWEEN calling dfs for its sons. Hence it will be called X-1 times, where X is the number of sons of @{num} in that dfs search.
     * @return true if the dfs search is about to halt in given moment. CAUTION! This will not invoke some functions, but may be used to terminate work as soon as the solution is found.
     */
    extern bool dfs( VVI & V, VB & was, VI & visitOrder, int num, int par, function< void (int num, int par) > edgeFunction = [](int a, int b){},
            function<void (int num) > preorderFunction = [](int a){}, function< void (int num)  > inorderFunction = [](int a){},
            function< void (int num)  > postorderFunction =  [](int a){}  );







}



//class DFS {
//
//};


#endif //ALGORITHMSPROJECT_DFS_H
