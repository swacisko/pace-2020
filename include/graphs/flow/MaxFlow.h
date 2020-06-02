/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_MAXFLOW_H
#define ALGORITHMSPROJECT_MAXFLOW_H

#include "Makros.h"
#include "Constants.h"

/**
* CAUTION!!
* CAUTION!!
* CAUTION!!
* CAUTION!!
* V must be a UNDIRECTED STRUCTURE OF THE GRAPH (even if we want to find a flow in the directed graph!!!)
* IF THE GRAPH IS DIRECTED, THEN I ADD MAX FLOW ONLY FOR ONE DIRECTION, FOR THE OTHER SET TO 0. DO NOT CHANGE THE STRUCTURE FOR DIRECTED GRAPH
*
* CAUTION!!
* CAUTION!!*
 *
 *  NOTE THAT THIS IS ALREADY AVOIDED BY CALLING makeBdirectionalStructure() IN CONSTRUCTOR - IT MAKES THE STRUCTURE BIDIRECTIONAL
*/
class MaxFlow {
public:
    typedef int wtype;

    // if dataset is small, that is if it admits full VxV matrices, then use VVWT as such
    typedef vector<wtype> VWT;
//    typedef vector<VWT> VVWT;

    //if dataset is large, then we have to use vector< unordered_map<int,wtype> > instead due to memory limitations
    typedef vector< unordered_map<int,wtype> > VVWT;

    /**
     * CAUTION!!
     * * CAUTION!!
     * CAUTION!!
     * CAUTION!!
     * CAUTION!!
     * IF THE GRAPH IS DIRECTED, THEN I ADD MAX FLOW ONLY FOR ONE DIRECTION, FOR THE OTHER SET TO 0. DO NOT CHANGE THE STRUCTURE FOR DIRECTED GRAPH
     * @param structure structure of the graph. This must be a UNDIRECTED STRUCTURE OF THE GRAPH (even if we want to find a flow in the directed graph!!!
     * @param weights vector of maximum flows for each edges. IF THE GRAPH IS DIRECTED, THEN I ADD MAX FLOW ONLY FOR ONE DIRECTION, FOR THE OTHER SET TO 0. DO NOT CHANGE THE STRUCTURE FOR DIRECTED GRAPH
     */
    MaxFlow( VVI & structure, VVWT & weights );


    /**
     *
     * @return saturation of the edges in the found maximum flow. Returned vector is just @kraw vector
     */
    VVWT getEdgeSaturation(){ return kraw; }


    /**
     * Function
     * @param beg source of the flow
     * @param end the end node of the flow
     * @param maxFlowBound iterations of creating maximum flow will be done until either there is no greater flow, or maxFlowBound has been reached( or perhaps exceeded). This allows us to check quickly whether max flow exceeds some small value
     * @return value of max flow from the source, to the end
     */
    wtype calculateMaxFlow(int beg, int end, wtype maxFlowBound = -1);

    /**
     *
     * @return value of the max flow
     */
    wtype getMAX_FLOW() const;

private:

    wtype MAX_FLOW;


    VVWT kraw; // wektor nasycenia danej krawedzi
    VVWT W; // wagi w tej macierzy( o wymiarach SIZE(V) x SIZE(V) ) nalezy uzupelnic przed wywoalaniem funkcji MaxFlow
    VVI V; // ten wektor nalezy uzupelnic przed wywolaniem MaxFlow - to jest Graf, dla ktorego chcemy znalezc przeplyw

    VB canReach; // wektor uzywany przy blockFlowDfs, do sprawdzania czy z danego wierzcholka da sie dojsc do ujscia, sluzy tylko w celu (wzglednej) optymalizacji czasowej

    VVI V2; // tu jest przechowywany graf warstwowy...
//    VVWT W2;

    /**
     *
     * @param v
     * @return transopsed graph of v
     */
    VVI transpose( VVI & v );

    void makeBdirectionalStructure();

    /**
     * BFS wyznaczajacy graf warstwowy. Jest to graf skierowany, przechowywany w V2 i W2
     * @param beg
     * @param end
     * @return
     */
    bool LayerBFS( int beg, int end );

    /**
     * wyznaczanie przeplywu blokujacego, minC to minimalny koszt krawedzi na danej sciezce, czyli w grafie warstwowym V2
     * minC to minimalny koszt krawedzi podczas szukania sciezki, ale dla ojca wierzcholka num, czyli jesli np num = 1 a minC = 7, to znaczy ze na pewnej sciezce na ktorej jest num
     * znajduje sie krawedz(przed num), ktorej waga jest rowna 7, a pozostale krawedzi przed num maja nie mniejsze wagi
     * @param num
     * @param end
     * @param minC
     * @param ile
     * @return
     */
    bool blockFlowDfs( int num, int end, wtype minC, wtype & ile );

};


#endif //ALGORITHMSPROJECT_MAXFLOW_H
