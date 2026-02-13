#ifndef GENERIC_SOLVER_RUNTIME_CONFIG_HPP
#define GENERIC_SOLVER_RUNTIME_CONFIG_HPP

#include "base.hpp"
#include "config.hpp"
#include <optional>
#include <set>
#include <fstream>
#include <sstream>

/**
 * Runtime configuration for the genetic algorithm solver.
 * This allows customization of algorithm behavior without recompilation.
 */
struct RuntimeConfig {
    // Population settings
    std::size_t populationSize = SOLUTIONS_SIZE;
    std::size_t hallOfFameSize = HALL_OF_FAME_SIZE;
    
    // Stagnation handling
    std::size_t stagnationThreshold = STAGNATION_THRESHOLD;
    std::size_t aggressiveStagnationThreshold = AGGRESSIVE_STAGNATION_THRESHOLD;
    std::size_t randomInjectionCount = RANDOM_INJECTION_COUNT;
    
    // Formula constraints
    std::optional<std::size_t> maxFormulaDepth = std::nullopt;
    std::optional<std::size_t> maxFormulaNodes = std::nullopt;
    std::optional<std::size_t> maxFormulaLength = std::nullopt;
    
    // Operator enablement
    bool enableAddition = true;
    bool enableSubtraction = true;
    bool enableMultiplication = true;
    bool enableDivision = true;
    bool enablePower = true;
    bool enableModulo = true;
    
    bool enableTrigonometric = true;
    bool enableHyperbolic = true;
    bool enableExponential = true;
    bool enableLogarithmic = true;
    bool enableRounding = true;
    bool enableRootFunctions = true;
    
    // Numeric range constraints
    std::optional<number> minConstantValue = std::nullopt;
    std::optional<number> maxConstantValue = std::nullopt;
    
    // Fitness function weights (for enhanced fitness)
    number accuracyWeight = 0.7L;
    number complexityWeight = 0.1L;
    number rangeConsistencyWeight = 0.1L;
    number monotonicityWeight = 0.1L;
    
    // Complexity penalty parameters
    number complexityPenaltyFactor = 0.001L;
    
    // Mutation rates
    number mutationProbability = 0.9L;
    number crossoverProbability = 0.1L;
    
    // Stopping criteria
    std::size_t maxIterations = 200000;
    bool stopOnPerfectMatch = true;
    bool stopOnHallOfFameFull = true;
    
    // Progress reporting
    std::size_t progressReportInterval = 1000; // iterations
    bool verboseLogging = false;
    
    /**
     * Load configuration from a simple key=value file format
     */
    static RuntimeConfig loadFromFile(const std::string& path) {
        RuntimeConfig config;
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::invalid_argument("Cannot open config file: " + path);
        }
        
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') continue;
            
            // Remove inline comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Parse key=value
            size_t eqPos = line.find('=');
            if (eqPos == std::string::npos) continue;
            
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));
            
            try {
                config.setValue(key, value);
            } catch (const std::exception& e) {
                throw std::invalid_argument("Error parsing config at line " + 
                    std::to_string(lineNum) + ": " + e.what());
            }
        }
        
        return config;
    }
    
    /**
     * Set a configuration value by key name
     */
    void setValue(const std::string& key, const std::string& value) {
        if (key == "populationSize") populationSize = parseSize(value);
        else if (key == "hallOfFameSize") hallOfFameSize = parseSize(value);
        else if (key == "stagnationThreshold") stagnationThreshold = parseSize(value);
        else if (key == "aggressiveStagnationThreshold") aggressiveStagnationThreshold = parseSize(value);
        else if (key == "randomInjectionCount") randomInjectionCount = parseSize(value);
        
        else if (key == "maxFormulaDepth") maxFormulaDepth = parseSize(value);
        else if (key == "maxFormulaNodes") maxFormulaNodes = parseSize(value);
        else if (key == "maxFormulaLength") maxFormulaLength = parseSize(value);
        
        else if (key == "enableAddition") enableAddition = parseBool(value);
        else if (key == "enableSubtraction") enableSubtraction = parseBool(value);
        else if (key == "enableMultiplication") enableMultiplication = parseBool(value);
        else if (key == "enableDivision") enableDivision = parseBool(value);
        else if (key == "enablePower") enablePower = parseBool(value);
        else if (key == "enableModulo") enableModulo = parseBool(value);
        
        else if (key == "enableTrigonometric") enableTrigonometric = parseBool(value);
        else if (key == "enableHyperbolic") enableHyperbolic = parseBool(value);
        else if (key == "enableExponential") enableExponential = parseBool(value);
        else if (key == "enableLogarithmic") enableLogarithmic = parseBool(value);
        else if (key == "enableRounding") enableRounding = parseBool(value);
        else if (key == "enableRootFunctions") enableRootFunctions = parseBool(value);
        
        else if (key == "minConstantValue") minConstantValue = parseNumber(value);
        else if (key == "maxConstantValue") maxConstantValue = parseNumber(value);
        
        else if (key == "accuracyWeight") accuracyWeight = parseNumber(value);
        else if (key == "complexityWeight") complexityWeight = parseNumber(value);
        else if (key == "rangeConsistencyWeight") rangeConsistencyWeight = parseNumber(value);
        else if (key == "monotonicityWeight") monotonicityWeight = parseNumber(value);
        else if (key == "complexityPenaltyFactor") complexityPenaltyFactor = parseNumber(value);
        
        else if (key == "mutationProbability") mutationProbability = parseNumber(value);
        else if (key == "crossoverProbability") crossoverProbability = parseNumber(value);
        
        else if (key == "maxIterations") maxIterations = parseSize(value);
        else if (key == "stopOnPerfectMatch") stopOnPerfectMatch = parseBool(value);
        else if (key == "stopOnHallOfFameFull") stopOnHallOfFameFull = parseBool(value);
        
        else if (key == "progressReportInterval") progressReportInterval = parseSize(value);
        else if (key == "verboseLogging") verboseLogging = parseBool(value);
        
        else {
            throw std::invalid_argument("Unknown configuration key: " + key);
        }
    }
    
private:
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
    static std::size_t parseSize(const std::string& value) {
        return static_cast<std::size_t>(std::stoull(value));
    }
    
    static number parseNumber(const std::string& value) {
        return std::stold(value);
    }
    
    static bool parseBool(const std::string& value) {
        std::string lower = value;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") return true;
        if (lower == "false" || lower == "0" || lower == "no" || lower == "off") return false;
        throw std::invalid_argument("Invalid boolean value: " + value);
    }
};

#endif
