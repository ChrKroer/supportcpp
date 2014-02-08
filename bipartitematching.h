#ifndef BIPARTITEMATCHINGH
#define BIPARTITEMATCHINGH

#include <ilcplex/ilocplex.h>
#include <map>
#include <vector>
#include <set>
ILOSTLBEGIN


class BipartiteMatching {
public:
    // [node_id1][node_id2] returns the cost of mapping node 1 from group 1 to node 2 from group 2
    BipartiteMatching(int num_vertices, std::vector<std::vector<double> > costs);
    ~BipartiteMatching();

    double optimal_weight() { return optimal_weight_; }
    bool IsFeasible();
    
    // [node_id1] returns the node from the second group that node_id1 from group 1 maps to
    const std::vector<int> matchings1() const { return matchings1_; }
    // [node_id2] returns the node from the first group that node_id2 from group 2 maps to
    const std::vector<int> matchings2() const { return matchings2_; }
    
private:
    // output
    std::vector<int> matchings1_;
    std::vector<int> matchings2_;
    double optimal_weight_;

    // auxiliary
    // [element1][element2] returns IP var describing whether node1 is mapped to node2
    std::map<int, std::map<int, shared_ptr<IloNumVar> > > matching_variables_;

    IloEnv env_;
    IloModel model_;
    IloCplex cplex_;

    void SetUp();
    void CreateVariables();
    void CreateConstraints();
    void Solve();
    void SetMatchings();
    
    // input
    int num_vertices_;
    std::vector<std::vector<double> > costs_;
};

#endif
