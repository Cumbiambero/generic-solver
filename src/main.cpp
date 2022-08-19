#include "utils/csv.hpp"
#include "solver/creators/operation-producer.hpp"

int main(int argc, char **argv) {
    if (argc < 4) {
        throw invalid_argument("Please provide 2 paths for input and result csv files followed by the variable names.");
    }

    CSVParser parser;
    const vector<vector<number>> &input = parser.parse(argv[1]);
    const vector<vector<number>> &results = parser.parse(argv[2]);

    if(input.size() != results.size()) {
        throw length_error("Both files need to have the same number of lines.");
    }
    
    vector<Variable> variables;
    for(int i = 3; i < argc; ++i) {
        variables.emplace_back(argv[i]);
    }
    
    OperationProducer operationProducer;
    const shared_ptr<Node> &operation = operationProducer.produce(variables);
    Formula formula(operation, variables);

    // TODO: Alter formula with changers and rate the results. Store them in a list and prefer the best changers.

    return 0;
}