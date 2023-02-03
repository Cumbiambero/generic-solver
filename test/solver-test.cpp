#include "test.hpp"
#include "../src/solver/solver.hpp"

TEST_CASE("Solution operators") {
    Variable x("x");
    Addition addition(x, Number(5));
    Formula formula(addition, vector<Variable>{x});
    Solution a(formula, ChangerType::FLIPPER, 0.00002);
    Solution b(formula, ChangerType::MERGER, 0.00003);
    CHECK((a == a));
    CHECK((b == b));
    CHECK((a < b));
    CHECK((a <= b));
    CHECK((b > a));
    CHECK((b >= a));
    CHECK((!(b < a)));
    CHECK((!(b <= a)));
    CHECK((!(a > b)));
    CHECK((!(a >= b)));
}

TEST_CASE("Formula rating") {
    Variable a("a");
    Addition addition(a, Number(2));
    Formula formula(addition, vector<Variable>{a});
    vector<vector<number>> input = {{1,2,3}};
    vector<vector<number>> results = {{3,4,5}};
    number bestRating = Evaluator::rate(formula, input, results);
    CHECK((bestRating == 1));

    vector<vector<number>> otherResults = {{12,112,1112}};
    number badRating = Evaluator::rate(formula, input, otherResults);
    CHECK((badRating < 0.1));
}

TEST_CASE("Division by zero") {
    vector<Variable> vec;
    vec.emplace_back("x");
    Division division(vec[0], Number(0));
    Formula formula(division, vec);
    CHECK(formula.toString() == "(x/(0))");
    vector<vector<number>> irrelevant = {{1}};
    number rating = Evaluator::rate(formula, irrelevant, irrelevant);
    CHECK(rating < 0.00000000001);
}