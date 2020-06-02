/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/


#include <utils/TimeMeasurer.h>

#include "utils/TimeMeasurer.h"

TimeMeasurer::TimeMeasurer() {
}

TimeMeasurer::TimeMeasurer(const TimeMeasurer& orig) {
}

TimeMeasurer::~TimeMeasurer() {
}

void TimeMeasurer::stopMeasurement(string option) {
//    if( Params::WRITE_STATISTICS == false ) return;

    if( times.find(option) == times.end() ) times[option] = clock();
    else{
        timesTotal[option] += ( clock() - times[option] );
        clearOption(option);
    }
}


void TimeMeasurer::startMeasurement(string option) {
//    if( Params::WRITE_STATISTICS == false ) return;
    times[option] = clock();
}

float TimeMeasurer::getMeasurementTimeInSeconds(string option) {
    if( timesTotal.find(option) == timesTotal.end() ) return -1;
    else return ( ( (double)timesTotal[option] / (double)CLOCKS_PER_SEC ) );
}

map<string, float> TimeMeasurer::getAllMeasurements() {
    map<string,float> res;
    for( auto a : timesTotal ){
        res[a.first] = getMeasurementTimeInSeconds(a.first);
    }
    return res;
}

void TimeMeasurer::writeAllMeasurements() {
//    if( Params::WRITE_STATISTICS == false ) return;

    cerr << endl << "TIME MEASUREMENTS:" << endl;
    auto a = getAllMeasurements();
    cerr << fixed;
    cerr.precision(3);
    for( auto b : a ){
        cerr << b.first << " -> " << b.second << " seconds" << endl;
    }
}

void TimeMeasurer::clearOption(string option) {
    times.erase(option);
//    timesTotal.erase(option);
}

void TimeMeasurer::resetAllOptions() {
    times.clear();
    timesTotal.clear();
}

void TimeMeasurer::resetOption(string option) {
    times.erase(option);
    timesTotal.erase(option);
}

map<string,LL> TimeMeasurer::times;
map<string,LL> TimeMeasurer::timesTotal;