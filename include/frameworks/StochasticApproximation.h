//
// Created by sylwester on 8/7/19.
//

#ifndef ALGORITHMSPROJECT_STOCHASTICAPPROXIMATION_H
#define ALGORITHMSPROJECT_STOCHASTICAPPROXIMATION_H

#include "Makros.h"
#include "Solution.h"
#include "Constants.h"

/**
 * This is the framework for approximation algorithms that use stochastic approach - usually run forever with the hope of finding a good solution. E.g. BMS (best from multiple)
 * is a StochasticApproimation instance.
 *
 * There is given @currentSolution that will be modified infinitely/specified_number_of_iterations/specified_amount_of_time. The best solution will be remembered.
 *
 *
 * !!!  A Solution instance needs to have overloaded operator <. StochasticApproximation will try to maximize the element, that is if A < B, then B is chosen as better.
 *
 */
class StochasticApproximation {
public:

   StochasticApproximation();

   /**
    * Virtual constructor needed for abstract class.
    */
   virtual ~StochasticApproximation(){}

    Solution *getBestSolution() const;

    /**
     *
     * @return maximal run time of the algorithm
     */
    long long getMaxRunTime() const {
        return maxRunTime;
    }

    /**
     * Sets @maxRunTime variable to given value. This is measured in miliseconds.
     * @param maxRunTime miliseconds after which algorithm will stop.
     */
    void setMaxRunTime(long long maxRunTime) {
        StochasticApproximation::maxRunTime = maxRunTime;
    }

    long long getMaxIterations() const;

    void setMaxIterations(long long maxIterations);

    /**
     * Start running
     */
    virtual void run();

    /**
     *
     * @return time in miliseconds that was spent in the @{run} function.
     */
    long long getExecutionTime();

    /**
     *
     * @return time after which the best solution was discovered for the first time.
     */
    long long getBestSolutionDiscoveryTime();

    /**
     *
     * @return number of iterations done in function @{run}.
     */
    long long int getIterationsDone() const;

    /**
     * Sets current solution to initial solution. If sol != nullptr, then getInitialSolution() will not be invoked
     * @param sol
     */
    void setInitialSolution( Solution * sol ){ currentSolution = sol; }

    /**
     *
     * @return true if logs are suppressed, false otherwise
     */
    bool isSupressAllLogs() const;

    /**
     * if true, then no logs will be displayed
     * @param supressAllLogs
     */
    void setSupressAllLogs(bool supressAllLogs);


    bool isTakeFirstBestSolution() const;

    /**
     * If true, then i will modify best solution only if a new one is strictly better. Otherwise i will modify a solution if a new one is equal to the best one.
     * In other words, if true, then i will return the first best solution found.
     * If false, then i will return the last best solution found.
     * @param takeFirstBestSolution
     */
    void setTakeFirstBestSolution(bool takeFirstBestSolution);

protected:
    /**
     * This is the beginning time of the run of the algorithm (in miliseconds). It may be used to set run time for certain amount of time.
     */
    std::chrono::time_point<std::chrono::system_clock> beginTime;
    std::chrono::time_point<std::chrono::system_clock> endTime;


    /**
     * This is the maximum running time set for the algorithm (in miliseconds). By default it is Makros::INF.
     */
    long long maxRunTime;

    /**
     * This function may be used
     * @return time from the beginning of the running of the algorithm (in miliseconds).
     */
    long long getTimeFromBegin();

    /**
     * This function will be checked after each iteration. If returns true, then algorithm terminates. So the actual run time will be at least @max_run_time.
     * @return true if the maximum run time of the algorithm is already exceeded, false otherwise
     */
    bool runTimeExceeded();

    std::chrono::time_point<std::chrono::system_clock> bestSolutionDiscoveryTime;

    /**
     * Function used to remember the time at which the best solution was discovered.
     * This function should be invoked each time a new solution was found - it needs to be implemented in @updateBestSolution function.
     */
    void newBestSolutionFound();

    /**
     * Maximal number of iterations that will be performed during each @run() function.
     */
    long long maxIterations;

    /**
     * Number of iterations that were done in @run method.
     */
    long long iterationsDone;


    /**
     * This function needs to be implemented for specific needs.
     * @return initial solution that will be subjected to futher improvements.
     */
    virtual Solution* getInitialSolution() = 0;

    /**
     * This function is responsible for initializing all data before @{run} method is invoked.
     * It is neccessary, since a solution can be set with setInitialSolution() instead of getInitialSolution().
     */
    virtual void makeInitialWork() = 0;

    /**
     * Function performs the next iteration. Most probably @currentSolution should be modified.
     */
    virtual void nextIteration() = 0;

    /**
     * Function checks whether @sol is better than @bestSolution ( better means bestSolution < sol  via overloaded operator <). If so, then bestSolution should updated.
     * However bestSolution should be deleted and a NEW INSTANCE os Solution (equal to sol) should be stored in @bestSOlution. We cannot store @currentSolution, because it may
     * be modified in next iteration, and that modifications would change the structure of @bestSolution.
     * @param sol
     * @return function should return true if better solution was found, false otherwise
     */
    virtual bool updateBestSolution( Solution * sol ) = 0;

    /**
     * Best solution found so far by the algorithm.
     */
    Solution *bestSolution;

    /**
     * @currentSolution is the state of the 'search'. It is modified in each iteration.
     */
    Solution *currentSolution;

    /**
     * If true, then i will not write any logs to cerr.
     */
    bool supressAllLogs;

    /**
     * If true, then i will modify best solution only if a new one is strictly better. Otherwise i will modify a solution if a new one is equal to the best one.
     * In other words, if true, then i will return the first best solution found.
     * If false, then i will return the last best solution found.
     */
    bool takeFirstBestSolution = false;



};


#endif //ALGORITHMSPROJECT_STOCHASTICAPPROXIMATION_H
