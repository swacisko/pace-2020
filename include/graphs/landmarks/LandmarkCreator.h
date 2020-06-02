/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_LANDMARKCREATOR_H
#define ALGORITHMSPROJECT_LANDMARKCREATOR_H

#include "Makros.h"

class LandmarkCreator{

public:

    /**
     * Functin creates and returns a set of landmarks nodes from unweighed graph V selected as follows:
     * 1. Select node that is furthest from beg (random one if there are many) and add it to res.
     * 2. while res.size() < landmarks - lastMiddleLandmarks
     *      select node that is furthest from the set res and add it to res.
     * 3. lastMiddleLandmarks we select from middle layers instead of the last
     *
     *
     *
     * the third step allows us to 'minimize distances' in some graphs. e.g. in full binary trees of heght h, we have 2^h leaves. Then if landmarks < 2^(h/2),
     * all landmarks would be in a leaf. By the third step, we ensure that the last landmark is placed nearer the root of the tree.
     *
     * @param V
     * @param beg
     * @return
     */
    VI getLandmarks(VVI &V, int beg, int landmarks, int lastMiddleLandmarks = 0);

    /**
     * Creates landmarks just as the getLandmarks function, but every second landmarks is selected randomly from middle bfs layer instead of furthest layer
     * @param V
     * @param beg
     * @param landmarks
     * @return
     */
    VI getLandmarksAlternateLayers( VVI & V, int beg, int landmarks );


    VI getLandmarks(VVI& V, VVI& W, int beg, int q );


};

#endif //ALGORITHMSPROJECT_LANDMARKCREATOR_H
