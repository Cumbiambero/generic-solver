#ifndef GENERIC_SOLVER_ARBITRARY_HPP
#define GENERIC_SOLVER_ARBITRARY_HPP

#include <random>

using namespace std;
static random_device rd;
static std::mt19937 gen(rd());

class Coin {
public:
    virtual ~Coin() = default;

    virtual bool toss() = 0;
};

class RandomNumber {
public:
    virtual int calculate(int from, int to) = 0;
};

class AlmostRandomNumber : public RandomNumber {
public:
    int calculate(int from, int to) override {
        uniform_int_distribution<> distribution(from, to);
        return distribution(gen);
    }
};

class RandomCoin : public Coin {
public:
    bool toss() override {
        if (bit == 63) {
            current = xorShift128Plus();
            bit = 0;
            return current & 1;
        }
        return current & (1 << ++bit);
    }

private:
    uint64_t s[2] = {(uint64_t(rd()) << 32) ^ (rd()),
                     (uint64_t(rd()) << 32) ^ (rd())};
    uint64_t current{};
    uint8_t bit = 63;

    uint64_t xorShift128Plus() {
        uint64_t x = s[0];
        uint64_t const y = s[1];
        s[0] = y;
        x ^= x << 23;
        s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
        return s[1] + y;
    }
};

#endif