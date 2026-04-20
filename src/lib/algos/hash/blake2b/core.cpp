// -------------------------------------------------------------
//
//!\file core.cpp
//!\brief Core BLAKE2b implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "algos/hash/blake2b/core.hpp"

// system
#include <cstring>
#include <stdexcept>

namespace
{
    constexpr uint64_t BLAKE2B_IV[8] = {
        0x6A09E667F3BCC908ULL,
        0xBB67AE8584CAA73BULL,
        0x3C6EF372FE94F82BULL,
        0xA54FF53A5F1D36F1ULL,
        0x510E527FADE682D1ULL,
        0x9B05688C2B3E6C1FULL,
        0x1F83D9ABFB41BD6BULL,
        0x5BE0CD19137E2179ULL
    };

    constexpr uint8_t BLAKE2B_SIGMA[12][16] = {
        {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
        { 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 },
        { 11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4 },
        {  7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8 },
        {  9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13 },
        {  2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9 },
        { 12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11 },
        { 13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10 },
        {  6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5 },
        { 10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13,  0 },
        {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
        { 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 }
    };

    //
    //!\brief Load a 64-bit little-endian value
    //
    uint64_t load_u64(const uint8_t* data)
    {
        return
            (static_cast<uint64_t>(data[0])      ) |
            (static_cast<uint64_t>(data[1]) <<  8) |
            (static_cast<uint64_t>(data[2]) << 16) |
            (static_cast<uint64_t>(data[3]) << 24) |
            (static_cast<uint64_t>(data[4]) << 32) |
            (static_cast<uint64_t>(data[5]) << 40) |
            (static_cast<uint64_t>(data[6]) << 48) |
            (static_cast<uint64_t>(data[7]) << 56);
    }

    //
    //!\brief Store a 64-bit little-endian value
    //
    void store_u64(uint8_t* data, uint64_t value)
    {
        data[0] = static_cast<uint8_t>( value        & 0xFFU);
        data[1] = static_cast<uint8_t>((value >>  8) & 0xFFU);
        data[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
        data[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);
        data[4] = static_cast<uint8_t>((value >> 32) & 0xFFU);
        data[5] = static_cast<uint8_t>((value >> 40) & 0xFFU);
        data[6] = static_cast<uint8_t>((value >> 48) & 0xFFU);
        data[7] = static_cast<uint8_t>((value >> 56) & 0xFFU);
    }

    //
    //!\brief Rotate a 64-bit value right
    //
    uint64_t rotr64(uint64_t value, unsigned int count)
    {
        return (value >> count) | (value << (64U - count));
    }

    //
    //!\brief Increment the byte counter
    //
    void increment_counter(OPERON::Algos::Hash::Blake2b::Core::State& state, uint64_t amount)
    {
        state.t[0] += amount;

        if (state.t[0] < amount)
        {
            state.t[1] += 1;
        }
    }

    //
    //!\brief The BLAKE2b G mixing function
    //
    void g(uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t x, uint64_t y)
    {
        a = a + b + x;
        d = rotr64(d ^ a, 32);
        c = c + d;
        b = rotr64(b ^ c, 24);
        a = a + b + y;
        d = rotr64(d ^ a, 16);
        c = c + d;
        b = rotr64(b ^ c, 63);
    }

    //
    //!\brief Compress one 128-byte block
    //
    void compress(OPERON::Algos::Hash::Blake2b::Core::State& state, const uint8_t block[128])
    {
        uint64_t m[16];
        uint64_t v[16];

        for (size_t i = 0; i < 16; ++i)
        {
            m[i] = load_u64(block + (i * 8));
        }

        for (size_t i = 0; i < 8; ++i)
        {
            v[i] = state.h[i];
            v[i + 8] = BLAKE2B_IV[i];
        }

        v[12] ^= state.t[0];
        v[13] ^= state.t[1];
        v[14] ^= state.f[0];
        v[15] ^= state.f[1];

        for (size_t round = 0; round < 12; ++round)
        {
            g(v[0], v[4], v[8],  v[12], m[BLAKE2B_SIGMA[round][0]],  m[BLAKE2B_SIGMA[round][1]]);
            g(v[1], v[5], v[9],  v[13], m[BLAKE2B_SIGMA[round][2]],  m[BLAKE2B_SIGMA[round][3]]);
            g(v[2], v[6], v[10], v[14], m[BLAKE2B_SIGMA[round][4]],  m[BLAKE2B_SIGMA[round][5]]);
            g(v[3], v[7], v[11], v[15], m[BLAKE2B_SIGMA[round][6]],  m[BLAKE2B_SIGMA[round][7]]);

            g(v[0], v[5], v[10], v[15], m[BLAKE2B_SIGMA[round][8]],  m[BLAKE2B_SIGMA[round][9]]);
            g(v[1], v[6], v[11], v[12], m[BLAKE2B_SIGMA[round][10]], m[BLAKE2B_SIGMA[round][11]]);
            g(v[2], v[7], v[8],  v[13], m[BLAKE2B_SIGMA[round][12]], m[BLAKE2B_SIGMA[round][13]]);
            g(v[3], v[4], v[9],  v[14], m[BLAKE2B_SIGMA[round][14]], m[BLAKE2B_SIGMA[round][15]]);
        }

        for (size_t i = 0; i < 8; ++i)
        {
            state.h[i] ^= v[i] ^ v[i + 8];
        }
    }

    //
    //!\brief Initialise a BLAKE2b state
    //
    void init_state(OPERON::Algos::Hash::Blake2b::Core::State& state, size_t output_length)
    {
        if (output_length == 0 || output_length > 64)
        {
            throw std::runtime_error("BLAKE2b output length must be between 1 and 64");
        }

        for (size_t i = 0; i < 8; ++i)
        {
            state.h[i] = BLAKE2B_IV[i];
        }

        state.h[0] ^= 0x01010000U ^ static_cast<uint64_t>(output_length);
        state.t[0] = 0;
        state.t[1] = 0;
        state.f[0] = 0;
        state.f[1] = 0;
        state.buffer_length = 0;
        state.output_length = output_length;
        std::memset(state.buffer, 0, sizeof(state.buffer));
    }

    //
    //!\brief Update the state with input bytes
    //
    void update_state(OPERON::Algos::Hash::Blake2b::Core::State& state, const uint8_t* input, size_t input_length)
    {
        if (input == nullptr && input_length != 0)
        {
            throw std::runtime_error("BLAKE2b input pointer is null");
        }

        size_t offset = 0;

        while (input_length > 0)
        {
            const size_t space = sizeof(state.buffer) - state.buffer_length;
            const size_t take = (input_length < space) ? input_length : space;

            std::memcpy(state.buffer + state.buffer_length, input + offset, take);
            state.buffer_length += take;
            offset += take;
            input_length -= take;

            if (state.buffer_length == sizeof(state.buffer))
            {
                increment_counter(state, static_cast<uint64_t>(sizeof(state.buffer)));
                compress(state, state.buffer);
                state.buffer_length = 0;
                std::memset(state.buffer, 0, sizeof(state.buffer));
            }
        }
    }

    //
    //!\brief Finalise the state and write output
    //
    std::vector<uint8_t> finalise_state(OPERON::Algos::Hash::Blake2b::Core::State& state)
    {
        std::vector<uint8_t> full_output(64);
        uint8_t words[64];

        increment_counter(state, static_cast<uint64_t>(state.buffer_length));
        state.f[0] = ~0ULL;

        for (size_t i = state.buffer_length; i < sizeof(state.buffer); ++i)
        {
            state.buffer[i] = 0;
        }

        compress(state, state.buffer);

        for (size_t i = 0; i < 8; ++i)
        {
            store_u64(words + (i * 8), state.h[i]);
        }

        std::memcpy(full_output.data(), words, state.output_length);
        full_output.resize(state.output_length);

        return full_output;
    }
}

//
//!\brief Hash input data with unkeyed BLAKE2b
//
std::vector<uint8_t> OPERON::Algos::Hash::Blake2b::Core::hash(const uint8_t* input, size_t input_length, size_t output_length)
{
    State state;

    init_state(state, output_length);
    update_state(state, input, input_length);

    return finalise_state(state);
}