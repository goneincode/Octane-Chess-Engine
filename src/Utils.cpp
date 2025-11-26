#include "Utils.h"
#include <sstream>
#include <algorithm>

int parseSquare(const std::string& s) {
    if (s.length() != 2) return -1;
    int file = s[0] - 'a';
    int rank = s[1] - '1';
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;
    return rank * 8 + file;
}

std::string squareToString(int square) {
    if (square < 0 || square > 63) return "-";
    int rank = square / 8;
    int file = square % 8;
    return std::string(1, 'a' + file) + std::to_string(rank + 1);
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
