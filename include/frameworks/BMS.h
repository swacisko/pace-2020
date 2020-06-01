//
// Created by sylwester on 8/7/19.
//

#ifndef ALGORITHMSPROJECT_BMS_H
#define ALGORITHMSPROJECT_BMS_H


#include "Makros.h"
#include "Solution.h"
#include "StochasticApproximation.h"
#include "ForbiddenStates.h"

/**
 * Best From Multiple framework. This framework can be applied for any problem, that makes local search operations via adding/removing elements to/from current solution.
 *
 * In each iteration there are @setsPerIteration subsets chosen of the specified set of choosable elements, each set of size @elementsInSet. To given state there will be added/removed
 * elements from those sets. The set that after application gives solution of best score will be chosen as the best from multiple.
 *
 * Class contains also a set of forbidden states - a state cannot be revisited if it is in history. Size of that history can be set as a parametr, by default is 0.
 */
class BMS : public StochasticApproximation {
public:

    BMS();

    /**
     * Virtual destructor neccessary for all abstract classes
     */
    virtual ~BMS(){}

    /**
     * In each iteration this number of subsets of {0,...,choosableElementsNumber-1} of size elementsInSet will be chosen to apply.
     * @return
     */
    int getSetsChosenPerIteration() const;
    /**
     * In each iteration this number of subsets of {0,...,choosableElementsNumber-1} of size elementsInSet will be chosen to apply.
     * @param setsChosenPerIteration
     */
    void setSetsChosenPerIteration(int setsChosenPerIteration);

    /**
     * In each iteration a number of subsets of {0,...,choosableElementsNumber-1} of size @{elementsInSet} will be chosen to apply.
     * @return
     */
    int getElementsInSet() const;
    /**
     * In each iteration a number of subsets of {0,...,choosableElementsNumber-1} of size @{elementsInSet} will be chosen to apply.
     * @param elementsInSet
     */
    void setElementsInSet(int elementsInSet);

    /**
     * This is the size of the universe. In each iteration a random subset of a permutation of
     * @return
     */
    int getChoosableElementsNumber() const;
    void setChoosableElementsNumber(int choosableElementsNumber);

    /**
     *
     * @return forbiddenStates object - we need to modify it to enable forbidden states
     */
    ForbiddenStates &getForbiddenStates();





protected:

    /**
     * Number of elements that form a single set. Look at class description for details.
     * By default it is 1.
     */
    int setsChosenPerIteration;

    /**
     * Number of elements in each of the set returned in each step of iteration.
     */
    int elementsInSet;

    /**
     * Maximal number of choosable elements. A random subset of {0 to choosableElementsNumber-1} will be returned in each step of each iteration.
     */
    int choosableElementsNumber;

    /**
     * Function returns a single random subset of size @{elementsInSet} from @{choosableElements}. Subset contains random integers from {0 to choosableElementsNumber-1}
     * @param choosableElements set of alements to choose from
     * @return random subset of size @{elementsInSet} from @{choosableElements}.
     */
    vector<int> getSetInIteration();

    /**
     * Function adds/removes all elements from @randomSubset to @currentSolution and returns new solution.
     * This functtion needs to be implemented for specific needs.
     * @param randomSubset
     * @param currentSolution
     * @return solution after adding/removing elements from @randomSubset
     */
    virtual Solution* applySetToSolution( vector<int> & randomSubset ) = 0;



    void nextIteration() override;


    /**
     * Forbidden states - list of configurations that were recently visited.
     */
    ForbiddenStates forbiddenStates;
public:


};


#endif //ALGORITHMSPROJECT_BMS_H
