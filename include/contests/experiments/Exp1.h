//
// Created by sylwe on 10/03/2026.
//

#ifndef EXTREEM_EXP1_H
#define EXTREEM_EXP1_H

#include "Config.h"
#include "DepthTree.h"
#include "DTKernelizer.h"
#include "Makros.h"
#include "../../graphs/GraphUtils.h"


struct ExpData {
    int N0,M0, N,M, treedepth;

    map<string,string> pivot_data;
    map<string,string> preprocessing_data;
    map<string,string> sep_minim_data;
    map<string,string> sep_cr_data;
    map<string,string> sep_eval_data;

    static vector<string> getHeader() {
        vector<string> fields{"N0", "M0", "N", "M", "td" };
        vector<string> res;
        res = fields;
        return res;
    }

    void writeData(ostream & str, Config cnf) {

    }
};


class Exp1 {
public:
    Exp1(VVI V, Config cnf) : init_kernelizer(V,cnf) {
        this->V0 = this->V = V;
        this->cnf = cnf;
    }

    void runForConfiguration();

    void runPreprocessingExperiments();
    void runSeparatorCreatorExperiments();
    void runSeparatorMinimizerExperiments();
    void runPivotExperiments();
    void runSeparatorEvaluatorExperiments();
    void runPredefinedConfigurationsExperiments();
    void runFixedTimeExperiments();
    void runAllExperiments();

    void runExtensiveConfigurationExperiments();


    VVI V0, V;
    Config cnf;
    ExpData data;


    void initPreprocessing();
    void liftSolution(DepthTree & dtree);
    void updateBestTree(DepthTree & best, DepthTree & dtree);
    DTKernelizer init_kernelizer;


    vector<pair<double,bool>> createNSFS();
    void assignExpData(auto & trees, auto & sep_stats);

};




#endif //EXTREEM_EXP1_H