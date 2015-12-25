#include <algorithm>
#include "extensiveformsolver.h"
#include "filewriter.h"


ExtensiveFormSolver::ExtensiveFormSolver(ExtensiveFormGame& game, int player) : game_(game), compute_abstraction_solution_(false), env_(IloEnv()), model_(IloModel(env_)), cplex_(IloCplex(model_)) {
    player_to_solve_for_ = player;
    player_not_to_solve_for_ = (player % 2) + 1;
    //game_ = game;
    SetUp();
}

ExtensiveFormSolver::ExtensiveFormSolver(ExtensiveFormGame& game, OptimalSignalTreeAbstractor& abstraction, int player) : game_(game), abstraction_(&abstraction), compute_abstraction_solution_(true), env_(IloEnv()), model_(IloModel(env_)), cplex_(IloCplex(model_)) {
    player_to_solve_for_ = player;
    player_not_to_solve_for_ = (player % 2) + 1;
    //game_ = game;
    SetUp();
}


ExtensiveFormSolver::~ExtensiveFormSolver(){

}

void ExtensiveFormSolver::SolveLP(){
    cplex_.solve();
}

double ExtensiveFormSolver::GetObjectiveValue(){
    return -cplex_.getObjValue();
}

vector<double> ExtensiveFormSolver::GetStrategyVarValues(){
    vector<double> values;
    for (shared_ptr<IloNumVar> primal_var : strategy_vars_){
        values.push_back(cplex_.getValue(*primal_var));
    }
    return values;
}

map<string, double> ExtensiveFormSolver::GetStrategyVarMap(){
    map<string, double> values;

    if (compute_abstraction_solution_) {
        // TODO?
        for (shared_ptr<IloNumVar> primal_var : strategy_vars_){
            values[primal_var->getName()] = cplex_.getValue(*primal_var);
        }
    } else {
        for (shared_ptr<IloNumVar> primal_var : strategy_vars_){
            values[primal_var->getName()] = cplex_.getValue(*primal_var);
        }
    }


    return values;
}

vector<double> ExtensiveFormSolver::GetStrategyVarMapByIndices(){
    vector<double> values;
    if (compute_abstraction_solution_) {
        strategy_vars_by_real_game_sequences_.push_back(strategy_vars_[0]);
	vector<bool> visited;
	if (player_to_solve_for_ == 1) {
	    visited.resize(game_.num_full_game_sequences_player1(), false);
	} else {
	    visited.resize(game_.num_full_game_sequences_player2(), false);
	}
        TraverseTreeToConstructRealGameVarMap(game_.root(), visited);
        for (int i = 0; i < strategy_vars_by_real_game_sequences_.size(); i++){
            shared_ptr<IloNumVar> primal_var = strategy_vars_by_real_game_sequences_[i];
            values.push_back(cplex_.getValue(*primal_var));
        }
    } else {
        for (int i = 0; i < strategy_vars_.size(); i++){
            shared_ptr<IloNumVar> primal_var = strategy_vars_[i];
            values.push_back(cplex_.getValue(*primal_var));
        }
    }
    return values;
}

void ExtensiveFormSolver::TraverseTreeToConstructRealGameVarMap(int current_node_id, vector<bool>& visited) {
    const Node& node = game_.nodes()[current_node_id];
    int prototype_node_id = abstraction_->GetPrototypeNodeId(current_node_id);
    const Node& prototype_node = game_.nodes()[prototype_node_id];
    
    bool traversed = visited[node.information_set];
    
    for (int action_id = 0; action_id < node.actions.size(); action_id++){
        const Action& action = node.actions[action_id];

        if (!traversed && node.player ==  player_to_solve_for_) {
	    int prototype_information_set = abstraction_->GetAbstractionInformationSet(node.player, prototype_node.information_set);
            shared_ptr<IloNumVar> strategy_var = strategy_vars_by_information_set_[prototype_information_set][action_id];
            strategy_vars_by_real_game_sequences_.push_back(strategy_var);
        }
        TraverseTreeToConstructRealGameVarMap(action.child_id, visited);
    }
    if (node.player == player_to_solve_for_) {
	visited[node.information_set] = true;
    }
}

