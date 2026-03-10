/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DEPTHTREECREATOREXACT_H
#define ALGORITHMSPROJECT_DEPTHTREECREATOREXACT_H

#include "DepthTreeCreator.h"

class DepthTreeCreatorExact : public DepthTreeCreator{
public:
    DepthTreeCreatorExact( VVI & V, int k, int recDpth );

    /**
     *
     * @return optimal depth tree for graph V
     */
    DepthTree getDepthTree();

    /**
     * @param k
     * @param dt
     * @return true if a depth tree of height <= k exists for V, false otherwise. If true, then bestTree represents that tree
     */
    bool existsDepthTree();
    bool checkLowerBounds();

    DepthTree& getBestTree(){ return bestTree; }

    static void test();

    VI originalGraphRemapper;

private:

    void logSpacing(){ for(int i=0; i<recDepth; i++) cerr << "  "; }

    bool branch();
    bool branch(VI sep);

    /**
     * This here is used to indicate that when checking DTCL lower bound the tree was found.
     */
    bool bestTreeFound = false;
    DepthTree bestTree;

    /**
     *
     * @return true if V is of a class that admits quick optimal tree creation (tree or clique), false otherwise. f in addition height of
     * the DepthTree of that graph is at most K, then bestTreeFound is set to true, and bestTree represents optimal tree of that
     */
    bool checkGraphClass();

    VI branchingNodes;
    void createBranchingNodes();

    VI artPoints;
    void createArtPointsAndUpdateBranchingPoints();

    VB minimalSubsets; // minimalSubsets[mask] is true if nodes represented by mask form a minimal V-disjoining subset
    void createMinimalSubsets();

    int C; // number of connected components of V \ branchingNodes. That is nodes in componentGraph[i] for i < C represent connected components of V \ branchingNodes
    VVI componentGraph;
    void createComponentGraph();


    int K; // parameter value to look for - looking for elimination tree with height at most K

    vector<DepthTree> dtrees; // we use DTCL lowe bound and hence create a depth tree. We store it in case we will use it for something else sometime later
    void addDtree(DepthTree& d){ dtrees.push_back(d); }

    /**
     * dominationGraph[i] is a set of nodes v such that is dominated by v. This is used due to he rule:
     * If v is dominated by i and v is in branching set P, then i is also in the branching set P. If i was not in P, then either V \ P is connected or P \ v also disconnected V,
     * a contradiction since P is minimal.
     */
    VVI dominationGraph;
    void createDominationGraph();


    void processAllSubsets(int N, int K, function<void(VI &)> fun);




    //******************************************************************************************************** LOWER BOUNDS

    bool boundsChecked = false;

    /**
     *
     * @return false if any of the lower bounds failse, true otherwise
     */

    int getDegreeLowerBound(int s, int deg){ return degreeLowerBound[s][deg]; }
    void preprocessDegreeLowerBound();
    static VVI degreeLowerBound; // degreeLowerBound[i][j] is the lower bound on graph of size i with greatest degree j

    /**
     * Uses DepthTreeCreatorLarge with only ComponentExpansion and no separators minimization to quickly create a lower bound.
     * @return depth tree found for V using quick DTCL
     */
    DepthTree getDTCLLowerBound();

    /**
     * Finds random spanning tree of V, then returns its diameter as the longest path. Repeats the procedure 3 times.
     * @return false if found lowe bound is greater than K, true otherwise
     */
    bool getPathLowerBound();

    //******************************************************** FORBIDDEN STATES

    static VLL origHashes;
    static unordered_map<LL,int> fS;
    static queue<LL> fSQueue;
    static void createOriginalGraphHashes();
    static const int MAX_FS_SIZE = 50'000'000;

    bool isForbiddenGraph();
    LL getGraphHash();
    void addGraphToFS();



};


#endif //ALGORITHMSPROJECT_DEPTHTREECREATOREXACT_H
