#include "hash.h"
#include <cstring> // For memcpy
#include <functional>

RandomHash::RandomHash(int seed)
{
    if (seed == -1)
    {
        // Generate a random seed if none is provided
        std::random_device rd;
        this->seed = rd();
    }
    else
    {
        this->seed = seed;
    }
    engine.seed(this->seed);
}

int RandomHash::getSeed() const
{
    return seed;
}

uint32_t RandomHash::hash(const std::string &input) const
{
    return murmurHash3(input.c_str(), static_cast<int>(input.size()), static_cast<uint32_t>(seed));
}

uint32_t RandomHash::hash(int input) const
{
    return murmurHash3(&input, sizeof(input), static_cast<uint32_t>(seed));
}

// MurmurHash3_x86_32 implementation
uint32_t RandomHash::murmurHash3(const void *key, int len, uint32_t seed)
{
    const uint8_t *data = (const uint8_t *)key;
    const int nblocks = len / 4;
    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // Process 4-byte blocks
    const uint32_t *blocks = (const uint32_t *)(data + nblocks * 4);
    for (int i = -nblocks; i; i++)
    {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> (32 - 15));
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> (32 - 13));
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Process the tail (remaining bytes)
    const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3)
    {
    case 3:
        k1 ^= tail[2] << 16;
    case 2:
        k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];
        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> (32 - 15));
        k1 *= c2;
        h1 ^= k1;
    }

    // Finalize hash
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}