void ExtensiveFormSolver::PrintValues(){
    switch (cplex_.getStatus()) {
    case IloAlgorithm::Unknown:
        cout << "-------------- CPLEX Unknown status ------------------" << endl;
        break;
    case IloAlgorithm::Feasible:
        cout << "-------------- Feasible ------------------" << endl;
        break;
    case IloAlgorithm::Optimal:
    {
        cout << "status: optimal" << endl;
        double objective_value = -cplex_.getObjValue();
        if (objective_value == -0) objective_value = 0;
        cout << "Obj val: " << objective_value << endl;
    }
    break;
    case IloAlgorithm::Infeasible:
        cout << "-------------- INFEASIBLE ------------------" << endl;
        break;
    case IloAlgorithm::Unbounded:
        cout << "-------------- UNBOUNDED ------------------" << endl;
        break;
    case IloAlgorithm::InfeasibleOrUnbounded:
        cout << "-------------- Infeasible or Unbounded ------------------" << endl;
        break;
    case IloAlgorithm::Error:
        cout << "-------------- CPLEX ERROR ------------------" << endl;
        break;
    default:
        cout << "-------------- Defaulted ------------------" << endl;
    }

    /*if (cplex_.getStatus() == IloAlgorithm::Unbounded) {
      cout << "-------------- UNBOUNDED ------------------" << endl;
      } else if(cplex_.getStatus() == IloAlgorithm::Optimal){
      cout << "status: optimal" << endl;
      double objective_value = -cplex_.getObjValue();
      if (objective_value == -0) objective_value = 0;
      cout << "Obj val: " << objective_value << endl;
      } else if (cplex_.getStatus() == IloAlgorithm::Infeasible) {
      cout << "-------------- INFEASIBLE ------------------" << endl;
      } else if (cplex_.getStatus() == IloAlgorithm::Error) {
      cout << "-------------- CPLEX ERROR ------------------" << endl;
      } else if (cplex_.getStatus() == IloAlgorithm::Unknown) {
      cout << "-------------- CPLEX Unknown status ------------------" << endl;
      } else {
      cout << "-------------- Defaulted ------------------" << endl;
      }*/

    if (true) {
        for (shared_ptr<IloNumVar> primal_var : strategy_vars_){
            if (cplex_.getValue(*primal_var) > 0) {
                cout << primal_var->getName() << ": \t" << cplex_.getValue(*primal_var) << endl;
            }
        }

        /*for (shared_ptr<IloNumVar> dual_var : dual_vars_){
          cout << dual_var->getName() << ": \t" << cplex_.getValue(*dual_var) << endl;
          }*/
    }

}

void ExtensiveFormSolver::PrintGameValue(){
    if (cplex_.getStatus() == IloAlgorithm::Unbounded) {
        cout << "-------------- UNBOUNDED ------------------" << endl;
    } else if(cplex_.getStatus() == IloAlgorithm::Optimal){
        cout << "status: optimal" << endl;
        double objective_value = -cplex_.getObjValue();
        if (objective_value == -0) objective_value = 0;
        cout << "Obj val: " << objective_value << endl;
    } else if (cplex_.getStatus() == IloAlgorithm::Infeasible) {
        cout << "-------------- INFEASIBLE ------------------" << endl;
    } else if (cplex_.getStatus() == IloAlgorithm::Error) {
        cout << "-------------- CPLEX ERROR ------------------" << endl;
    } else {
        cout << "-------------- CPLEX Defaulted ------------------" << endl;
    }
}


void ExtensiveFormSolver::WriteToFile(string filename){
    FileWriter fw(filename);

    for (shared_ptr<IloNumVar> primal_var : strategy_vars_){
        if (cplex_.getValue(*primal_var) > 0) {
            cout << primal_var->getName() << ": \t" << cplex_.getValue(*primal_var) << endl;
        }
    }

    fw.CloseFile();
}

