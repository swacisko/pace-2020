/*
 * Copyright (c) 2020, Sylwester Swat
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

