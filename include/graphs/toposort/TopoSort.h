/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_TOPOSORT_H
#define ALGORITHMSPROJECT_TOPOSORT_H

#include "Makros.h"

class TopoSort {
public:

    TopoSort( VVI & structure );


    /**
     * funkcja nie zmienia kolejnosci wierzcholkow w kolejnosci topologicznej, tylko ustawia w wektorze kol ich odpowiednia kolejnosc
     * zmienic caly wektor trzeba to jeszcze przenumerowac
     * dziala w czasie liniowym, bo jest to DFS, ale nie jest to posortowane minimalnie leksykograficznie (do tego potrzeba NlgN)
     * @return nodes in the topological order
     */

    VI sortTopologically();


    /**
     *
     * @return the same as the last function that returned a topological order. (so far there is only one function sortTopologically that returns topological order).
     */
    VI getTopologicalOrder(){ return kol; }

    /**
     *
     * @return nodes on longest path in the graph.
     */
    VI getLongestPath();

private :



    int N;
    VVI V;
    VI deg; // ilosc ojcow danego wierzcholka w grafie, jesli ma 0 to mozna go w danym momencie dodac do listy topologicznej
    VI kol; // kol[i] - nr wierzcholka ktory jest i-ty w kolejnosci topologicznej
    VB was; // zapisuje czy juz bylem w danym wierzcholku podczas topologicznego dfsa

    void DFS( int num );

    /**
     * Function responsible for topological sort.
     */
    void topoSort();


};


#endif //ALGORITHMSPROJECT_TOPOSORT_H
