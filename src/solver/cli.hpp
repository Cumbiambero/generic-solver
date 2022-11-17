#ifndef GENERIC_SOLVER_CLI_HPP
#define GENERIC_SOLVER_CLI_HPP

#include "solver.hpp"

void interactWithSolver(Solver &solver) {
    while (solver.getState() != SolverState::DONE) {
        for (std::string input; std::cout << "Solver > " && std::getline(std::cin, input);) {
            if (!input.empty()) {
                if ("exit" == input) {
                    exit(solver.getState() == SolverState::DONE ? 0 : 1);
                }
                if ("help" == input) {
                    cout << "You can use the following commands:\n"
                            "\texit\tTerminates the solver\n"
                            "\thelp\tPrints this output\n"
                            "\tprint\tPrints the best solutions\n";
                    continue;
                }
                if ("print" == input) {
                    solver.print();
                    continue;
                }
                cout << input << " is not a supported command. Type help for a list of commands.\n";
            }
        }
        this_thread::sleep_for(std::chrono::milliseconds(333));
    }
}

#endif