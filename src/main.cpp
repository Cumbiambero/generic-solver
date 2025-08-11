#include "utils/csv.hpp"
#include "solver/solver.hpp"
#include "solver/cli.hpp"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

int start(Solver& solver, bool noCli) {
    if (noCli) { // Run solver synchronously without CLI thread
        bool success = solver.start();
        return success ? 0 : 1;
    }
    bool solverResult = false;
    std::thread solverThread([&solver, &solverResult]() { solverResult = solver.start(); });
    std::thread commandLineThread([&solver]() { interactWithSolver(solver); });
    commandLineThread.join();
    solverThread.join();
    return solverResult ? 0 : 1;
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

        // Parse positional variables until we hit an option starting with '-'
        vector<Variable> variables;
        variables.reserve(static_cast<size_t>(argc - 3));
        int i = 3;
        for (; i < argc; ++i) {
            std::string_view tok(argv[i]);
            if (!tok.empty() && tok[0] == '-') break;
            variables.emplace_back(argv[i]);
        }

        bool useEnhanced = true;
        bool useUltra = false;
        bool noCli = false;
        number target = ALMOST_PERFECT;
        std::size_t threads = 0; // auto
        std::chrono::seconds timeLimit{0};

        // Optional flags
        for (; i < argc; ++i) {
            std::string arg(argv[i]);
            auto next = [&](int ahead = 1) -> const char* {
                if (i + ahead >= argc) throw std::invalid_argument("Missing value for " + arg);
                return argv[i + ahead];
            };
            if (arg == "--fitness") {
                std::string v(next());
                if (v == "basic") { useEnhanced = false; useUltra = false; }
                else if (v == "enhanced") { useEnhanced = true; useUltra = false; }
                else if (v == "ultra") { useEnhanced = true; useUltra = true; }
                else throw std::invalid_argument("--fitness must be basic|enhanced|ultra");
                ++i;
            } else if (arg == "--ultra") {
                useUltra = true; useEnhanced = true;
            } else if (arg == "--target") {
                target = std::stold(next());
                ++i;
            } else if (arg == "--time") {
                timeLimit = std::chrono::seconds(std::stoll(next()));
                ++i;
            } else if (arg == "--threads") {
                threads = static_cast<std::size_t>(std::stoull(next()));
                ++i;
            } else if (arg == "--no-cli") {
                noCli = true;
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "Usage: solver <input.csv> <results.csv> <var...> [options]\n"
                          << "Options:\n"
                          << "  --fitness <basic|enhanced|ultra>  Fitness function (default: enhanced)\n"
                          << "  --target <0..1>                   Early-stop target fitness (default: " << (double)ALMOST_PERFECT << ")\n"
                          << "  --time <seconds>                  Time limit; stops after N seconds\n"
                          << "  --threads <N>                     Number of worker threads (default: CPU-1)\n"
                          << "  --no-cli                          Disable interactive prompt (batch mode)\n"
                          << "  -h, --help                        Show this help\n";
                return 0;
            } else {
                throw std::invalid_argument("Unknown option: " + arg);
            }
        }

        std::cout << "Starting solver with " << variables.size() << " variable(s).";
        if (timeLimit.count() > 0) std::cout << " Time limit: " << timeLimit.count() << "s.";
        std::cout << " Target: " << (double)target << ".";
        std::cout << " Fitness: " << (useUltra ? "ultra" : (useEnhanced ? "enhanced" : "basic"))
                  << ". Threads: " << (threads ? threads : std::max(1u, std::thread::hardware_concurrency() - 1)) << "\n";

        Solver solver(std::move(variables), std::move(input), std::move(results),
                      useEnhanced, useUltra, target, threads, timeLimit);
    return start(solver, noCli);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}