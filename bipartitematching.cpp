#include "bipartitematching.h"

BipartiteMatching::BipartiteMatching(int num_vertices, vector<vector<double> > costs) : num_vertices_(num_vertices), costs_(costs), env_(IloEnv()), model_(IloModel(env_)), cplex_(IloCplex(model_)) {
    SetUp();
    Solve();
    SetMatchings();
}

BipartiteMatching::~BipartiteMatching() {

}

bool BipartiteMatching::IsFeasible() {
    if (cplex_.getStatus() == IloAlgorithm::Optimal){
	return true;
    } else if (cplex_.getStatus() == IloAlgorithm::Infeasible) {
	return false;
    } else {
	cout << "BipartiteMatching::IsFeasible error: IP is neither optimal nor infeasible" << endl;
	return false;
    }
}

void BipartiteMatching::SetUp() {
    cplex_.setParam(IloCplex::SimDisplay, 0);
    cplex_.setParam(IloCplex::MIPDisplay, 0);
    cplex_.setParam(IloCplex::TuningDisplay, 0);
    cplex_.setParam(IloCplex::BarDisplay, 0);
    cplex_.setParam(IloCplex::SiftDisplay, 0);
    cplex_.setParam(IloCplex::ConflictDisplay, 0);
    cplex_.setParam(IloCplex::NetDisplay, 0);
    cplex_.setParam(IloCplex::TiLim, 1e+75);

    matchings1_.resize(num_vertices_);
    matchings2_.resize(num_vertices_);    
    
    CreateVariables();
    CreateConstraints();

    cplex_.exportModel("/Users/ckroer/Documents/research/abstraction-extensive-form-games/bipartitematching.lp");
}

void BipartiteMatching::CreateVariables() {
    IloObjective obj(env_, 0, IloObjective::Minimize);



    IloExpr sum_to_num_buckets(env_);
    for (int i = 0; i < num_vertices_; i++) {
	for (int j = 0; j < num_vertices_; j++) {
	    if (costs_[i][j] >= 0) {
		std::stringstream ss_mapping_var;
		ss_mapping_var << "I(" << i << "," << j << ")";
		shared_ptr<IloNumVar>  mapping_var(new IloNumVar(env_, 0, 1, ILOBOOL, ss_mapping_var.str().c_str()));
		matching_variables_[i][j] = mapping_var;
		obj.setLinearCoef(*mapping_var, costs_[i][j]);
	    }
	}
    }
    model_.add(obj);
}

void BipartiteMatching::CreateConstraints() {
    for (int i = 0; i < num_vertices_; i++) {
	IloExpr sum_to_one(env_);
	for (int j = 0; j < num_vertices_; j++) {
	    if (costs_[i][j] >= 0) {
		sum_to_one += *matching_variables_[i][j];
	    }
	}
	model_.add(sum_to_one == 1);
    }    
    for (int j = 0; j < num_vertices_; j++) {
	IloExpr sum_to_one(env_);
	for (int i = 0; i < num_vertices_; i++) {
	    if (costs_[i][j] >= 0) {
		sum_to_one += *matching_variables_[i][j];
	    }
	}
	model_.add(sum_to_one == 1);
    }    
}

void BipartiteMatching::Solve() {
    cplex_.solve();
    optimal_weight_ = cplex_.getObjValue();
    /*if (cplex_.getStatus() == IloAlgorithm::Unbounded) {
	cout << "-------------- UNBOUNDED ------------------" << endl;
    } else if(cplex_.getStatus() == IloAlgorithm::Optimal){
        cout << "status: optimal" << endl;
        double objective_value = cplex_.getObjValue();
        if (objective_value == -0) objective_value = 0;
        cout << "Obj val: " << objective_value << endl;
    } else if (cplex_.getStatus() == IloAlgorithm::Infeasible) {
	cout << "-------------- INFEASIBLE ------------------" << endl;
    } else if (cplex_.getStatus() == IloAlgorithm::Error) {
	cout << "-------------- CPLEX ERROR ------------------" << endl;
    } else {
	cout << "-------------- Defaulted ------------------" << endl;
	}*/

}

void BipartiteMatching::SetMatchings() {
    for (int i = 0; i < num_vertices_; i++) {
	for (int j = 0; j < num_vertices_; j++) {
	    if (cplex_.getValue(*matching_variables_[i][j]) == 1) {
		matchings1_[i] = j;
		matchings2_[j] = i;
	    }
	}
    }    
}
