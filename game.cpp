#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include "game.h"
#include "stringfunctions.h"

using namespace std;



Action::Action() {

}

Action::~Action() {

}

Node::Node() : initiated(false) {

}

Node::~Node() {

}



ExtensiveFormGame::ExtensiveFormGame() : abstraction_(false) {

}

ExtensiveFormGame::ExtensiveFormGame(string filename) : abstraction_(false) {
    CreateGameFromFile(filename);
}

ExtensiveFormGame::~ExtensiveFormGame() {

}

int ExtensiveFormGame::num_nature_nodes() {
    if (!abstraction_) {
	return num_chance_histories_; 
    } else {
	return abstraction_num_chance_histories_;
    }
}

int ExtensiveFormGame::num_information_sets_player1() {
    if (!abstraction_) {
	return num_information_sets_player1_;
    } else {
	return abstraction_num_information_sets_player1_;
    }
}

int ExtensiveFormGame::num_information_sets_player2() {
    if (!abstraction_) {
	return num_information_sets_player2_;
    } else {
	return abstraction_num_information_sets_player2_;
    }
}

int ExtensiveFormGame::num_sequences_player1() {
    if (!abstraction_) {
	return num_sequences_[0];
    } else {
	return abstraction_num_sequences_[0];
    }
}

int ExtensiveFormGame::num_sequences_player2() {
    if (!abstraction_) {
	return num_sequences_[1];
    } else {
	return abstraction_num_sequences_[1];
    }
}

void ExtensiveFormGame::RecomputeInformation() {
    vector<vector<bool> > seen_information_set;
    vector<bool> information_set_bool_vector1(num_information_sets_player1_, false);
    vector<bool> information_set_bool_vector2(num_information_sets_player2_, false);


    num_chance_histories_ = 0;
    num_combined_player_histories_ = 0;
    num_terminal_histories_ = 0;
    num_nodes_ = 0;
    num_information_sets_player1_ = 0;
    num_information_sets_player2_ = 0;

    //num_sequences_.resize(2,1);
    num_sequences_[0] = 1;
    num_sequences_[1] = 1;
    



    seen_information_set.push_back(information_set_bool_vector1);
    seen_information_set.push_back(information_set_bool_vector2);
    
    TraverseTreeAndUpdateInformation(root_, seen_information_set);
}

void ExtensiveFormGame::TraverseTreeAndUpdateInformation(int current_node_id, vector<vector<bool> >& seen_information_set) {
    const Node& node = nodes_[current_node_id];
    UpdateInformationForNode(node, seen_information_set);
    
    
    // recurse
    for(int i = 0; i < node.actions.size(); i++){
	int child_id = node.actions[i].child_id;
	TraverseTreeAndUpdateInformation(child_id, seen_information_set);
    }
}

void ExtensiveFormGame::UpdateInformationForNode(const Node& node, vector<vector<bool> >& seen_information_set) {
    num_nodes_++;

    if (node.player == -2) {
	num_terminal_histories_++;
    } else if (node.player == 0) {
	num_chance_histories_++;
    } else {
	num_combined_player_histories_++;
	if (!seen_information_set[node.player-1][node.information_set]) {
	    if (node.player == 1) {
		num_information_sets_player1_++;
	    } else {
		num_information_sets_player2_++;
	    }

	    // this is valid, since we do not abstract player actions
	    num_sequences_[node.player-1] += node.actions.size();
	    seen_information_set[node.player-1][node.information_set] = true;
	}
    }
}


void ExtensiveFormGame::ComputeAbstractionInformation(const vector<int>& node_abstraction, const vector<vector<int> >& information_set_abstraction) {
    abstraction_ = true;
    
    abstraction_num_chance_histories_ = 0;
    abstraction_num_combined_player_histories_ = 0;
    abstraction_num_terminal_histories_ = 0;
    abstraction_num_nodes_ = 0;
    abstraction_num_information_sets_player1_ = 0;
    abstraction_num_information_sets_player2_ = 0;

    abstraction_num_sequences_.resize(2,1);

    vector<vector<bool> > seen_information_set;
    vector<bool> information_set_bool_vector1(num_information_sets_player1_, false);
    vector<bool> information_set_bool_vector2(num_information_sets_player2_, false);

    abstracted_information_sets_with_nodes_.resize(2);
    abstracted_information_sets_with_nodes_[0].resize(num_information_sets_player1_);
    abstracted_information_sets_with_nodes_[1].resize(num_information_sets_player2_);


    seen_information_set.push_back(information_set_bool_vector1);
    seen_information_set.push_back(information_set_bool_vector2);
    
    TraverseTreeAndUpdateAbstractionInformation(root_, node_abstraction, information_set_abstraction, seen_information_set);
}

