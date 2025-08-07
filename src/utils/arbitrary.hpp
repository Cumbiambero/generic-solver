#ifndef GENERIC_SOLVER_ARBITRARY_HPP
#define GENERIC_SOLVER_ARBITRARY_HPP

#include "base.hpp"
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());

class Coin {
public:
    virtual ~Coin() = default;
    [[nodiscard]] virtual bool toss() = 0;
};

class RandomNumber {
public:
    virtual ~RandomNumber() = default;
    [[nodiscard]] virtual int calculate(int from, int to) = 0;
};

class AlmostRandomNumber : public RandomNumber {
public:
    [[nodiscard]] int calculate(int from, int to) override {
        std::uniform_int_distribution<> distribution(from, to);
        return distribution(gen);
    }
};

class RandomCoin : public Coin {
public:
    [[nodiscard]] bool toss() override {
        if (bit_ == 63) {
            current_ = xorShift128Plus();
            bit_ = 0;
            return current_ & 1;
        }
        return current_ & (1ULL << ++bit_);
    }

private:
    std::uint64_t s_[2] = {(std::uint64_t(rd()) << 32) ^ (rd()),
                           (std::uint64_t(rd()) << 32) ^ (rd())};
    std::uint64_t current_{};
    std::uint8_t bit_ = 63;

    [[nodiscard]] std::uint64_t xorShift128Plus() noexcept {
        std::uint64_t x = s_[0];
        const std::uint64_t y = s_[1];
        s_[0] = y;
        x ^= x << 23;
        s_[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
        return s_[1] + y;
    }
};

#endif