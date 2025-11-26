#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Converts algebraic notation (e.g., "e4") to square index (0-63).
// Returns -1 on failure.
int parseSquare(const std::string& s);

// Converts square index (0-63) to algebraic notation (e.g., "e4").
std::string squareToString(int square);

// Helper to split a string by a delimiter
std::vector<std::string> split(const std::string& s, char delimiter);

// Helper to trim whitespace
std::string trim(const std::string& str);

#endif // UTILS_H