void ExtensiveFormGame::TraverseTreeAndUpdateAbstractionInformation(int current_node_id, const vector<int>& node_abstraction, const vector<vector<int> >& information_set_abstraction, vector<vector<bool> >& seen_information_set) {
    const Node& node = nodes_[current_node_id];
    UpdateAbstractionInformationForNode(node, information_set_abstraction, seen_information_set);
    
    
    // recurse
    for(int i = 0; i < node.actions.size(); i++){
	int child_id = node.actions[i].child_id;
	if (node_abstraction[child_id] == child_id) {
	    TraverseTreeAndUpdateAbstractionInformation(child_id, node_abstraction, information_set_abstraction, seen_information_set);
	}
    }
}

void ExtensiveFormGame::UpdateAbstractionInformationForNode(const Node& node, const vector<vector<int> >& information_set_abstraction, vector<vector<bool> >& seen_information_set) {
    abstraction_num_nodes_++;

    if (node.player == -2) {
	abstraction_num_terminal_histories_++;
    } else if (node.player == 0) {
	abstraction_num_chance_histories_++;
    } else {
	abstraction_num_combined_player_histories_++;
	int abstract_information_set = information_set_abstraction[node.player][node.information_set];
	abstracted_information_sets_with_nodes_[node.player-1][abstract_information_set].push_back(node);
	if (!seen_information_set[node.player-1][information_set_abstraction[node.player][node.information_set]]) {
	    if (node.player == 1) {
		abstraction_num_information_sets_player1_++;
	    } else {
		abstraction_num_information_sets_player2_++;
	    }

	    // this is valid, since we do not abstract player actions
	    abstraction_num_sequences_[node.player-1] += node.actions.size();
	    seen_information_set[node.player-1][information_set_abstraction[node.player][node.information_set]] = true;
	}
    }
}

void ExtensiveFormGame::CreateGameFromFile(string filename){
    ifstream file(filename);
    num_sequences_.resize(2,1);
    
    
    string line;
    while (std::getline(file, line)) {
	boost::trim_if(line, boost::is_any_of("\t "));
	std::vector<std::string> split_line = StringFunctions::SplitWithQuotes(line, ' ');
	if (line[0] == '#') {
	    continue;
	} else if (split_line[0] == "game info") {
	    ReadGameInfoLine(split_line);
	} else if (split_line[0] == "signals") {
	    ReadSignalsLine(split_line);
	} else if (split_line[0] == "signal data") {
	    ReadSignalTreeLine(split_line);
	} else {	
	    if (split_line.size() == 3 || split_line.size() == 5) {
		CreateLeafNode(split_line);
	    } else if (split_line[5][0] == 'n') { // nature node
		CreateNatureNode(split_line);
	    } else { // player node
		CreatePlayerNode(split_line);
	    }
	}
    }


    //TraverseTreeAndSetRemainingProbabilities(root_, 0);
}

void ExtensiveFormGame::ReadGameInfoLine(const vector<string>& split_line) {
    num_chance_histories_ = stoi(split_line[1]);
    num_combined_player_histories_ = stoi(split_line[2]);
    num_terminal_histories_ = stoi(split_line[3]);
    num_nodes_ = stoi(split_line[5])+1;
    num_information_sets_player1_ = stoi(split_line[7]);
    num_information_sets_player2_ = stoi(split_line[8]);

    information_sets_.resize(2);
    information_sets_with_nodes_.resize(2);
    information_sets_[0].resize(num_information_sets_player1_);
    information_sets_[1].resize(num_information_sets_player2_);
    information_sets_with_nodes_[0].resize(num_information_sets_player1_);
    information_sets_with_nodes_[1].resize(num_information_sets_player2_);
    information_sets_seen_.resize(2);
    information_sets_seen_[0].resize(num_information_sets_player1_, false);
    information_sets_seen_[1].resize(num_information_sets_player2_, false);
    nodes_.resize(num_nodes_);
    child_node_id_by_signal_id_.resize(num_nodes_);
    action_id_by_signal_id_.resize(num_nodes_);
}

void ExtensiveFormGame::ReadSignalTreeLine(const vector<string>& split_line) {
    num_rounds_ = stoi(split_line[1]);
    depth_ = 2*(stoi(split_line[2])) + stoi(split_line[3]);
    num_private_signals_ = stoi(split_line[2]);
}

void ExtensiveFormGame::ReadSignalsLine(const vector<string>& split_line) {
    for (int i = 1; i < split_line.size(); i++) {
	signals_.push_back(split_line[i]);
	signal_name_to_id_[split_line[i]] = i-1;
    }
}


