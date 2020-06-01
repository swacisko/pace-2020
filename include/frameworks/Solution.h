//
// Created by sylwester on 8/7/19.
//

#ifndef ALGORITHMSPROJECT_SOLUTION_H
#define ALGORITHMSPROJECT_SOLUTION_H

#include "Makros.h"

/**
 * This clas represents the solution for give nalgorithm. It needs to be extended to specific needs.
 */
class Solution {
public:

    /**
     *
     * @param oth
     * @return the better solution should be 'bigger' via this operator
     */
    virtual bool operator<( Solution * oth ) = 0;

    /**
     *
     * @param oth
     * @return true if solutions are equal. This will be used to admit 'rotations' in local optimums.
     */
    virtual bool operator==( Solution * oth ) = 0;

    /**
     * Function that writes the solution in a string. It may be used to write the solution as a helper.
     * @return
     */
    virtual string toString(){ return ""; };

    /**
     * Writes given solution to the stream
     * @param str
     * @param sol
     * @return ostream object
     */
    friend ostream& operator<<( ostream & str, Solution & sol ){
        str << sol.toString();
        return str;
    }

};




#endif //ALGORITHMSPROJECT_SOLUTION_H