void ExtensiveFormSolver::SetUp(){
    cplex_.setParam(IloCplex::SimDisplay, 0);
    cplex_.setParam(IloCplex::MIPDisplay, 0);
    cplex_.setParam(IloCplex::TuningDisplay, 0);
    cplex_.setParam(IloCplex::BarDisplay, 0);
    cplex_.setParam(IloCplex::SiftDisplay, 0);
    cplex_.setParam(IloCplex::ConflictDisplay, 0);
    cplex_.setParam(IloCplex::NetDisplay, 0);
    cplex_.setParam(IloCplex::TiLim, 1e+75);
    //cplex_.setParam(IloCplex::RootAlg, IloCplex::Barrier);
    
    game_.RecomputeInformation();
    if (compute_abstraction_solution_) {
        game_.ComputeAbstractionInformation(abstraction_->game_node_mappings(), abstraction_->information_set_mappings());
    }
    

    CreateVariables();
    CreateDualConstraints();
    SetObjective();

    /*if (compute_abstraction_solution_) {
	cplex_.exportModel("/Users/ckroer/Documents/research/abstraction-extensive-form-games/extensive-form-game-abstraction.lp");
    } else {
	cplex_.exportModel("/Users/ckroer/Documents/research/abstraction-extensive-form-games/extensive-form-game.lp");
	}*/
}

void ExtensiveFormSolver::CreateVariables(){
    int num_dual_vars;
    vector<pair<int,double> > first_dual_row;
    if (player_to_solve_for_ == 1){
        num_dual_vars = game_.num_information_sets_player2()+1;
        for (int seq = 0; seq < game_.num_full_game_sequences_player2(); seq++) {
            sequences_times_sequences_payoffs_[seq] = map<int, double>();
        }
    } else {
        num_dual_vars = game_.num_information_sets_player1()+1;
        for (int seq = 0; seq < game_.num_full_game_sequences_player1(); seq++) {
            sequences_times_sequences_payoffs_[seq] = map<int, double>();
        }
    }
    for (int dual_idx = 0; dual_idx < num_dual_vars; dual_idx++){
        std::stringstream ss_dual_var;
        ss_dual_var << "y" << dual_idx;
        string var_name = ss_dual_var.str();
        replace(var_name.begin(), var_name.end(), '/', ';');
        //cout << var_name << endl;
        dual_vars_.push_back(make_shared<IloNumVar>(env_, var_name.c_str()));
    }

    first_dual_row.push_back(pair<int,double>(0,1));
    dual_matrix_.push_back(first_dual_row);



    Sequence seq;
    seq.node = -1;
    seq.action_name = "START";
    seq.probability_on_path = 1;
    seq.payoff1 = 0;
    seq.payoff2 = 0;

    sequences1_.push_back(seq);
    sequences2_.push_back(seq);


    std::stringstream ss_empty_var;
    ss_empty_var << "START";
    shared_ptr<IloNumVar>  empty_var(new IloNumVar(env_, 1, 1, ss_empty_var.str().c_str()));
    strategy_vars_.push_back(empty_var);

    nodes_visited_ = 0;
    leaf_nodes_visited_ = 0;
    nature_nodes_visited_ = 0;
    
    visited_information_set_.resize(3);
    TraverseTree(game_.root(), 1, empty_var, 0, 0);
}

void ExtensiveFormSolver::SetObjective(){
    IloObjective obj(env_, 0, IloObjective::Minimize);
    obj.setLinearCoef(*dual_vars_[0], 1);
    model_.add(obj);
}

void ExtensiveFormSolver::TraverseTree(int current_node_id, double probability, shared_ptr<IloNumVar> parent_var, int player1_parent_sequence, int player2_parent_sequence){
    Node current_node = game_.nodes()[current_node_id];
    nodes_visited_++;
    
    if (game_.IsLeaf(current_node_id)){
        HandleLeafNode(current_node_id, probability, player1_parent_sequence, player2_parent_sequence);
    } else if (current_node.player > 0) {
        TraversePlayerNode(current_node_id, probability, parent_var, player1_parent_sequence, player2_parent_sequence);
    } else {
        TraverseNatureNode(current_node_id, probability, parent_var, player1_parent_sequence, player2_parent_sequence);
    }
}

