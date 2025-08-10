#include "test.hpp"
#include "../src/solver/creators/operation-producer.hpp"

TEST_CASE("Formula evaluation square") {
    Variable d("d");
    auto var_d = std::make_shared<Variable>(d);
    Formula formula(std::make_shared<Square>(var_d), d);
    CHECK(formula.toString() == "(d)^2");
    CHECK(format(formula.evaluate(5)) == "25");
}

TEST_CASE("Formula evaluation multiplication") {
    Variable r("r", 1);
    auto var_r = std::make_shared<Variable>(r);
    Formula formula(std::make_shared<Multiplication>(
        std::make_shared<Square>(var_r),
        std::make_shared<Pi>()
    ), r);
    CHECK(formula.toString() == "((r)^2*π)");
    CHECK((formula.evaluate(1) == Pi().calculate()));
    CHECK(format(formula.evaluate(4)) == "50.265482");
    CHECK(format(formula.evaluate(200)) == "125663.71");
}

TEST_CASE("Formula evaluation operation producer") {
    OperationProducer operationProducer(makeTestRandomNumber()); // deterministic
    vector<Variable> vec;
    vec.emplace_back("x");
    auto node = operationProducer.produce(vec);
    Formula formula(node, vec);
<<<<<<< HEAD
    CHECK(formula.toString() == "exp(x)");
    CHECK(format(formula.evaluate(2)) == "7.3890561");
=======
    CHECK(formula.toString() == "(x)^2");
    CHECK(format(formula.evaluate(2)) == "4");
>>>>>>> 1460f4d65a7255c809aea5f37c017a5db5ba945b
}