#include "utils/csv.hpp"
#include "solver/solver.hpp"
#include "solver/cli.hpp"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iomanip>

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
        std::string outFormat = "human"; // human | json | csv
        std::string outPath; // empty -> stdout

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
            } else if (arg == "--format") {
                std::string v(next());
                if (v == "human" || v == "json" || v == "csv") outFormat = v;
                else throw std::invalid_argument("--format must be human|json|csv");
                ++i;
            } else if (arg == "--output") {
                outPath = std::string(next());
                ++i;
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "Usage: solver <input.csv> <results.csv> <var...> [options]\n"
                          << "Options:\n"
                          << "  --fitness <basic|enhanced|ultra>  Fitness function (default: enhanced)\n"
                          << "  --target <0..1>                   Early-stop target fitness (default: " << (double)ALMOST_PERFECT << ")\n"
                          << "  --time <seconds>                  Time limit; stops after N seconds\n"
                          << "  --threads <N>                     Number of worker threads (default: CPU-1)\n"
                          << "  --no-cli                          Disable interactive prompt (batch mode)\n"
                          << "  --format <human|json|csv>         Output format (default: human)\n"
                          << "  --output <file>                   Write output to file (default: stdout)\n"
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

        const bool batchMode = noCli || (outFormat != "human");
        const int rc = start(solver, batchMode);

        // small JSON escape helper
        auto escape_json = [](const std::string& s) {
            std::ostringstream o;
            for (char ch : s) {
                unsigned char c = static_cast<unsigned char>(ch);
                switch (c) {
                    case '"': o << "\\\""; break;
                    case '\\': o << "\\\\"; break;
                    case '\b': o << "\\b"; break;
                    case '\f': o << "\\f"; break;
                    case '\n': o << "\\n"; break;
                    case '\r': o << "\\r"; break;
                    case '\t': o << "\\t"; break;
                    default:
                        if (c < 0x20) {
                            o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                        } else o << c;
                }
            }
            return o.str();
        };

        auto escape_csv = [](const std::string& s) {
            // Escape quotes by doubling them, and wrap field in quotes
            std::string out;
            out.reserve(s.size() + 2);
            for (char ch : s) {
                if (ch == '"') out.push_back('"'), out.push_back('"');
                else out.push_back(ch);
            }
            return out;
        };

        if (outFormat == "json" || outFormat == "csv") {
            std::ostringstream oss;
            const auto bestSet = solver.hallOfFameCopy();
            if (!bestSet.empty()) {
                const auto best = *bestSet.rbegin();
                if (outFormat == "json") {
                    oss << "{\n";
                    oss << "  \"metrics\": { \"duration_seconds\": " << solver.elapsed_seconds() << ", \"threads\": " << solver.threads() << " },\n";
                    oss << "  \"result\": { \"formula\": \"" << escape_json(best.getFormula().toString()) << "\", \"score\": " << best.getRate() << " }\n";
                    oss << "}\n";
                } else {
                    oss << "formula,score,duration_seconds\n";
                    oss << '"' << escape_csv(best.getFormula().toString()) << "\"," << best.getRate() << ',' << solver.elapsed_seconds() << "\n";
                }
            } else {
                if (outFormat == "json") oss << "{ \"metrics\": { \"duration_seconds\": " << solver.elapsed_seconds() << " }, \"result\": null }\n";
            }

            if (outPath.empty()) std::cout << oss.str();
            else {
                std::ofstream ofs(outPath);
                if (!ofs) std::cerr << "Failed to open output file: " << outPath << "\n";
                else ofs << oss.str();
            }
        }

        return rc;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}