//
// Created by sylwe on 10/03/2026.
//

#ifndef EXTREEM_EXP1_H
#define EXTREEM_EXP1_H

#include <ranges>

#include "Config.h"
#include "DepthTree.h"
#include "DTKernelizer.h"
#include "Makros.h"


struct ExpData {
    int N0,M0, N,M;

    // map<string,string> pivot_data;
    // map<string,string> preprocessing_data;
    // map<string,string> sep_minim_data;
    // map<string,string> sep_cr_data;
    // map<string,string> sep_eval_data;

    string tree_heights;

    string sep_sizes_before_minim;
    string avg_sep_sizes_before_minim;
    double avg_avg_sep_sizes_before_minim;
    string sep_sizes_after_minim;
    string avg_sep_sizes_after_minim;
    double avg_avg_sep_sizes_after_minim;

    string estimated_td_before_minim;
    string avg_estimated_td_before_minim;
    double avg_avg_estimated_td_before_minim;
    string estimated_td_after_minim;
    string avg_estimated_td_after_minim;
    double avg_avg_estimated_td_after_minim;

    static vector<string> getHeader() {
        vector<string> fields{
            "N0", "M0", "N", "M",
            "tree heights",

            "avg sep sizes before minim",
            "avg sep sizes after minim",
            "avg avg sep sizes before minim",
            "avg avg sep sizes after minim",

            "avg appx td before minim",
            "avg appx td after minim",
            "avg avg appx td before minim",
            "avg avg appx td after minim",

            "sep sizes before minim",
            "sep sizes after minim",
            "appx td before minim",
            "appx td after minim",
        };
        vector<string> res;
        res = fields;
        return res;
    }



    void writeData(ostream & str, Config cnf) {
        auto writeHeader = [&](){
            auto header_fields = getHeader();
            for( auto [i,f] : views::enumerate(header_fields) ) {
                if (i) str << ",";
                str << f;
            }
            str << endl;
        };

        auto writeData = [&]() {
            str << N0 << "," << M0 << "," << N << "," << M << ","
            << tree_heights << ","

            << avg_sep_sizes_before_minim << ","
            << avg_sep_sizes_after_minim << ","
            << avg_avg_sep_sizes_before_minim << ","
            << avg_avg_sep_sizes_after_minim << ","

            << avg_estimated_td_before_minim << ","
            << avg_estimated_td_after_minim << ","
            << avg_avg_estimated_td_before_minim << ","
            << avg_avg_estimated_td_after_minim << ","

            << sep_sizes_before_minim << ","
            << sep_sizes_after_minim << ","
            << estimated_td_before_minim << ","
            << estimated_td_after_minim;

            str << endl;
        };

        writeHeader();
        writeData();
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