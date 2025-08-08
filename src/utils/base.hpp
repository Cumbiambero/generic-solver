#ifndef GENERIC_SOLVER_BASE_HPP
#define GENERIC_SOLVER_BASE_HPP

#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <type_traits>
#include <concepts>

using number = long double;
using std::vector;
using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept StringLike = std::convertible_to<T, std::string>;

#endif