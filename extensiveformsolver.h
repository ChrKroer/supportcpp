#ifndef EXTENSIVEFORMSOLVERH
#define EXTENSIVEFORMSOLVERH

#include <ilcplex/ilocplex.h>
#include <map>
#include <set>
#include "game.h"
#include "sequenceform.h"
#include "optimalsignaltreeabstractor.h"
ILOSTLBEGIN

class ExtensiveFormSolver{
public:
    ExtensiveFormSolver(ExtensiveFormGame& game, int player = 1);
    ExtensiveFormSolver(ExtensiveFormGame& game, OptimalSignalTreeAbstractor& abstraction, int player = 1);
    ~ExtensiveFormSolver();
    void SolveLP();
    double GetObjectiveValue();
    std::vector<double> GetStrategyVarValues();
    std::map<string, double> GetStrategyVarMap();
    std::vector<double> GetStrategyVarMapByIndices();
    void PrintValues();
    void PrintGameValue();
    void WriteToFile(std::string filename);

    std::vector<std::vector<pair<int, double> > > dual_matrix() { return dual_matrix_; }
    
private:
    ExtensiveFormGame& game_;
    OptimalSignalTreeAbstractor* abstraction_;
    int player_to_solve_for_;
    int player_not_to_solve_for_;
    bool compute_abstraction_solution_;

    // Number of these is |information_sets_p2|+1
    std::vector<std::shared_ptr<IloNumVar> > dual_vars_;
    // Number of these is |sequences1|
    std::vector<std::shared_ptr<IloNumVar> > strategy_vars_;
    std::vector<std::shared_ptr<IloNumVar> > strategy_vars_by_real_game_sequences_;
    // sequence_payoff_matrix_[player1][player2]
    //std::vector<std::vector<double> > sequence_payoff_matrix_;
    
    IloEnv env_;
    IloModel model_;
    IloCplex cplex_;

    void TraverseTreeToConstructRealGameVarMap(int current_node_id, std::vector<bool>& visited);
    void SetUp();
    void CreateVariables();
    void SetObjective();
    void BuildSequencePayoffMatrices();
    void TraverseTree(int current_node, double probability, std::shared_ptr<IloNumVar> parent, int player1_parent_sequence, int player2_parent_sequence);
    void TraversePlayerNode(int current_node_id, double probability, std::shared_ptr<IloNumVar> parent_var, int player1_parent_sequence, int player2_parent_sequence);
    void TraverseNatureNode(int current_node_id, double probability, std::shared_ptr<IloNumVar> parent_var, int player1_parent_sequence, int player2_parent_sequence);
    void HandleLeafNode(int current_node, double probability, int player1_parent_sequence, int player2_parent_sequence);
    void CreateDualConstraints();

        // visited_information_set_[player][set]
    std::vector<std::set<int> > visited_information_set_;
    std::map<int, std::vector<std::shared_ptr<IloNumVar> > > strategy_vars_by_information_set_;
    std::map<int, std::vector<int> > player1_sequences_by_information_set_;
    std::map<int, std::vector<int> > player2_sequences_by_information_set_;
    
    std::vector<Sequence> sequences1_;
    std::vector<Sequence> sequences2_;
    std::map<int, std::map<int, double> > sequences_times_sequences_payoffs_;
    //std::map<int, std::map<int, double> > sequences_times_sequences_payoffs2_;
    std::vector<Leaf> leaves_;

    std::vector<std::vector<pair<int, double> > > dual_matrix_;
    //std::map<std::vector<int> > dual_matrix_;

    int nodes_visited_;
    int leaf_nodes_visited_;
    int nature_nodes_visited_;
};

#endif