void ExtensiveFormSolver::TraversePlayerNode(int current_node_id, double probability, shared_ptr<IloNumVar> parent_var, int player1_parent_sequence, int player2_parent_sequence) {
    Node current_node = game_.nodes()[current_node_id];
    int current_information_set = current_node.information_set;
    if (compute_abstraction_solution_ && current_node.player > 0) {
	current_information_set = abstraction_->GetAbstractionInformationSet(current_node.player, current_node.information_set);
    }
    int new_player1_parent_sequence = player1_parent_sequence;
    int new_player2_parent_sequence = player2_parent_sequence;

    IloExpr realization_sum(env_);

    std::vector<pair<int, double> > dual_row;
    if (player_to_solve_for_ == 1){
        dual_row.push_back(pair<int,double>(player2_parent_sequence, -1));
    } else {
        dual_row.push_back(pair<int,double>(player1_parent_sequence, -1));
    }

    for (int child_idx = 0; child_idx < current_node.actions.size(); child_idx++) {
        if (visited_information_set_[current_node.player].count(current_information_set) == 0) {
            Sequence seq;
            seq.node = current_node_id;
	    std::stringstream ss_seq_name;

            
            seq.probability_on_path = probability;
            if (current_node.player == 1){
		ss_seq_name << sequences1_[player1_parent_sequence].action_name;
		ss_seq_name << current_node.actions[child_idx].name;
		seq.action_name = ss_seq_name.str();
		new_player1_parent_sequence = sequences1_.size();
                player1_sequences_by_information_set_[current_information_set].push_back(sequences1_.size());
                sequences1_.push_back(seq);
            } else {
		ss_seq_name << sequences2_[player2_parent_sequence].action_name;
		ss_seq_name << current_node.actions[child_idx].name;
		seq.action_name = ss_seq_name.str();
                new_player2_parent_sequence = sequences2_.size();
                player2_sequences_by_information_set_[current_information_set].push_back(sequences2_.size());

                sequences2_.push_back(seq);
            }

            if (current_node.player == player_to_solve_for_){
                std::stringstream ss_realization_var;
                string action = current_node.actions[child_idx].name;
                ss_realization_var << ";" << current_information_set << ";" << action;
                string var_name = ss_realization_var.str();
                replace(var_name.begin(), var_name.end(), '/', ';');
		replace(var_name.begin(), var_name.end(), ':', ';');
                shared_ptr<IloNumVar>  realization_var(new IloNumVar(env_, 0, 1, var_name.c_str()));
                strategy_vars_.push_back(realization_var);
                strategy_vars_by_information_set_[current_information_set].push_back(realization_var);
                realization_sum += *realization_var;
            } else { // build dual matrix
                if (player_to_solve_for_ == 1){
                    dual_row.push_back(pair<int,double>(sequences2_.size()-1,1));
                } else {
                    dual_row.push_back(pair<int,double>(sequences1_.size()-1,1));
                }
            }
        } else {
            if (current_node.player == 1){
                new_player1_parent_sequence = player1_sequences_by_information_set_[current_information_set][child_idx];
            } else {
                new_player2_parent_sequence = player2_sequences_by_information_set_[current_information_set][child_idx];
            }
        }
        if (current_node.player == player_to_solve_for_){
            TraverseTree(current_node.actions[child_idx].child_id, probability, strategy_vars_by_information_set_[current_information_set][child_idx], new_player1_parent_sequence, new_player2_parent_sequence);
        } else {
            TraverseTree(current_node.actions[child_idx].child_id, probability, parent_var, new_player1_parent_sequence, new_player2_parent_sequence);
        }
    }
    if (visited_information_set_[current_node.player].count(current_information_set) == 0){
        if (current_node.player == player_to_solve_for_){
            model_.add(realization_sum - *parent_var == 0);
        } else if (current_node.player > 0){
            dual_matrix_.push_back(dual_row);
        }
    }
    visited_information_set_[current_node.player].insert(current_information_set);
}

