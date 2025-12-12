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
    result.reserve(1000);

    bool headerSkipped = false;

    for (string line; std::getline(input, line);) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        vector<number> row;

        if (!result.empty()) {
            row.reserve(result.front().size());
        }

        bool parseError = false;
        vector<string> tokens;
        while (true) {
            string value;
            if (!std::getline(ss, value, CSV_DELIMITER)) break;
            tokens.push_back(value);
        }

        string firstFailingToken;
        for (const auto &value : tokens) {
            try {
                row.emplace_back(std::stold(value));
            } catch (const std::exception&) {
                parseError = true;
                if (firstFailingToken.empty()) firstFailingToken = value;
                break;
            }
        }

        if (parseError) {
            // If this row contains ONLY non-numeric tokens and we haven't skipped
            // a header yet, treat it as a header and ignore it. Otherwise, it's
            // malformed numeric content and we should throw.
            bool anyNumeric = false;
            for (const auto &value : tokens) {
                try {
                    std::stold(value);
                    anyNumeric = true;
                    break;
                } catch (...) {
                    // not numeric
                }
            }
            if (!headerSkipped && result.empty() && !anyNumeric) {
                headerSkipped = true;
                continue;
            }
            throw std::invalid_argument("Incorrect content detected: " + firstFailingToken);
        }

        if (!row.empty()) {
            result.emplace_back(std::move(row));
        }
    }

    return result;
}

#endif