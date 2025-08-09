#include "test.hpp"
#include "../src/solver/solver.hpp"

TEST_CASE("Solution operators") {
    Variable x("x");
    auto var_x = std::make_shared<Variable>(x);
    Formula formula(std::make_shared<Addition>(var_x, std::make_shared<Number>(5)), vector<Variable>{x});
    Solution a(formula, ChangerType::FLIPPER, 0.00002);
    Solution b(formula, ChangerType::MERGER, 0.00003);
    CHECK((a < b));
    CHECK((!(b < a)));
}

TEST_CASE("Formula rating") {
    Variable a("a");
    auto var_a = std::make_shared<Variable>(a);
    Formula formula(std::make_shared<Addition>(var_a, std::make_shared<Number>(2)), vector<Variable>{a});
    vector<vector<number>> input = {{1,2,3}};
    vector<vector<number>> results = {{3,4,5}};
    number bestRating = Evaluator::rate(formula, input, results);
    CHECK((bestRating == false));

    vector<vector<number>> otherResults = {{12,112,1112}};
    number badRating = Evaluator::rate(formula, input, otherResults);
    CHECK((badRating < 0.1));
}

TEST_CASE("Division by zero") {
    vector<Variable> vec;
    vec.emplace_back("x");
    auto var0 = std::make_shared<Variable>(vec[0]);
    Formula formula(std::make_shared<Division>(var0, std::make_shared<Number>(0)), vec);
    CHECK(formula.toString() == "(x/(0))");
    vector<vector<number>> irrelevant = {{1}};
    number rating = Evaluator::rate(formula, irrelevant, irrelevant);
    CHECK(rating < 0.00000000001);
}