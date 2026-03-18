//
// Created by sylwe on 10/03/2026.
//

#include "../../../include/contests/experiments/Exp1.h"

#include <getopt.h>
#include <ranges>

#include "DepthTreeCreatorLarge.h"
#include "GraphReader.h"
#include "../../../include/contests/experiments/Config.h"

#include "DTKernelizer.h"
#include "IntGenerator.h"
#include "SeparatorEvaluators.h"

vector<pair<double, bool>> Exp1::createNSFS() {
    vector<pair<double,bool>> nsfs;
    for (int i=0; i<cnf.main_repetitions; i++) nsfs.emplace_back(1.0 * i / (cnf.main_repetitions-1), i & 1);
    ranges::sort(nsfs,[&](auto a, auto b){ return abs(0.5 - a.first) < abs(0.5 - b.first); });
    return nsfs;
}

void Exp1::assignExpData(auto &trees, auto & sep_stats) {
    ranges::sort(trees, [&](auto &t1, auto & t2){ return t1.second < t2.second; });

    clog << "Found trees: " << endl;
    for (int t : views::transform( trees, [&](auto & tr){ return tr.first.height; } )) clog << t << " ";
    clog << endl;



    clog << "Found avg separator sizes, before minimization: " << endl;
    for (double t : views::transform( sep_stats, [&](auto & vec) {
        double avg_size = accumulate( ALL(vec), 0.0, [&](double s, auto & b) { return s + b.first.size; } ) / vec.size();
        return avg_size;
    } )) clog << t << " ";
    clog << endl;

    clog << "Found avg estimated tree depth based on separator stats, before minimization: " << endl;
    for (double t : views::transform( sep_stats, [&](auto & vec) {
        double avg_td = accumulate( ALL(vec), 0.0, [&](double s, auto & b) {
            return s + b.first.estimated_td_edge_plus_node;
        } ) / vec.size();
        return avg_td;
    } )) clog << t << " ";
    clog << endl;



    clog << "Found avg separator sizes, after minimization: " << endl;
    for (double t : views::transform( sep_stats, [&](auto & vec) {
        double avg_size = accumulate( ALL(vec), 0.0, [&](double s, auto & b) { return s + b.second.size; } ) / vec.size();
        return avg_size;
    } )) clog << t << " ";
    clog << endl;

    clog << "Found avg estimated tree depth based on separator stats, after minimization: " << endl;
    for (double t : views::transform( sep_stats, [&](auto & vec) {
        double avg_td = accumulate( ALL(vec), 0.0, [&](double s, auto & b) {
            return s + b.second.estimated_td_edge_plus_node;
        } ) / vec.size();
        return avg_td;
    } )) clog << t << " ";
    clog << endl;

}

void Exp1::runForConfiguration() {
    clog << "Exp1 -> running for configuration" << endl;

    vector<pair<DepthTree,double>> trees;
    vector<vector<pair<SeparatorStats,SeparatorStats>>> sep_stats;

    auto nsfs = createNSFS();
    if ( cnf.node_scale_factor != -1.0 ) std::ranges::for_each(nsfs, [&](auto & a){a.first = cnf.node_scale_factor;});
    DEBUG(nsfs);

    Config cnf = this->cnf;
    cnf.write_logs = false;
    if (cnf.experiment_name == "sep_cr" || cnf.experiment_name == "sep_minim") {
        cnf.find_valid_dtree = false;
    }
    cnf.startMain();
    if ( cnf.predefined_config_id ) cnf.setPredefinedConfig(cnf.predefined_config_id);

    while ( !cnf.sw.tle("main") ) {
        for(auto [nsf, min_node_iter] : nsfs) {
            if (cnf.sw.tle("main")) {
                unordered_map<int,int> par;
                // for (int i=1; i<V.size(); i++) par[i] = i-1;
                trees.emplace_back(DepthTree(V,0,V.size(),par), nsf); // path 0->1->2...
                continue;
            }

            initPreprocessing();

            SeparatorEvaluators::nodeScaleFactor = nsf;
            SeparatorEvaluators::edgeScaleFactor = 1.0 - nsf;
            cnf.minimize_nodes_iteration = min_node_iter;

            DepthTreeCreatorLarge creator( V,0, cnf );
            auto dtree = creator.getDepthTree();
            if (cnf.find_valid_dtree) liftSolution(dtree);

            for (auto &sd: creator.sep_data) {
                sd.first.estimated_td_edge_plus_node =
                    SeparatorEvaluators::edgeScaleFactor * SeparatorEvaluators::estimateDepthBasedOnEdges(sd.first)
                    + SeparatorEvaluators::nodeScaleFactor * SeparatorEvaluators::estimateDepthBasedOnNodes(sd.first);

                sd.second.estimated_td_edge_plus_node =
                    SeparatorEvaluators::edgeScaleFactor * SeparatorEvaluators::estimateDepthBasedOnEdges(sd.second)
                    + SeparatorEvaluators::nodeScaleFactor * SeparatorEvaluators::estimateDepthBasedOnNodes(sd.second);

                DEBUG2(sd.first, sd.second);
            }

            trees.emplace_back(dtree,nsf);
            sep_stats.push_back(creator.sep_data);
        }

        if ( !cnf.run_until_time_limit ) break;

        // if running until tle, we keep only the best tree for given nfs
        sort(ALL(trees), [&](auto & a, auto & b) {
            if (a.second != b.second) return a.second < b.second;
            return a.first.height < b.first.height;
        });
        decltype(trees) new_trees;
        for ( int i=0; i<trees.size(); ) {
            int p = i+1;
            while ( trees[i].second == trees[p].second && p < trees.size() ) p++;
            new_trees.push_back(trees[i]);
            i = p;
        }
        if ( trees.size() > cnf.main_repetitions ) assert( new_trees.size()+cnf.main_repetitions == trees.size() );
    }

    assignExpData(trees, sep_stats);
}

