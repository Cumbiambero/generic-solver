#ifndef GENERIC_SOLVER_ARBITRARY_HPP
#define GENERIC_SOLVER_ARBITRARY_HPP

#include "base.hpp"
#include <random>
#include <mutex>

class Coin {
public:
    virtual ~Coin() = default;
    [[nodiscard]] virtual bool toss() = 0;
};

class RandomNumber {
public:
    virtual ~RandomNumber() = default;
    [[nodiscard]] virtual int calculate(int from, int to) = 0;
    [[nodiscard]] virtual number calculateReal(number from, number to) = 0;
};

class AlmostRandomNumber : public RandomNumber {
public:
    AlmostRandomNumber() : generator_(std::random_device{}()) {}
    
    [[nodiscard]] int calculate(int from, int to) override {
        std::uniform_int_distribution<> distribution(from, to);
        std::lock_guard<std::mutex> lock(mutex_);
        return distribution(generator_);
    }
    
    [[nodiscard]] number calculateReal(number from, number to) override {
        std::uniform_real_distribution<number> distribution(from, to);
        std::lock_guard<std::mutex> lock(mutex_);
        return distribution(generator_);
    }

private:
    std::mt19937 generator_;
    std::mutex mutex_;
};

class RandomCoin : public Coin {
public:
    RandomCoin() = default;
    
    [[nodiscard]] bool toss() override {
        // Thread-local state for lock-free concurrent access
        thread_local std::uint64_t s0 = initSeed();
        thread_local std::uint64_t s1 = initSeed();
        thread_local std::uint64_t current = 0;
        thread_local std::uint8_t bit = 63;
        
        if (bit >= 63) {
            // xorshift128+ algorithm
            std::uint64_t x = s0;
            const std::uint64_t y = s1;
            s0 = y;
            x ^= x << 23;
            s1 = x ^ y ^ (x >> 17) ^ (y >> 26);
            current = s1 + y;
            bit = 0;
            return current & 1;
        }
        ++bit;
        return (current >> bit) & 1;
    }

private:
    static std::uint64_t initSeed() {
        std::random_device rd;
        return (std::uint64_t(rd()) << 32) ^ rd();
    }
};

#endif