#include "../src/solver/creators/operation-producer.hpp"
#include "test.hpp"

TEST_CASE("OperationProducer") {
<<<<<<< HEAD
    OperationProducer operationProducer(makeTestRandomNumber());
=======
    OperationProducer operationProducer(makeTestRandomNumber()); // deterministic
>>>>>>> 1460f4d65a7255c809aea5f37c017a5db5ba945b
    Variable x("x");
    vector<Variable> vec;
    vec.push_back(x);
    auto node = operationProducer.produce(vec);
<<<<<<< HEAD
    CHECK(node->toString() == "exp(x)");
=======
    CHECK(node->toString() == "abs(x)");
>>>>>>> 1460f4d65a7255c809aea5f37c017a5db5ba945b

    Variable y("y");
    vec.push_back(y);
    node = operationProducer.produce(vec);
    CHECK(node->toString() == "(x*y)");

    Variable z("z");
    vec.push_back(z);
    node = operationProducer.produce(vec);
    CHECK(node->toString() == "((x/y)-z)");
}