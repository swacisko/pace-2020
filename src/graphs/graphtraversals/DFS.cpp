//
// Created by sylwester on 8/26/19.
//

#include "graphs/graphtraversals/DFS.h"


namespace DFS{


    bool dfs( VVI & V, VB & was, VI & visitOrder, int num, int par, function< void (int num, int par) > edgeFunction,
              function<void (int num) > preorderFunction, function< void (int num)  > inorderFunction,
              function< void (int num)  > postorderFunction   ){

        if( was[num] ) return false;
        was[num] = true;

        if( num == par || par == -1 ) visitOrder[num] = 0;
        else visitOrder[num] = visitOrder[par]+1;

        preorderFunction(num);

        for( int i=0; i < V[num].size(); i++ ){
            int d = V[num][i];

            if( !was[d] ){
                edgeFunction(num,d);
                bool terminate = dfs( V,was,visitOrder,d,num, edgeFunction, preorderFunction, inorderFunction, postorderFunction );
                if( terminate ) return true;

            }else if( visitOrder[d] > visitOrder[num] ){
                edgeFunction(num,d);
            }

            if( i < V[num].size()-1  ){
                inorderFunction(d);
            }

        }

        postorderFunction(num);

        return false;
    }



}