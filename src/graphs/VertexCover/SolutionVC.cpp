/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#include <graphs/VertexCover/SolutionVC.h>


SolutionVC::SolutionVC( VI vc ){
    this->vc = vc;
}

SolutionVC::SolutionVC(SolutionVC &oth) {
    this->vc = oth.vc;
}

bool SolutionVC::operator<(Solution *oth) {
//    cerr << "comparing solutions" << endl;

    SolutionVC* sol = (SolutionVC*) oth;
    if( vc.size() <= sol->vc.size() ) return false;
    else return true;

}

string SolutionVC::toString() {
    ostringstream s;
    s << vc;
    return s.str();
}

bool SolutionVC::operator==(Solution *oth) {
    SolutionVC* sol = (SolutionVC*) oth;
    return sol->vc.size() == vc.size();
}

