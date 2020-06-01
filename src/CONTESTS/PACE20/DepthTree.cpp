//
// Created by sylwester on 3/16/20.
//

#include <CONTESTS/PACE20/DepthTree.h>
#include <datastructures/FAU.h>
#include <graphs/GraphReader.h>
#include "graphs/GraphUtils.h"
#include "graphs/trees/LCABinaryLifting.h"

#include "CONTESTS/PACE20/DepthTree.h"

DepthTree::DepthTree(VVI& V){
    this->V = &V;
}

DepthTree::DepthTree( VVI& V, int rt, int h, unordered_map<int,int>& parent ){
    this->V = &V;
    root= rt;
    height = h;
    par = parent;
}


bool DepthTree::isCorrect(){

//    cerr << "LCABinaryLifting isCorrect() check" << endl;
    VVI stdStr = getStandardStructure();

    if( stdStr.size() != par.size() ){
        cerr << "stdStr.size() != par.size()" << endl;
        return false;
    }

    if( V->size() != par.size() ){
        cerr << "V->size() != par.size()" << endl;
        return false;
    }

//    assert( stdStr.size() == par.size() );
//    assert( V->size() == par.size() );

    LCABinaryLifting lcabl( stdStr, stdStr, root, [](int a, int b){ return a+b; },0 );

    for( int i=0; i <V->size(); i++ ){
        for( int d : (*V)[i] ){
            int lca = lcabl.lca( i,d );
//            assert( lca == i || lca == d );
            if( lca != i && lca != d ) return false;
        }
    }

    return true;
}


ostream& operator<<(ostream& str, DepthTree& dt){
    str << "[[ root: " << dt.root << "  |  height: " << dt.height << "  |  dt.V.size(): " << dt.V->size() << " ]]";
//    str << "[[ root: " << dt.root << "  |  height: " << dt.height << "  |  dt.par: " << set<PII>(ALL(dt.par)) << " ]]";
//    str << "[[ root: " << dt.root << "  |  height: " << dt.height << "  |  dt.struct: " << dt.getStandardStructure() << " ]]";
    return str;
}

VVI DepthTree::getStandardStructure() {
    VVI res(par.size());
    for( PII p : par ){
        if( p.second != -1 ){
            res[p.first].push_back(p.second);
            res[p.second].push_back(p.first);
        }
    }
    return res;
}

pair<VVI, vector<DepthTree::StretchStructureNode> > DepthTree::getStretchStructure() {
//    cerr << "Entering getStretchStructure()" << endl;
    VVI T = getStandardStructure();

//    DEBUG(T);

    int N = T.size();
    FAU fau(N);

    function< void(int,int) > dfsUnion = [=,&N, &dfsUnion,&T,&fau](int num, int par){
//        DEBUG(num);
//        DEBUG(par);
        if( (num == par && T[num].size() == 1) || ( num != par && T[num].size() == 2 ) ){
//            cerr << "unifying, num = " << num << "   par = " << par << endl;
            for( int p : T[num] ){
                if( p != par ) fau.Union(num,p);
            }
        }
        if( T[num].size() == 1 && num != par ) return; // num is a leaf
        for( int p : T[num] ){
            if( p != par ) dfsUnion(p,num);
        }
    };

    dfsUnion(root,root);

//    for(int i=0; i<N; i++) cerr << "fau.Find(" << i << ") = " << fau.Find(i) << endl;


    VVI strStr(N);
    VVI nodesInStretch(N);

    function< void(int,int) > dfsContract = [&N, &fau, &dfsContract, &T, &strStr, &nodesInStretch]( int num, int par ){
        nodesInStretch[ fau.Find(num)].push_back(num);

        if( num != par ){
            int p = fau.Find(num);
            int d = fau.Find(par);

            if( p != d ){
                strStr[p].push_back(d);
                strStr[d].push_back(p);
            }
        }

        for( int p : T[num] ){
            if( p != par ) dfsContract(p,num);
        }
    };


    dfsContract(root,root);

//    DEBUG(strStr);
//    DEBUG(nodesInStretch);

    int cnt = 0;
    for( int i=0; i<N; i++ ){
//        if( !strStr[i].empty() ){
        if( !nodesInStretch[i].empty() ){
            cnt++;
        }
    }


    VVI resStr(cnt);
    VVI resNodesInStretch(cnt);

//    DEBUG(N);
//    DEBUG(cnt);

    cnt = -1;
    function< void(int,int,int) > dfsRemap = [&N,&dfsRemap,&strStr,&cnt,&resStr, &nodesInStretch, &resNodesInStretch](int num, int par, int cntPar){
//        DEBUG(num);
//        DEBUG(par);
//        ENDL(1);
        cnt++;
        resNodesInStretch[cnt] = nodesInStretch[num];
        if( num != par ){
            resStr[cnt].push_back( cntPar );
            resStr[cntPar].push_back(cnt);
        }

        int myCnt = cnt;
        for(int p : strStr[num]){
            if( p != par ) dfsRemap( p,num,myCnt );
        }


    };

//    DEBUG(root);

    bool remapped = false;
    for( int i=0; i<N; i++ ){
        for( int d : nodesInStretch[i] ){
            if( d == root ){
//                cerr << "remapping" << endl;
                dfsRemap( i,i,-1 );
            }
        }
    }

//    DEBUG(resStr);
//    DEBUG(resNodesInStretch);



    int n = resStr.size();
    vector<StretchStructureNode> data(n);

    function< void(int,int) > dfsCreateData = [&dfsCreateData,&data,&resStr,&resNodesInStretch](int num, int par){
        for( int p : resStr[num] ){
            if(p != par) dfsCreateData(p,num);
        }

        int M = 0;
        for(int p :resStr[num]){
            if( p != par ){
                if( data[p].height > M ){
                    M = data[p].height;
                }
            }
        }

        data[num].nodeId = num;
        data[num].height = resNodesInStretch[num].size() + M;
        data[num].sepNodes = resNodesInStretch[num];
    };

    dfsCreateData(0,0);

//    DEBUG(resStr);
//    DEBUG(data);

//    exit(1);
//    cerr << "Leaving getStretchStructure()" << endl;
    return { resStr, data };
}