void ExtensiveFormSolver::TraverseNatureNode(int current_node_id, double probability, shared_ptr<IloNumVar> parent_var, int player1_parent_sequence, int player2_parent_sequence) {
    const Node& current_node = game_.nodes()[current_node_id];
    nature_nodes_visited_++;
    
    for (int child_idx = 0; child_idx < current_node.actions.size(); child_idx++){
        const Action& action = current_node.actions[child_idx];
        int child_node_id = action.child_id;
        if (!compute_abstraction_solution_ || abstraction_->GetPrototypeNodeId(child_node_id) == child_node_id) {
	    //int prototype_node_id = abstraction_->GetPrototypeNodeId(child_node_id);
            double probability_of_action = action.probability;
            if (compute_abstraction_solution_) {
                probability_of_action = abstraction_->GetActionProbability(current_node_id, child_idx);
            }
	    int debug = 9;
            TraverseTree(current_node.actions[child_idx].child_id, probability_of_action * probability, parent_var, player1_parent_sequence, player2_parent_sequence);
        } //else {
	//cout << "Abstracted: " << child_node_id << " to: " << abstraction_->GetPrototypeNodeId(child_node_id) << endl;
//	}
    }
}

void ExtensiveFormSolver::HandleLeafNode(int current_node, double probability, int player1_parent_sequence, int player2_parent_sequence){
    //cout << "Leaf probability: " << probability << ", leaf payoff: " << game_.nodes()[current_node].value << endl;
    leaf_nodes_visited_++;
    Leaf leaf;
    leaf.sequence1_index = player1_parent_sequence;
    leaf.sequence2_index = player2_parent_sequence;
    leaf.payoff1 = game_.nodes()[current_node].value;
    leaf.payoff2 = -game_.nodes()[current_node].value;
    leaf.probability = probability;
    if (player_to_solve_for_ == 1) {
	if (sequences_times_sequences_payoffs_[leaf.sequence2_index].count(leaf.sequence1_index) > 0) {
	    sequences_times_sequences_payoffs_[leaf.sequence2_index][leaf.sequence1_index] += leaf.payoff2 * probability;
	} else {
	    sequences_times_sequences_payoffs_[leaf.sequence2_index][leaf.sequence1_index] = leaf.payoff2 * probability;
	}
    } else {
	if (sequences_times_sequences_payoffs_[leaf.sequence1_index].count(leaf.sequence2_index) > 0) {
	    sequences_times_sequences_payoffs_[leaf.sequence1_index][leaf.sequence2_index] += leaf.payoff1 * probability;
	} else {
	    sequences_times_sequences_payoffs_[leaf.sequence1_index][leaf.sequence2_index] = leaf.payoff1 * probability;
	}
    }
    leaves_.push_back(leaf);
}

void ExtensiveFormSolver::CreateDualConstraints(){
    int num_constraints;

    if (player_to_solve_for_ == 1){
        num_constraints = game_.num_information_sets_player2()+1;
    } else {
        num_constraints = game_.num_information_sets_player1()+1;
    }

    map<int, vector<pair<int,double> > > inverted_dual_matrix;
    for (int constraint = 0; constraint < num_constraints; constraint++){
        for (pair<int,double> dual_seq : dual_matrix_[constraint]) {
            inverted_dual_matrix[dual_seq.first].push_back(pair<int,double>(constraint,dual_seq.second));
        }
    }

    int count_dual_adds = 0;
    map<int,map<int,double> >::iterator it_outer = sequences_times_sequences_payoffs_.begin();
    map<int,map<int,double> >::iterator end_outer = sequences_times_sequences_payoffs_.end();
    for (; it_outer != end_outer; it_outer++) {
        IloExpr rhs(env_);
        for (map<int,double>::iterator it_inner = it_outer->second.begin(); it_inner != it_outer->second.end(); it_inner++) {
            rhs += it_inner->second * (*strategy_vars_[it_inner->first]);
        }

        IloExpr lhs(env_);
        for (pair<int,double> constraint : inverted_dual_matrix[it_outer->first]){
            lhs += constraint.second * (*dual_vars_[constraint.first]);
        }

	model_.add(lhs >= rhs);
        /*if (it_outer->second.size() > 0 || inverted_dual_matrix[it_outer->first].size() > 0) {
            
	    count_dual_adds++;
	    }*/
    }
    //cout << "Dual adds: " << count_dual_adds << endl;
}

