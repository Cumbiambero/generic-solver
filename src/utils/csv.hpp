#ifndef GENERIC_SOLVER_CSV_HPP
#define GENERIC_SOLVER_CSV_HPP

#include "base.hpp"
#include "config.hpp"
#include <fstream>
#include <stdexcept>

[[nodiscard]] vector<vector<number>> parseCSV(const string& path) {
    std::ifstream input{path};
    if (!input.is_open()) {
        throw std::invalid_argument("Couldn't read file: " + path);
    }
    
    vector<vector<number>> result;
    result.reserve(1000); // Reserve space for better performance

    for (string line; std::getline(input, line);) {
        if (line.empty()) continue; // Skip empty lines
        
        std::istringstream ss(std::move(line));
        vector<number> row;
        
        if (!result.empty()) {
            row.reserve(result.front().size());
        }
        
        for (string value; std::getline(ss, value, CSV_DELIMITER);) {
            try {
                row.emplace_back(std::stold(value));
            } catch (const std::exception&) {
                throw std::invalid_argument("Incorrect content detected: " + value);
            }
        }
        
        if (!row.empty()) {
            result.emplace_back(std::move(row));
        }
    }
    
    return result;
}

#endif