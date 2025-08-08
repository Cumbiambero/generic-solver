#ifndef GENERIC_SOLVER_CONFIG_HPP
#define GENERIC_SOLVER_CONFIG_HPP

#include <cstdint>

// CSV parsing
static constexpr char CSV_DELIMITER = ',';

// Algorithm tuning
static constexpr number ALMOST_PERFECT = 0.9999999999L;
static constexpr number EPSILON_FOR_RATE = 1.0L - ALMOST_PERFECT;
static constexpr std::size_t SOLUTIONS_SIZE = 1000;
static constexpr std::size_t HALL_OF_FAME_SIZE = 3;
static constexpr std::size_t STAGNATION_THRESHOLD = 200;
static constexpr std::size_t RANDOM_INJECTION_COUNT = 500;
static constexpr std::size_t AGGRESSIVE_STAGNATION_THRESHOLD = 100;

// Display formatting
static constexpr std::size_t FORMULA_WIDTH = 70;
static constexpr std::size_t FORMULA_NUMBER_PRECISION = 8;
static constexpr std::size_t NUMBER_OF_RESULTS = 20;
static constexpr std::size_t RATE_WIDTH = 10;
static constexpr std::size_t RATE_PRECISION = RATE_WIDTH - 2;

// Numerical stability
static constexpr number EPSILON = 1e-6L;
static constexpr number DIVISION_BY_ZERO_THRESHOLD = 1e-15L;

#endif