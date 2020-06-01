//
// Created by sylwester on 8/13/19.
//

#ifndef ALGORITHMSPROJECT_STRONGLYCONNECTEDCOMPONENTS_H
#define ALGORITHMSPROJECT_STRONGLYCONNECTEDCOMPONENTS_H

#include "Makros.h"

class StronglyConnectedComponents {
public:

    StronglyConnectedComponents( VVI & structure );

    void createStronglyConnectedComponents();

    /**
     *
     * @return vector containing all strongly connected components
     */
    VVI getComponents(){ return Comps; }

    /**
     * A Strongly-Connected-Component graph is a graph that has vertices representing a strongly connected components of graph V.
     * An edge (C1,C2) is in that graph only if there is and edge with ends in those two components.
     *
     * Function creates and returns such graph. If neccessary, it invokes createStronglyConnectedComponents to create strongly connected components first.
     * @return
     */
    VVI getStronglyConnectedComponentGraph();

    /**
     *
     * @return vector compParent with property: compParent[i] is the number d such that component Comps[d] contains node i from V.
     */
    const VI &getCompParent() const;


//private:

    VI PostOrder; // PostOrder[i] - numer post_order i-tego wierzcholka
    VVI V;
    int N;
    VVI Comps; // Copms[i] - wektor w sklad ktorego wchodza wierzcholki i-tej silnie spojnej skladowej

    /**
     * compParent[i] is the number d such that component Comps[d] contains node i from V.
     */
    VI compParent;


    VI temp_comp; // wektor tymczasowy, reprezentujacy jedna spojna skladowa podczas jednej serii rekurencji Add_PO_DFS

    VB was; // czy juz bylem w danym wierzcholku

    void PO_DFS( int num, int & ponum );

    void Add_PO_DFS( int num );

    VVI transpose( VVI & v );



};


#endif //ALGORITHMSPROJECT_STRONGLYCONNECTEDCOMPONENTS_H
