#ifndef HASH_H
#define HASH_H

#include <string>
#include <random>

class RandomHash
{
public:
    // Constructor with optional seed parameter
    explicit RandomHash(int seed = -1);

    // Returns the seed used for hashing (useful for testing consistency)
    int getSeed() const;

    // Hash functions
    uint32_t hash(const std::string &input) const;
    uint32_t hash(int input) const;

private:
    int seed;
    mutable std::mt19937 engine;

    // MurmurHash3 function
    static uint32_t murmurHash3(const void *key, int len, uint32_t seed);
};

#endif // HASH_H
