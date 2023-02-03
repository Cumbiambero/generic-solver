#include "test.hpp"
#include "../src/utils/csv.hpp"

#include <filesystem>
#include <iostream>

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

TEST_CASE("CSV Parsing: Multiple values") {
    const vector<vector<number>> &input = parseCSV(getPath("test/resources/csv-parser-ok.csv"));
    const vector<vector<number>> expected{{1,  1},
                                          {2,  3},
                                          {5,  8},
                                          {13, 21}};
    for (int line = 0; line < input.size(); ++line) {
        CHECK((input[line][0] == expected[line][0]));
        CHECK((input[line][1] == expected[line][1]));
    }
}

TEST_CASE("CSV Parsing: Decimals") {
    const vector<vector<number>> &input = parseCSV(getPath("test/resources/circle-results.csv"));
    const vector<vector<number>> expected{{3.141592653589793},
                                          {12.566370614359172},
                                          {28.274333882308138},
                                          {50.26548245743669},
                                          {78.53981633974483},
                                          {113.09733552923255},
                                          {153.93804002589985},
                                          {201.06192982974676},
                                          {254.46900494077323},
                                          {314.1592653589793}};
    for (int line = 0; line < input.size(); ++line) {
        CHECK((input[line][0] == expected[line][0]));
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-result"

TEST_CASE("CSV Parsing: Incorrect path") {
    CHECK_THROWS_WITH(parseCSV("non-existing-file.csv"), "Couldn't read file: non-existing-file.csv");
}

TEST_CASE("CSV Parsing: Incorrect content") {
    CHECK_THROWS_WITH(parseCSV(getPath("test/resources/csv-parser-nok.csv")), "Incorrect content detected: two");
}

#pragma clang diagnostic pop