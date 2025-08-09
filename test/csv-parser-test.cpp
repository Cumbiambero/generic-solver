#include "test.hpp"
#include "../src/utils/csv.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

std::string getPath(const std::string &filename) {
    const auto &currentDir = std::filesystem::current_path().filename();
    if (currentDir == "test") {
        return filename;
    }
    if (currentDir == "build") {
        return "test/" + filename;
    }
    if (currentDir == "code") {
        return "../test/" + filename;
    }
    throw std::invalid_argument("Please run the test from within the test directory");
}

TEST_CASE("CSV Parsing: Multiple values") {
    const vector<vector<number>> &input = parseCSV(getPath("resources/csv-parser-ok.csv"));
    const vector<vector<number>> expected{{1,  1},
                                          {2,  3},
                                          {5,  8},
                                          {13, 21}};
    for (std::size_t line = 0; line < input.size(); ++line) {
        CHECK((input[line][0] == expected[line][0]));
        CHECK((input[line][1] == expected[line][1]));
    }
}

TEST_CASE("CSV Parsing: Decimals") {
    const vector<vector<number>> &input = parseCSV(getPath("resources/circle-results.csv"));
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
    for (std::size_t line = 0; line < input.size(); ++line) {
        CHECK(std::abs(input[line][0] - expected[line][0]) < 1e-8);
    }
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-result"
#endif

TEST_CASE("CSV Parsing: Incorrect path") {
    CHECK_THROWS_WITH(parseCSV("non-existing-file.csv"), "Couldn't read file: non-existing-file.csv");
}

TEST_CASE("CSV Parsing: Incorrect content") {
    CHECK_THROWS_WITH(parseCSV(getPath("resources/csv-parser-nok.csv")), "Incorrect content detected: two");
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif