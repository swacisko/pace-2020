/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_SOLUTIONVC_H
#define ALGORITHMSPROJECT_SOLUTIONVC_H

#include "frameworks/Solution.h"

class SolutionVC : public Solution {
public:
    SolutionVC( VI vc );

    SolutionVC( SolutionVC &oth );

    VI& getVC(){ return vc; }
    int size(){ return vc.size(); }

    /**
     * this here is the other way round - a better solution is the one with less nodes. But better solution must be 'greater' than worse via operator<
     * @param oth
     * @return
     */
    bool operator<( Solution * oth ) override;

    /**
     *
     * @param oth
     * @return true if two solutions are equal - if they have a vertex cover of the same size.
     */
    bool operator==( Solution * oth ) override;


    string toString() override;

private:
    VI vc;


};


#endif //ALGORITHMSPROJECT_SOLUTIONVC_H
