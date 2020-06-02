/*
 * Copyright (c) 2020, Sylwester Swat
*/
#include <graphs/landmarks/LandmarkCreator.h>
#include <graphs/graphtraversals/BFS.h>


VI LandmarkCreator::getLandmarks(VVI &V, int beg, int landmarks, int lastMiddleLandmarks) {
    if( landmarks > V.size()-1 ) landmarks = V.size()-1;
    if( lastMiddleLandmarks > landmarks ) lastMiddleLandmarks = landmarks;

//    VVI layers = BFS::getBfsLayers(V, beg );
//    VI res(1, layers.back()[0] );


    VVI layers;
    VI res(1, beg );

//    DEBUG(beg);
//    DEBUG(res);

    while( res.size() < landmarks - lastMiddleLandmarks ){
        layers = BFS::getBfsLayers( V,res );
        int n = layers.back().size();
        res.push_back( layers.back()[ rand()%n ] );
//        DEBUG(res.back());
    }

    for( int i=0; i<lastMiddleLandmarks; i++ ){
        layers = BFS::getBfsLayers( V,res );
        int l = (layers.size()+1)/2;
        int n = layers[l].size();
        res.push_back( layers[l][ rand()%n ] );
//        DEBUG(res.back());
    }

//    DEBUG(res);

    return res;
}

VI LandmarkCreator::getLandmarks(VVI &V, VVI &W, int beg, int q) {
    cerr << "NOT IMPLEMENTED YET" << endl;
    exit(1);
}

VI LandmarkCreator::getLandmarksAlternateLayers(VVI &V, int beg, int landmarks) {
    if( landmarks > V.size() ) landmarks = V.size();

    VVI layers = BFS::getBfsLayers(V, beg );

//    VI res(1, layers.back()[0] ); // here we do not insert beg node.
    VI res(1, beg);

//    DEBUG(beg);
//    DEBUG(res);


    while( res.size() < landmarks ){
        layers = BFS::getBfsLayers( V,res );

        if( res.size() & 1 ){
            int n = layers.back().size();
            res.push_back( layers.back()[ rand()%n ] );
        }else{
            int l = (layers.size()+1)/2;
            int n = layers[l].size();
            res.push_back( layers[l][ rand()%n ] );
        }
//        DEBUG(res.back());

    }

//    DEBUG(res);

    return res;


}
