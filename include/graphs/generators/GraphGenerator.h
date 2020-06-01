//
// Created by sylwester on 8/8/19.
//

#ifndef ALGORITHMSPROJECT_GRAPHGENERATOR_H
#define ALGORITHMSPROJECT_GRAPHGENERATOR_H

#include "Makros.h"

class GraphGenerator {

public:

    /**
     * Creates graph G(N,M)
     * @param N
     * @param M
     * @param directed if true, then a directed graph is created, otherwise undirected
     * @return random graph with N vertices and M edges.
     */
    static VVI getRandomGraph(int N, int M, bool directed = false);

    /**
     * Creates random graph G(N,p)
     * @param N
     * @param p
     * @return random graph with N vertices, in which every edge hash probability p of beiing in that graph
     */
    static VVI getRandomGraph(int N, double p, bool directed = false);

    /**
     *
     * @param L
     * @param R
     * @param M
     * @return random bipartitie graph with M edges, where the first L vertices are in first part, and last R are in second part of the biartition
     */
    static VVI getRandomBipartiteGraph( int L, int R, int M );

    /**
     *
     * @param L
     * @param R
     * @param p
     * @param directed
     * @return random bipartitie graph in which each edge is with probability p, where the first L vertices are in first part, and last R are in second part of the biartition
     */
    static VVI getRandomBipartiteGraph( int L, int R, double p );

    /**
     * Creates and returns random grid with rows*columns nodes. Node in row r and column c will have id = r*rows + c
     * @param rows
     * @param columns
     * @return random grid with @{rows} rows and @{columns} columns.
     */
    static VVI getRandomGrid( int rows, int columns );


    /**
     * Function generates random tree. In first step it generates random prufer code, then creates tree represented by that code.
     * @param N
     * @return random tree.
     */
    static VVI getRandomTreePrufer(int N);

    /**
     * Function craetes and returns path on N vertices
     * @param N
     * @return path on N vertices
     */
    static VVI getPath(int N, bool randomOrder = true);

};


#endif //ALGORITHMSPROJECT_GRAPHGENERATOR_H
