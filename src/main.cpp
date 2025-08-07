#include "utils/csv.hpp"
#include "solver/solver.hpp"
#include "solver/cli.hpp"
#include <iostream>
#include <stdexcept>
#include <thread>

int start(Solver& solver) {
    std::thread solverThread([&solver]() { solver.start(); });
    std::thread commandLineThread([&solver]() { interactWithSolver(solver); });
    
    commandLineThread.join();
    solverThread.join();
    return 0;
}

int main(int argc, char** argv) {
    try {
        if (argc < 4) {
            throw std::invalid_argument(
                "Please provide 2 paths for input and result csv files followed by the variable names.");
        }

        const auto input = parseCSV(argv[1]);
        const auto results = parseCSV(argv[2]);

        if (input.size() != results.size()) {
            throw std::length_error("Both files need to have the same number of lines.");
        }

        vector<Variable> variables;
        variables.reserve(static_cast<size_t>(argc - 3));
        for (int i = 3; i < argc; ++i) {
            variables.emplace_back(argv[i]);
        }

        Solver solver(std::move(variables), std::move(input), std::move(results));
        return start(solver);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}