void Exp1::runPreprocessingExperiments() {
    clog << "Exp1 -> running preprocessing experiments" << endl;
    initPreprocessing();

}

void Exp1::runSeparatorCreatorExperiments() {
    clog << "Exp1 -> running separator creator experiments" << endl;
    initPreprocessing();

}

void Exp1::runSeparatorMinimizerExperiments() {
    clog << "Exp1 -> running separator minimizer experiments" << endl;
    initPreprocessing();

}

void Exp1::runPivotExperiments() {
    clog << "Exp1 -> running pivot experiments" << endl;
    initPreprocessing();

}

void Exp1::runSeparatorEvaluatorExperiments() {
    clog << "Exp1 -> running separator evaluator experiments" << endl;
    initPreprocessing();

}

void Exp1::runPredefinedConfigurationsExperiments() {
    clog << "Exp1 -> running predefined configurations experiments" << endl;
    initPreprocessing();

}

void Exp1::runFixedTimeExperiments() {
    clog << "Exp1 -> running fixed time experiments" << endl;
    initPreprocessing();

}

void Exp1::runAllExperiments() {
    clog << "Exp1 -> running all experiments" << endl;

    string en = cnf.experiment_name;

    if (en == "cur_config") runForConfiguration();
    if (en == "sep_cr") runForConfiguration();
    if (en == "sep_minim") runForConfiguration();


    // else if (en == "pivots") runPivotExperiments(); // done in runForConfiguration
    // else if (en == "sep_cr") runSeparatorCreatorExperiments(); // done in runForConfiguration
    // else if (en == "sep_minim") runSeparatorMinimizerExperiments(); // done in runForConfiguration
    // else if (en == "prepr") runPreprocessingExperiments(); // done in runForConfiguration
    // else if (en == "predefined_configs") runPredefinedConfigurationsExperiments(); // done in runForConfiguration
    // else if (en == "fixed_time") runFixedTimeExperiments(); // done in runForConfiguration
    // else throw runtime_error("Unknown experiment name: " + en);
    // else if (en == "sep_eval") runSeparatorEvaluatorExperiments(); // do not do that at all - it can be extracted from almost all other experiments
}

void Exp1::runExtensiveConfigurationExperiments() {
}

void Exp1::initPreprocessing() {
    tie(data.N0, data.M0) = PII(V0.size(), GraphUtils::countEdges(V0));

    if (cnf.use_init_prepr) {
        V = V0;
        init_kernelizer = DTKernelizer(V,cnf);
        if (cnf.write_logs) clog << "Starting initial kernelization" << endl;
        V = init_kernelizer.getKernelizedGraphSubgraphs(); // harder kernelization
        if (cnf.write_logs)
            clog << "\t initial kernelization done, V.size(): " << V.size() << ", edges: "
                 << GraphUtils::countEdges(V) << endl;
    }

    tie(data.N, data.M) = PII(V.size(), GraphUtils::countEdges(V));
}

void Exp1::liftSolution(DepthTree & dtree) {
    if (!cnf.use_init_prepr) return;
    dtree = init_kernelizer.dekernelizeSubgraphs(dtree);
    dtree.V = &V0;
}

void Exp1::updateBestTree(DepthTree &best, DepthTree &dtree) {
    if (dtree.height < best.height) best = dtree;
}




