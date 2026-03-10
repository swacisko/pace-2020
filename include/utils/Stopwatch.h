//
// Created by sylwester on 3/30/22.
//

#ifndef ALGORITHMSPROJECT_STOPWATCH_H
#define ALGORITHMSPROJECT_STOPWATCH_H


#include "Makros.h"



/**
 * Just a stopwatch - equivalent to TimeMeasurer class, but not necessary to make it static.
 * Use [start] function to start running time for specified option, use [stop] to stop it.
 */
class Stopwatch {
public:
    Stopwatch();

    /**
     * Starts (or restarts if it was started and stopped) measuring time of give [option].
     * If the option was already started and was not stopped, then nothing is done (and the previous start time is
     * valid).
     * @param option
     */
    void start(string option);
    void restart(string option){stop(option); reset(option); start(option);}

    /**
     *  Stops measuring time for given [option]. If the option was not started, nothing happens.
     */
    void stop( string option );

    /**
     * Start the option, if it is not running already or stops the option if it is already running.
     */
    void startOrStop(string option);


    /**
     * @return milliseconds measured for the option. It will be the union of all disjoint intervals for which the
     * option was started and stopped plus, perhaps, the time since last start (if it was not stopped).
     */
    double getTime(string option = "main");

    /**
     * @return a map with (option,time) pairs of measurements. Times for given option are in milliseconds.
     */
    map<string,double> getAllMeasurements();

    /**
     * @return a vector with pairs (option,time) of all measurements, sorted in nondecreasing/nonincreasing order
     * of time
     */
    vector<pair<string,double>> getAllMeasurementsSorted( bool nondecreasing = true );

    /**
     * Writes all measurements to [clog]
     */
    void writeAll();

    /**
     * Writes time that was taken by given option
     */
    void write(string option);

    /**
     * Resets all options - clears the whole time measurer
     */
    void resetAllOptions();

    /**
     * Removes given option completely from history from all tables.
     */
    void reset(string option);

    /**
     * Returns the average time, that is the value getTime(option) / (1000*tests);
     */
    double getAvgTimeInSeconds( string option, int tests ){ return 1.0 * getTime(option) / (1000*tests); }


//    ***************************************************************** TLE options

    /**
     * Sets time limit for given option in milliseconds. This may be set to later check tle(option) if the time
     * limit for this option was exceeded,
     */
    void setLimit(string option, double limit );

    /**
     * @return limit that was set for given option, or 1e9 if not limit was set
     */
    double getLimit(string option);

    /**
     * @return true if time limit specified in [limits] was exceeded, false otherwise. If given option was given a time
     * limit earlier, then false is returned.
     */
    bool tle(string option = "main");

    /**
     * Resets time limit for given option (no limit for the option will be set after this, it basically equals
     * to setting the limit to infinity).
     */
    void resetLimit( string option );

private:

    /**
     * limits[option] is the time limit of given option. It must be set by [setLimit] option and can be used
     * to check for TLE using [tle] function.
     */
    map<string,double> limits;

    /**
     * times[option] is the timestamp of the started, but not yet finished option.
     */
    map<string,std::chrono::time_point<std::chrono::steady_clock> > times;

    /**
     * timesTotal[option] is the total time measured for all FINISHED measurements (start() and stop() must have been
     * called for the option to make it appear in [timesTotal]).
     */
    map<string,LL> timesTotal;
};

#endif //ALGORITHMSPROJECT_STOPWATCH_H
