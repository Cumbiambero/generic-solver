#include "test.hpp"
#include "../src/solver/solver.hpp"

TEST_CASE("Solution operators") {
    Variable x("x");
    Addition addition(x, Number(5));
    Formula formula(addition, vector<Variable>{x});
    Solution a(formula, FLIPPER, 0.00002);
    Solution b(formula, MERGER, 0.00003);
    CHECK((a == a));
    CHECK((b == b));
    CHECK((a < b));
    CHECK((a < b));
    CHECK((a <= b));
    CHECK((b > a));
    CHECK((b >= a));
    CHECK((!(b < a)));
    CHECK((!(b <= a)));
    CHECK((!(a > b)));
    CHECK((!(a >= b)));
    CHECK((!(a == b)));
    CHECK((!(b == a)));
    CHECK((!(a >= b)));
}