void ExtensiveFormGame::CreateLeafNode(vector<string> line) {
    int node_id = stoi(line[0]);
    NodeSetGeneralInformation(line);
    nodes_[node_id].player = -2;
    nodes_[node_id].value = stoi(line[2]);
    if (line.size() == 5) {
	nodes_[node_id].signal_group_player1 = stoi(line[3]);
	nodes_[node_id].signal_group_player2 = stoi(line[4]);
    }
}

void ExtensiveFormGame::CreatePlayerNode(vector<string> line) {
    int node_id = stoi(line[0]);
    int player = stoi(line[2])+1;
    int information_set = stoi(line[3]);
    NodeSetGeneralInformation(line);
    nodes_[node_id].player = player;
    nodes_[node_id].information_set = information_set;
    information_sets_[player-1][information_set].push_back(node_id);
    information_sets_with_nodes_[player-1][information_set].push_back(nodes_[node_id]);
    
    int num_actions = stoi(line[4]);
    nodes_[node_id].actions.resize(num_actions);
    for (int i = 0; i < num_actions; i++) {
	if (!information_sets_seen_[player-1][information_set]){
	    num_sequences_[player-1]++;
	}
	nodes_[node_id].actions[i].name = line[5+2*i];
	int child_id = stoi(line[6+2*i]);
	nodes_[node_id].actions[i].child_id = child_id;
	//parent_indices_[child_id] = node_id;
    }
    information_sets_seen_[player-1][information_set] = true;
    if (nodes_[node_id].name == "/") {
	//cout << "found root: " << node_id << endl;
	root_ = node_id;
    }
    nodes_[node_id].initiated = true;
}

void ExtensiveFormGame::CreateNatureNode(vector<string> line) {
    int node_id = stoi(line[0]);
    NodeSetGeneralInformation(line);
    nodes_[node_id].player = 0;
    vector<string> split_name = StringFunctions::Split(line[5], ':');
    if (split_name[1] == "a") {
	nodes_[node_id].public_signal = true;
    } else {
	nodes_[node_id].public_signal = false;
	nodes_[node_id].player_receiving_signal = stoi(split_name[1])+1;
    }

    nodes_[node_id].signal_group_player1 = stoi(line[2]);
    nodes_[node_id].signal_group_player2 = stoi(line[3]);
    
    int num_actions = stoi(line[4]);
    nodes_[node_id].actions.resize(num_actions);
    double sum = 0;
    for (int i = 0; i < num_actions; i++) {
	string name = line[5+3*i];
	string signal_name = StringFunctions::Split(name, ':')[2];
	nodes_[node_id].actions[i].name = name;
	int child_id = stoi(line[6+3*i]);
	nodes_[node_id].actions[i].child_id = child_id;
	nodes_[node_id].actions[i].rem = stoi(line[7+3*i]);
	sum += nodes_[node_id].actions[i].rem;
	child_node_id_by_signal_id_[node_id][signal_name_to_id_[signal_name]] = child_id;
	action_id_by_signal_id_[node_id][signal_name_to_id_[signal_name]] = i;
    }

    for (int i = 0; i < num_actions; i++) {
	nodes_[node_id].actions[i].probability = (double)nodes_[node_id].actions[i].rem / sum;
    }

    // locate root node
    if (nodes_[node_id].name == "/") {
	root_ = node_id;
    }
}


void ExtensiveFormGame::NodeSetGeneralInformation(vector<string> line) {
    int node_id = stoi(line[0]);
    nodes_[node_id].name = line[1];
}
			    
void ExtensiveFormGame::TraverseTreeAndSetRemainingProbabilities(int current_node_id, int rem) {
    for(int i = 0; i < nodes_[current_node_id].actions.size(); i++){
	if (current_node_id != root_ && nodes_[current_node_id].player == 0 && nodes_[current_node_id].actions[i].rem != rem){ // fix probabilities
	    nodes_[current_node_id].actions[i].probability = (double)nodes_[current_node_id].actions[i].rem / rem;
	} else {
	    nodes_[current_node_id].actions[i].probability = (double)1/nodes_[current_node_id].actions.size();
	    //nodes_[current_node_id].actions[i].probability = (double)nodes_[current_node_id].actions[i].rem / rem;
	}


	if (nodes_[current_node_id].player == 0) {
	    TraverseTreeAndSetRemainingProbabilities(nodes_[current_node_id].actions[i].child_id, nodes_[current_node_id].actions[i].rem);
	} else {
	    TraverseTreeAndSetRemainingProbabilities(nodes_[current_node_id].actions[i].child_id, rem);
	}
    }
}
