#include "../src/creators/operation-producer.hpp"
#include "test.hpp"

TEST_CASE("OperationProducer") {
    OperationProducer picker(testRandomNumber);
    Variable x("x");
    vector<Variable> vec;
    vec.push_back(x);
    auto node = picker.produce(vec);
    CHECK(node->toString() == "(x)³");

    Variable y("y");
    vec.push_back(y);
    node = picker.produce(vec);
    CHECK(node->toString() == "(x*y)");

    Variable z("z");
    vec.push_back(z);
    node = picker.produce(vec);
    CHECK(node->toString() == "((x*y)*z)");
}