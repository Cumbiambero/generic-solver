#include "../src/solver/creators/operation-producer.hpp"
#include "test.hpp"

TEST_CASE("OperationProducer") {
    OperationProducer operationProducer(makeTestRandomNumber());
    Variable x("x");
    vector<Variable> vec;
    vec.push_back(x);
    auto node = operationProducer.produce(vec);
    CHECK(node->toString() == "exp(x)");

    Variable y("y");
    vec.push_back(y);
    node = operationProducer.produce(vec);
    CHECK(node->toString() == "(x*y)");

    Variable z("z");
    vec.push_back(z);
    node = operationProducer.produce(vec);
    CHECK(node->toString() == "((x*y)*z)");
}