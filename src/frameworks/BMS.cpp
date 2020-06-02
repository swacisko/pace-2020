/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/


#include <utils/RandomNumberGenerators.h>
#include "frameworks/BMS.h"

BMS::BMS() : StochasticApproximation() {

}

void BMS::nextIteration(){

    if(!supressAllLogs) cerr << "\rIteration #" << iterationsDone++ << flush;

    Solution *bestIterationSolution = nullptr;

    for( int k=0; k < setsChosenPerIteration; k++ ){
        vector<int> randomSubset = getSetInIteration();
        Solution *temp = applySetToSolution( randomSubset );

        if( bestIterationSolution == nullptr || bestIterationSolution < temp ){
            delete bestIterationSolution;
            bestIterationSolution = temp;
        }else{
            delete temp;
            temp = nullptr;
        }

    }


    delete currentSolution;
    currentSolution = bestIterationSolution;

}



vector<int> BMS::getSetInIteration(){
    vector<int> res;

    if( choosableElementsNumber < 10 *elementsInSet ){
        VI ind( choosableElementsNumber );
        iota( ALL(ind),0 );
        random_shuffle( ALL(ind) );
        for( int i=0; i < min( (int)ind.size(), elementsInSet ); i++ ){
            int p = ind[i];
            res.push_back(p);
        }
    }else{ // this here is in case when the universe of all choosable elements is HUGE. Then i cannot store it in a vector and hence generate with rand.
        unordered_set<int> zb;

//        std::mt19937_64 rng;
//        std::uniform_int_distribution<int> unif(0,choosableElementsNumber-1);

        UniformIntGenerator gen( 0,choosableElementsNumber-1 );

        while( zb.size() < elementsInSet ){
            int d = gen.rand(); // % choosableElementsNumber;
            zb.insert(d);
        }
        res = VI( ALL(zb) );
    }

    return res;
}

int BMS::getSetsChosenPerIteration() const {
    return setsChosenPerIteration;
}

int BMS::getElementsInSet() const {
    return elementsInSet;
}

int BMS::getChoosableElementsNumber() const {
    return choosableElementsNumber;
}
 ForbiddenStates &BMS::getForbiddenStates() {
    return forbiddenStates;
}

void BMS::setSetsChosenPerIteration(int setsChosenPerIteration) {
    BMS::setsChosenPerIteration = setsChosenPerIteration;
}

void BMS::setElementsInSet(int elementsInSet) {
    BMS::elementsInSet = elementsInSet;
}

void BMS::setChoosableElementsNumber(int choosableElementsNumber) {
    BMS::choosableElementsNumber = choosableElementsNumber;
}




