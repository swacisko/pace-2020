//
// Created by sylwester on 10/4/19.
//

#ifndef ALGORITHMSPROJECT_DECISIONPROBLEM_H
#define ALGORITHMSPROJECT_DECISIONPROBLEM_H

#include "Solution.h"

class DecisionProblem{
public:
    virtual bool existsSolutionOfSize(int K) = 0;

    Solution* getSolution(){return solution;}

protected:
    Solution *solution;

};

#endif //ALGORITHMSPROJECT_DECISIONPROBLEM_H
