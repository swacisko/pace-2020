/*
 * Copyright (c) 2020, Sylwester Swat
*/

#include <frameworks/StochasticApproximation.h>

#include "frameworks/StochasticApproximation.h"

StochasticApproximation::StochasticApproximation() : maxRunTime( Constants::INF_LL ), maxIterations( Constants::INF_LL ) {
    supressAllLogs = false;
    currentSolution = nullptr;
    bestSolution = nullptr;
}

Solution *StochasticApproximation::getBestSolution() const {
    return bestSolution;
}

void StochasticApproximation::run() {
    beginTime = std::chrono::system_clock::now();

    if( currentSolution == nullptr ) currentSolution = getInitialSolution();
    updateBestSolution( currentSolution );

    makeInitialWork();

//    cerr << "currentSolution:" << endl << *currentSolution << endl;

    iterationsDone = 0;
    while(true){
        iterationsDone++;
        if( !supressAllLogs ) cerr << "\riteration #" << iterationsDone << "\t" << flush;

        nextIteration();
        if( updateBestSolution( currentSolution ) ){
            newBestSolutionFound();
        }



        if( runTimeExceeded() ) break;
        if( iterationsDone >= maxIterations ) break;
    }

    endTime = std::chrono::system_clock::now();
}

long long StochasticApproximation::getMaxIterations() const {
    return maxIterations;
}

void StochasticApproximation::setMaxIterations(long long maxIterations) {
    StochasticApproximation::maxIterations = maxIterations;
}

bool StochasticApproximation::runTimeExceeded() {
    return getTimeFromBegin() > maxRunTime;
}

long long StochasticApproximation::getTimeFromBegin() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - beginTime;

    return (long long) (1000 * elapsed_seconds.count());
}

long long StochasticApproximation::getExecutionTime() {
    std::chrono::duration<double> elapsed_seconds = endTime - beginTime;
    return (long long) (1000 * elapsed_seconds.count());
}

void StochasticApproximation::newBestSolutionFound() {
    bestSolutionDiscoveryTime = std::chrono::system_clock::now();
}

long long StochasticApproximation::getBestSolutionDiscoveryTime() {
    std::chrono::duration<double> elapsed_seconds = bestSolutionDiscoveryTime - beginTime;
    return (long long) (1000 * elapsed_seconds.count());;
}

long long int StochasticApproximation::getIterationsDone() const {
    return iterationsDone;
}

bool StochasticApproximation::isSupressAllLogs() const {
    return supressAllLogs;
}

void StochasticApproximation::setSupressAllLogs(bool supressAllLogs) {
    StochasticApproximation::supressAllLogs = supressAllLogs;
}

bool StochasticApproximation::isTakeFirstBestSolution() const {
    return takeFirstBestSolution;
}

void StochasticApproximation::setTakeFirstBestSolution(bool takeFirstBestSolution) {
    StochasticApproximation::takeFirstBestSolution = takeFirstBestSolution;
}

