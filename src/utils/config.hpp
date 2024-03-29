#ifndef GENERIC_SOLVER_CONFIG_HPP
#define GENERIC_SOLVER_CONFIG_HPP

static const char CSV_DELIMITER = ',';
static const double ALMOST_PERFECT = 0.9999999999;
static const double EPSILON_FOR_RATE = 1 - ALMOST_PERFECT;
static const unsigned int SOLUTIONS_SIZE = 1000;
static const unsigned short HALL_OF_FAME_SIZE = 3;
static const unsigned short FORMULA_WIDTH = 70;
static const unsigned short FORMULA_NUMBER_PRECISION = 8;
static const unsigned short NUMBER_OF_RESULTS = 20;
static const unsigned short RATE_WIDTH = 10;
static const unsigned short RATE_PRECISION = RATE_WIDTH - 2;

#endif