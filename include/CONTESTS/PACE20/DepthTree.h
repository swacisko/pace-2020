/*
 * This file is a part of ExTREEm - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.
 * Copyright (c) 2020 Sylwester Swat
 * ExTREEm is free software, under GPL3 license. See the GNU General Public License for more details.
*/

#ifndef ALGORITHMSPROJECT_DEPTHTREE_H
#define ALGORITHMSPROJECT_DEPTHTREE_H

#include "Makros.h"

class DepthTree{


public:

    class StretchStructureNode{
    public:
        int nodeId; // id of the node in stretch-tree
        VI sepNodes; // nodes of separator represented by node with id given above
        int height; // height of the subtree (recursively created maximum height of son-tree plus sepNodes.size()
//        int subtreeSize; // size of the subtree
        friend ostream& operator<<(ostream& str, StretchStructureNode& s){
            str << "[[ id: " << s.nodeId << "  |  height: " << s.height << "  |  nodes: " << s.sepNodes << " ]]";
//            str << "[[ id: " << s.nodeId << "  |  height: " << s.height << "  |  nodes.size(): " << s.sepNodes.size() << " ]]";
            return str;
        }
    };



    //********************//********************//********************//********************
    DepthTree(VVI & V);

    DepthTree( VVI&, int rt, int h, unordered_map<int,int>& parent );

    VVI * V;

    /**
     * parent relation of the tree, -1 for root
     */
    unordered_map<int,int> par;

    int root;

    int height;


    /**
     *
     * @return true if given tree is a correct depth tree of given graph V
     */
    bool isCorrect();


    int calculateHeight();

    /**
     *
     * @return tree in usual VVI representation
     */
    VVI getStandardStructure();

    /**
     * res[i] is the depth of node i in this tree
     * @return
     */
    unordered_map<int,int> getNodeDepths();

    /**
     * Function returns stretch representation, that is each separator (sretch in the tree) is contracted and returned as single tree node.
     * root of the tree is contained always in segment with id 0.
     *
     * @return res.first is the structure of the tree, res.second is a vector containing nodes that are included in separators represented by nodes
     */
    pair<VVI,vector<StretchStructureNode> > getStretchStructure();

    static void test();

    void writeBalanceStructure(int DEPTH = 5);


    friend ostream& operator<<(ostream& str, DepthTree& dt);

    void write(){
        cout << height << endl;
        for( int i=0; i < V->size(); i++ ){
            cout << par[i]+1 << endl;
        }
    }

};



#endif //ALGORITHMSPROJECT_DEPTHTREE_H
