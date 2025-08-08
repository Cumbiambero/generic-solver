#ifndef GENERIC_SOLVER_CLI_HPP
#define GENERIC_SOLVER_CLI_HPP

#include "solver.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void interactWithSolver(Solver& solver) {
    while (solver.getState() != SolverState::DONE) {
        for (string input; std::cout << "Solver > " && std::getline(std::cin, input);) {
            if (!input.empty()) {
                if (input == "exit") {
                    std::exit(solver.getState() == SolverState::DONE ? 0 : 1);
                }
                if (input == "help") {
                    std::cout << "You can use the following commands:\n"
                            "\texit\tTerminates the solver\n"
                            "\thelp\tPrints this output\n"
                            "\tshrink\tCleans the solutions cache\n"
                            "\tprint\tPrints the best solutions\n"
                            "\tstop\tRequests early stop and prints results\n";
                    continue;
                }
                if (input == "shrink") {
                    solver.shrink();
                    continue;
                }
                if (input == "print") {
                    solver.print();
                    continue;
                }
                if (input == "stop") {
                    solver.requestStop();
                    continue;
                }
                std::cout << input << " is not a supported command. Type help for a list of commands.\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(333));
    }
}

#endif