Config parseArguments(int argc, char ** argv) {
    Config cnf{};

    class ArgParser {
    public:
        // Store flags: --verbose, --help
        unordered_map<string, bool> flags;

        // Store options with values: --input=..., --threads=...
        unordered_map<string, string> options;

        // Which names are flags/options
        unordered_set<string> flag_names;
        unordered_set<string> option_names;
        unordered_set<string> required_options;

        void addFlag(const string &name) {
            flag_names.insert(name);
            flags[name] = false;
        }

        void addOption(const string &name, bool required) {
            option_names.insert(name);
            options[name] = "";
            if (required) required_options.insert(name);
        }

        using VARIANT = variant<bool*,int*,double*,string*>;
        void findAndAssign(string name, string type, VARIANT data) {
            if (!hasProvidedOption(name)) return;


            if (type == "bool") {
                bool* ptr = get<bool*>(data);
                auto isTrue = [&](string s) { return s == "True" || s == "true" || s == "1"; };
                *ptr = isTrue(getOption(name));
            }
            else if (type == "int") {
                int* ptr = get<int*>(data);
                *ptr = stoi(getOption(name));
            }else if (type == "double") {
                double* ptr = get<double*>(data);
                *ptr = stod(getOption(name));
            }else if (type == "string") {
                string* ptr = get<string*>(data);
                *ptr = getOption(name);
            }
        }

        void parse(int argc, char **argv) {
            for (int i = 1; i < argc; i++) {
                string arg = argv[i];

                if (!startsWithDoubleDash(arg)) throw runtime_error("Unknown positional or malformed argument: " + arg);

                string inner = arg.substr(2); // strip "--"
                size_t eq = inner.find('='); // Split on '='
                string name, value;

                if (eq == string::npos) { // No '=' → must be a flag (e.g., --verbose)
                    name = inner;

                    if (flag_names.contains(name)) {
                        flags[name] = true;
                    } else if (option_names.contains(name)) {
                        throw runtime_error("Missing '=value' for option --" + name + " (expected --" + name + "=VALUE)");
                    } else {
                        throw runtime_error("Unknown argument: --" + name);
                    }
                } else {
                    // Has '=' → must be an option: --name=value
                    name = inner.substr(0, eq);
                    value = inner.substr(eq + 1);

                    if (flag_names.contains(name)) {
                        throw runtime_error("Flag --" + name + " does not take a value (remove '=...').");
                    } else if (option_names.contains(name)) {
                        if (value.empty()) {
                            throw runtime_error("Missing value for option --" + name + " (use --" + name + "=VALUE)");
                        }
                        options[name] = value;
                    } else {
                        throw runtime_error("Unknown argument: --" + name);
                    }
                }
            }
        }

        bool getFlag(const string &name) const {
            auto it = flags.find(name);
            if (it == flags.end()) throw runtime_error("Flag not registered: " + name);
            return it->second;
        }

        bool hasProvidedOption(const string &name) const { return options.find(name)->second != ""; }

        string getOption(const string &name) const {
            auto it = options.find(name);
            if (it == options.end()) throw runtime_error("Option not registered: " + name);
            return it->second;
        }

        void printHelp(const string &progName) const {
            cout << "Usage: " << progName << " [options]\n\n";
            cout << "Options:\n";
            for (auto &f : flag_names) cout << "  --" << f << "\n";
            for (auto &o : option_names) cout << "  --" << o << "=<value>\n";
            cout << "\n";
        }

    private:
        static bool startsWithDoubleDash(const string &s) {
            return s.size() >= 2 && s[0] == '-' && s[1] == '-';
        }
    };


    ArgParser ap;
    ap.addOption("experiment_name",true);
    ap.addOption("time", true);
    ap.addOption("mtd", true);
    ap.addOption("run_until_tle", false);
    ap.addOption("pred_conf", false);
    ap.addOption("main_reps", false);


    // ap.addOption("imb_impr", false); // imbalanced improver
    ap.addOption("pivots_mask", false);
    ap.addOption("sep_cr_mask", false);
    ap.addOption("sep_minim_mask", false);
    ap.addOption("prepr_mask", false);
    ap.addOption("nsf", false);
    ap.addOption("init_prepr", false);

    ap.parse(argc, argv);
    for ( string opt : ap.required_options ) if( !ap.hasProvidedOption(opt) ) {
        clog << "Option " << opt << " is not provided, but is mandatory!" << endl;
    }
    for ( string opt : ap.required_options ) assert( ap.hasProvidedOption(opt) );


    ap.findAndAssign("time", "int", &cnf.max_time_millis);
    cnf.max_time_millis *= 1000;

    ap.findAndAssign("mtd", "string", &cnf.metadata_filepath);
    ap.findAndAssign("experiment_name", "string", &cnf.experiment_name);
    ap.findAndAssign("run_until_tle", "bool", &cnf.run_until_time_limit);
    ap.findAndAssign("pred_conf", "int", &cnf.predefined_config_id);
    ap.findAndAssign("main_reps", "int", &cnf.main_repetitions);
    ap.findAndAssign("nsf", "double", &cnf.node_scale_factor);
    ap.findAndAssign("init_prepr", "bool", &cnf.use_init_prepr);


    assert( cnf.allowed_experiments.contains(cnf.experiment_name) );
    if (cnf.experiment_name == "predefined_configs") assert( cnf.predefined_config_id != 0 );

    return cnf;
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout << fixed;

    clog << "Hello, running experiments!" << endl;

    Config cnf = parseArguments(argc, argv);
    cnf.writeBasicInfo();

    // auto V = GraphReader::readGraphStandardEdges(cin);
    auto V = GraphReader::readGraphDIMACSWunweighed(cin);
    clog << "Graph read, V.size() = " << V.size() << ", edges: " << GraphUtils::countEdges(V) << endl;


    // running experiments
    Exp1 exp_runner(V,cnf);
    exp_runner.runAllExperiments();


    // writing data to the cnf.metadata_filepath file
    auto data = exp_runner.data;
    ofstream f(cnf.metadata_filepath);
    data.writeData(f,cnf);
    f.close();

    return 0;
}
