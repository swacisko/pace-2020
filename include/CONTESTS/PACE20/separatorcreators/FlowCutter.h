/*
 * Copyright (c) 2020, Sylwester Swat
*/

#ifndef ALGORITHMSPROJECT_FLOWCUTTER_H
#define ALGORITHMSPROJECT_FLOWCUTTER_H

//#include <graphs/flow/UnitFlow.h>
#include <graphs/flow/UnitFlow2.h>
#include "SeparatorCreator.h"

class FlowCutter : public SeparatorCreator{

public:

    FlowCutter( SeparatorEvaluator eval );

    ~FlowCutter(){ if( uf != nullptr ) delete uf; }

    vector<Separator> createSeparators( VVI& V, int repeats = 1 );

    vector<Separator> getSeparatorsForSourcesAndTargets(VVI &V, VVI &expV, VI S, VI T);

    VI getFlowCutterExpansionOrder( VVI& V, VI sources, VI ends );

    void setMaximalDeviationFromBestResultFactor(double d){ maximalDeviationFromBestResultFactor = d; }

    static VVI getExpansionGraph( VVI& V );

    void setBalanceParameter(double d){ balanceParameter = d; }

    static void test();

    /**
    * If we add pN to source, then we maximize dst(t,p), otherwise we maximize dst(s,p)
    */
    static const int MAXIMIZE_DISTANCE = 1;

    /**
     * As in the original FLowCutter, if we add pN to source, then we maximize dst(t,p) - dst(s,p), if we add pN to targets, we maximize dst(s,p) - dst(t,p)
     */
    static const int MAXIMIZE_HIPERBOLIC_DISTANCE = 2;

    int PN_STRATEGY = MAXIMIZE_HIPERBOLIC_DISTANCE;
//    int PN_STRATEGY = MAXIMIZE_DISTANCE;

private:
    SeparatorEvaluator sepEval;
    VVI V;
    VVI revV; // reverse graph of V needed to get piercing target nodes

    int N;

    bool usingExpansionGraph = false;

    double balanceParameter = 2; // this is the maximum value such that at the end of FC it can happen that, |A| == balanceParameter * |B|

    VI expansionOrder;
    VB inExpansionOrder;
    void addToExpansionOrder( VI nodes );

    bool canBeAugmented;

    /**
     * if at some FlowCutter iteration the value of current flow exceeds
     * maximalDeviationFromBestResultFactor * bestResultSoFarSeparatorNodesSize
     * then it is terminated
     */
    int bestResultSoFarSeparatorNodesSize;
    double maximalDeviationFromBestResultFactor = 1.5;

    VI sources, targets;
    VB isSource, isTarget;

    void addToSources( VI S );
    void addToTargets( VI T );

    VI sourceReachable, targetReachable;
    VI sourceBorder, targetBorder; // sourceBorder should contain those nodes, that have a neighbor outside source-reachable / target-reachable nodes
    VB isSourceReachable, isTargetReachable;

    VI dstFromSources, dstFromTargets;
    /**
     * if PN_STRATEGY == MAXIMIZE_HIPERBOLIC_DISTANCE, then dst1[p] - dst2[p] is returned
     * if MAXIMIZE_DISTANCE, then dst1[p] is returned.
     * This function can be used to get both source and target piercing nodes, just passing correctly dstFromSources and dstFromTargets as parameters
     * @param dst1
     * @param dst2
     * @param p
     * @return
     */
    int getDistanceValueFrom( VI& dst1, VI& dst2, int p );

    void calculateDistances();

//    UnitFlow *uf = nullptr;
    UnitFlow2 *uf = nullptr;

    void fGrow(int s = -1);
    void bGrow(int t = -1);

    int getPiercingSourceNode();
    int getPiercingTargetNode();

    class PiercingNode{
    public:
        PiercingNode( int id, bool src, bool augm ) : v(id), sourcePN(src), augmenting(augm) {}
        int v; // just the node
        bool sourcePN; // if true, then it is source piercing node
        bool augmenting; // true if the node is an augmenting node
    };

    /**
     *
     * @return vector containing all piercing nodes.
     */
    vector<PiercingNode> getAllPiercingNodes();



    VI getVertexCoverOfSTBarrier( VVI& V );



};

#endif //ALGORITHMSPROJECT_FLOWCUTTER_H
