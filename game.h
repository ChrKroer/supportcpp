#ifndef GAMEH
#define GAMEH
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>
#include <assert.h>

using namespace std;

class Action {
public:
    Action();
    ~Action();
    string name;
    int child_id;
    double probability;
    int rem;
};

class Node {
public:
    Node();
    ~Node();
    const bool IsLeaf() const { return player==-2; }
    string name;
    bool initiated;
    // players are 0,1, nature is -1, leaf is -2
    int player;
    bool public_signal;
    int player_receiving_signal;
    int information_set;
    int signal_group_player1;
    int signal_group_player2;
    std::vector<Action> actions;
    
    int value;
};

class ExtensiveFormGame {
public:
    ExtensiveFormGame();
    ExtensiveFormGame(std::string filename);
    ~ExtensiveFormGame();

    int root() { return root_; }
    int num_nature_nodes();
    int num_information_sets_player1();
    int num_information_sets_player2();
    int num_sequences_player1();
    int num_sequences_player2();
    int num_full_game_sequences_player1() { return num_sequences_[0]; }
    int num_full_game_sequences_player2() { return num_sequences_[1]; }

    int GetAbstractGameSize() { assert(abstraction_); return abstraction_num_nodes_; }
    
    const std::vector<string>& signals() { return signals_; }
    int num_rounds() { return num_rounds_; }
    int depth() { return depth_; }
    int num_private_signals() { return num_private_signals_; }
    
    const vector<vector<vector<int> > >& information_sets() const { return information_sets_; }
    const vector<Node>& nodes() const { return nodes_; }

    bool IsLeaf(int node_id) { return nodes_[node_id].player==-2; }
    void RecomputeInformation();
    void ComputeAbstractionInformation(const std::vector<int>& node_abstraction, const std::vector<std::vector<int> >& information_set_abstraction);
    void TurnOffAbstraction() { abstraction_ = false; }
    int GetChildNodeIdBySignalId(int parent_node, int signal_id) { return child_node_id_by_signal_id_[parent_node][signal_id]; }
    int GetActionIdBySignalId(int parent_node, int signal_id) { return action_id_by_signal_id_[parent_node][signal_id]; }
    bool NodeContainsSignal(int parent_node, int signal_id) { return action_id_by_signal_id_[parent_node].count(signal_id) > 0; }
private:
    // [player][info set index] returns vector of indices of the nodes in the information set
    std::vector<vector<std::vector<int> > > information_sets_;
    std::vector<vector<std::vector<Node> > > information_sets_with_nodes_;
    std::vector<vector<std::vector<Node> > > abstracted_information_sets_with_nodes_;
    std::vector<std::vector<bool> > information_sets_seen_;
    std::vector<Node> nodes_;
    std::vector<std::map<int, int> > child_node_id_by_signal_id_;
    std::vector<std::map<int, int> > action_id_by_signal_id_;

    bool abstraction_;
    
    // index of the root node
    int root_;
    int num_chance_histories_;
    int num_combined_player_histories_;
    int num_terminal_histories_;
    int num_nodes_;
    int num_information_sets_player1_;
    int num_information_sets_player2_;
    std::vector<int> num_sequences_;

    int abstraction_num_chance_histories_;
    int abstraction_num_combined_player_histories_;
    int abstraction_num_terminal_histories_;
    int abstraction_num_nodes_;
    int abstraction_num_information_sets_player1_;
    int abstraction_num_information_sets_player2_;
    std::vector<int> abstraction_num_sequences_;

    
    std::vector<string> signals_;
    std::map<string, int> signal_name_to_id_;
    int num_rounds_;
    int depth_;
    int num_private_signals_;

    void TraverseTreeAndUpdateInformation(int current_node_id, std::vector<std::vector<bool> >& seen_information_set);
    void UpdateInformationForNode(const Node& node, vector<vector<bool> >& seen_information_set);

    
    void TraverseTreeAndUpdateAbstractionInformation(int current_node_id, const std::vector<int>& node_abstraction, const std::vector<std::vector<int> >& information_set_abstraction, std::vector<std::vector<bool> >& seen_information_set);
    void UpdateAbstractionInformationForNode(const Node& node, const std::vector<std::vector<int> >& information_set_abstraction, vector<vector<bool> >& seen_information_set);
    
    void NodeSetGeneralInformation(vector<string> line);
    void CreateNatureNode(vector<string> line);
    void ReadGameInfoLine(const vector<string>& split_line);
    void ReadSignalTreeLine(const vector<string>& split_line);
    void ReadSignalsLine(const vector<string>& split_line);

    void CreateLeafNode(vector<string> line);
    void CreatePlayerNode(vector<string> line);
    void CreateGameFromFile(string filename);

    void TraverseTreeAndSetRemainingProbabilities(int current_node_id, int rem);
};

#endif
