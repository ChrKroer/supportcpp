#include <string>
#include <vector>

class StringFunctions {
public:
    static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    static std::vector<std::string> Split(const std::string &s, char delim=' ');
    static std::vector<std::string> SplitWithQuotes(const std::string &s, char delim=' ');
};
