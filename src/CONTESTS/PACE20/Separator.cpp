/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <CONTESTS/PACE20/Separator.h>
#include "graphs/components/ConnectedComponents.h"
#include "graphs/GraphUtils.h"

Separator::Separator( VVI & V, VI nodes ){
    this->V = &V;
    this->nodes = nodes;
    stats.sep = this;
}

ostream& operator<<(ostream& str, Separator& sep){
//    str << "[[ V.size: " << sep.V->size() << ", " << sep.stats << " ]]";
//    str << "[[ " << sep.nodes << "  |  " << sep.stats << " ]]";
    str << "[[ " << sep.stats << " ]]";
    return str;
}

void Separator::createSeparatorStats() {

    VVI comps = ConnectedComponents::getConnectedComponents(*V,nodes);

    stats.numberOfComponents = comps.size();
    stats.size = nodes.size();
    stats.originalGraphEdges = GraphUtils::countEdges(*V);
    stats.originalGraphSize = V->size();

    VB zb(V->size(),false);

    for(VI & v : comps){
//        stats.compsSizeSquares += 1ll * v.size() * v.size();
        stats.maxCompSize = max( stats.maxCompSize, (int)v.size() );


//        double compAvgDeg = 0;
        int compAvgDeg = 0;
        for( int d : v ) zb[d] = true;

        for(int d : v){
            for( int j=0; j<(*V)[d].size(); j++ ){
                if( zb[ (*V)[d][j] ] ) compAvgDeg++;
            }
        }

        for( int d : v ) zb[d] = false;

//        stats.maxCompEdges = max( stats.maxCompEdges, (int)(compAvgDeg/2) );
        stats.maxCompEdges = max( stats.maxCompEdges, compAvgDeg >> 1 );

        /*compAvgDeg /= v.size();
        stats.maxCompAvgDeg = max( stats.maxCompAvgDeg, compAvgDeg );

        compAvgDeg /= max(1, (int)v.size()-1 );
        stats.maxCompDensity = max( stats.maxCompDensity, compAvgDeg );*/
    }

}

Separator &Separator::operator=(const Separator &oth) {
    this->nodes = oth.nodes;
    this->stats = oth.stats;
    this->stats.sep = this;
    this->V = oth.V;


    return *this;
}

void Separator::updatePointers(VVI &V) {
    this->V = &V;
    stats.sep = this;
}
