//
// Created by sylwe on 10/03/2026.
//

#ifndef EXTREEM_EXP1_H
#define EXTREEM_EXP1_H

#include "Config.h"
#include "Makros.h"
#include "../../graphs/GraphUtils.h"


struct ExpData {
    int N0,M0, N,M, treedepth;

    map<string,double> pivot_data;
    map<string,double> preprocessing_data;
    map<string,double> sep_minim_data;
    map<string,double> sep_cr_data;
    map<string,double> sep_eval_data;

    static vector<string> getHeader() {
        vector<string> fields{"N0", "M0", "N", "M", "treedepth" };
        vector<string> res;

        // for( string s : {""} ) { for(const auto & f : fields) res.push_back(s + "-" + f); }

        res = fields;

        return res;
    }

    void writeData(ostream & str) {

    }
};


class Exp1 {
public:
    Exp1(VVI V, Config cnf) { this->V = V; this->cnf = cnf; }

    void runPreprocessingExperiments();
    void runSeparatorCreatorExperiments();
    void runSeparatorMinimizerExperiments();
    void runPivotExperiments();
    void runSeparatorEvaluatorExperiments();
    void runAllExperiments();


    VVI V;
    Config cnf;
    ExpData data;
};




#endif //EXTREEM_EXP1_H