int DepthTree::calculateHeight() {
    VVI V = getStandardStructure();
    if( V.size() == 1 ) return 1;

    function< int(int,int) > dfs = [=,&dfs,&V](int num, int par){
        if( num != root && V[num].size() == 1 ) return 1;

        int M = 0;
        for(int d : V[num]){
            if( d != par ) M = max( M, dfs(d,num) );
        }
        return M+1;
    };

    return dfs(root,root);
}

unordered_map<int, int> DepthTree::getNodeDepths() {
    auto stdStruct = getStandardStructure();

    unordered_map<int,int> nodeDepth;
    function< void(int,int,int) > calcDepthDfs = [ &stdStruct, &nodeDepth, &calcDepthDfs ]( int num, int par, int depth ){
        nodeDepth[num] = depth;
        for( int p : stdStruct[num] ) if( p != par ) calcDepthDfs( p,num,depth+1 );
    };

    calcDepthDfs( root, root, 0 );

    return nodeDepth;
}

void DepthTree::writeBalanceStructure(int DEPTH) {

    DEBUG(height);
    auto strStr = getStretchStructure();

    auto tree = strStr.first;
    int T = tree.size();
    auto segments = strStr.second;

    VI subHeight(T,-1), subSize(T,0);

    function< void(int,int) > calc = [&tree, &segments, &subHeight, &subSize, &calc](int num, int par){
        subSize[num] = 1;
        subHeight[num] = 1;

        for( int d : tree[num] ){
            if( d != par ){
                calc(d,num);
                subHeight[num] = max( subHeight[num], 1 + subHeight[d] );
                subSize[num] += subSize[d];
            }
        }
    };

    calc(0,0);

    function< void(int,int,int) > writeDfs = [&tree, &segments, &subHeight, &subSize, &writeDfs, &DEPTH](int num, int par, int depth){
//        DEBUG(tree[num]);
        sort( ALL( tree[num] ), [&subHeight]( int a, int b ){ return subHeight[a] > subHeight[b]; } );
//        DEBUG(tree[num]);

        if( depth <= DEPTH ) {
            for (int i = 0; i < depth-1; i++) cerr << "     ";
                                              cerr << "-----";
            cerr << "segment: " << segments[num] << endl;

            for( int d : tree[num] ){
                if( d != par ){
                    for( int i=0; i<depth; i++ ) cerr << "     ";
//                                                   cerr << "-----";
                    cerr << "\t\t" << segments[d] << endl;
                }
            }
            cerr << endl;
            for( int d : tree[num] ) if(d != par) writeDfs(d,num, depth+1);
        }
    };

    writeDfs( 0,0,0 ); // root of the stretch tree is always 0.

}



void DepthTree::test() {
//    VVI V = GraphReader::readGraphDIMACSWunweighed(cin);
//    DEBUG(V.size());
//    DEBUG( GraphUtils::countEdges(V) );

    VVI V = { {1,2,4}, {0,5}, {0}, {4}, {3,0,7}, {1,6}, {5,10}, {4,8}, {7,9}, {8}, {6,11}, {10}, {6,13}, {12} };
    int N = V.size();

    DepthTree dt(V);
    dt.root = 3;
    dt.height = 8;
    dt.V = &V;

    dt.par = { {3,-1}, {4,3}, {0,4}, {2,0}, {1,0}, {5,1}, {6,5}, {10,6}, {11,10}, {12,6}, {13,12}, {7,4}, {8,7}, {9,8} };

    assert( dt.par.size() == N );

    auto structure = dt.getStretchStructure();
    DEBUG(structure);

    exit(1);

}