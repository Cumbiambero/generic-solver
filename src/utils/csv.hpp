#ifndef GENERIC_SOLVER_CSV_HPP
#define GENERIC_SOLVER_CSV_HPP

#include "base.hpp"
#include "config.hpp"
#include <fstream>

using namespace std;

[[nodiscard]] vector<vector<number>> parseCSV(const string &path) {
    ifstream input{path};
    if (!input.is_open()) {
        throw invalid_argument("Couldn't read file: " + path);
    }
    vector<vector<number>> result;

    for (string line; getline(input, line);) {
        istringstream ss(std::move(line));
        vector<number> row;
        if (!result.empty()) {
            row.reserve(result.front().size());
        }
        for (string value; getline(ss, value, CSV_DELIMITER);) {
            try {
                row.push_back(stoi(value));
            } catch (...) {
                throw invalid_argument("Incorrect content detected: " + value);
            }
        }
        result.push_back(std::move(row));
    }
    input.close();
    return result;
}

#endif