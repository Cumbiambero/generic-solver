#include "../src/solver/creators/operation-producer.hpp"
#include "test.hpp"

TEST_CASE("OperationProducer") {
    OperationProducer operationProducer(makeTestRandomNumber());
    vector<Variable> vec;
    Variable x("x");
    vec.push_back(x);
    auto node = operationProducer.produce(vec);

    CHECK(node != nullptr);
    CHECK(node->toString().find("x") != string::npos);
    
    Variable y("y");
    vec.push_back(y);
    node = operationProducer.produce(vec);
    CHECK(node != nullptr);
    CHECK(node->toString().find("x") != string::npos);
    CHECK(node->toString().find("y") != string::npos);

    Variable z("z");
    vec.push_back(z);
    node = operationProducer.produce(vec);
    CHECK(node != nullptr);
    CHECK(node->toString().find("x") != string::npos);
    CHECK(node->toString().find("y") != string::npos);
    CHECK(node->toString().find("z") != string::npos);
}