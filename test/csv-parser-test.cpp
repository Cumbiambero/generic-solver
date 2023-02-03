#include "test.hpp"
#include "../src/utils/csv.hpp"

#include <filesystem>
#include <iostream>

CSVParser parser;

string getPath(const string &path) {
    const filesystem::path &currentDir = std::filesystem::current_path().filename();
    if (currentDir == "test") {
        return "../../" + path;
    }
    if (currentDir == "build") {
        return "../" + path;
    }
    if (currentDir == "code") {
        return path;
    } else {
        throw invalid_argument("Please run the test from within the test directory");
    }
}

TEST_CASE("CSV Parsing: Existing file") {
    const vector<vector<number>> &input = parser.parse(getPath("test/resources/csv-parser-ok.csv"));
    const vector<vector<number>> expected{{1,  1},
                                          {2,  3},
                                          {5,  8},
                                          {13, 21}};
    for (int line = 0; line < input.size(); ++line) {
        CHECK((input[line][0] == expected[line][0]));
        CHECK((input[line][1] == expected[line][1]));
    }
}

TEST_CASE("CSV Parsing: Incorrect path") {
    CHECK_THROWS_WITH(parser.parse("non-existing-file.csv"), "Couldn't read file: non-existing-file.csv");
}

TEST_CASE("CSV Parsing: Incorrect content") {
    CHECK_THROWS_WITH(parser.parse(getPath("test/resources/csv-parser-nok.csv")), "Incorrect content detected: two");
}