//
// Created by sylwester on 8/27/19.
//

#ifndef ALGORITHMSPROJECT_FINDANDUNION_H
#define ALGORITHMSPROJECT_FINDANDUNION_H

#include "Makros.h"


class FAU {
public:

    FAU( int n );
    ~FAU();

    /**
     *
     * @param x
     * @return id of element representing set containing x
     */
    int Find(int x);

    /**
     *
     * @param p
     * @return number of elements in the set containing p
     */
    int countNodesInComponent( int p );

    /**
     * Merges two sets: containing x and containing y
     * @param x
     * @param y
     */
    void Union( int x, int y );

    int* getP(){return p;}
    int* getW(){return w;}


private:
    int *p,*w;
};


#endif //ALGORITHMSPROJECT_FINDANDUNION_H
