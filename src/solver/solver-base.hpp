#ifndef GENERIC_SOLVER_CHANGERS_BASE_HPP
#define GENERIC_SOLVER_CHANGERS_BASE_HPP

#include "../utils/arbitrary.hpp"
#include "formula.hpp"

enum ChangerType {
    FLIPPER
};

static vector<ChangerType> CHANGERS({FLIPPER});

#endif
