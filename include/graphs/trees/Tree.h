/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_TREE_H
#define ALGORITHMSPROJECT_TREE_H

#include "Makros.h"
#include <graphs/generators/GraphGenerator.h>

class Tree {
public:

    Tree( VVI & V );


    /**
     * Vector holding the structure of the graph if the tree is unweighed.
     */
    VVI *V;

    int getRoot() const;

    void setRoot(int root);

    int getPar() const;

    void setPar(int par);


    /**
     *
     * @param V
     * @return true if given graph represented by @{V} is a tree, false otherwise
     */
    static bool isTree( VVI & V );

    static void test();

protected:

    /**
     * root of the tree - index of the node that is made the root.
     */
    int root;


    /**
     * Parent of the root. This value is different than @{root} if the tree is a subtree. In that case, @{V} points to the original structure
     */
    int par;


};


#endif //ALGORITHMSPROJECT_TREE_H
