#ifndef SEQUENCEFORMH
#define SEQUENCEFORMH

#include <string>


class Sequence{
public:
    int node;
    std::string action_name;
    double probability_on_path;
    double payoff1;
    double payoff2;
};

class Leaf{
public:
    int sequence1_index;
    int sequence2_index;
    double payoff1;
    double payoff2;
    double probability;
};

#endif
