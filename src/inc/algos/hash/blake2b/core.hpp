// -------------------------------------------------------------
//
//!\file core.hpp
//!\brief Core BLAKE2b implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_BLAKE2B_CORE_HPP
#define OPERON_ALGOS_HASH_BLAKE2B_CORE_HPP

// system
#include <cstddef>
#include <cstdint>
#include <vector>

namespace OPERON::Algos::Hash::Blake2b::Core {

    struct State
    {
        //!\brief Chained state words
        uint64_t h[8] = { 0 };

        //!\brief Low and high byte counters
        uint64_t t[2] = { 0 };

        //!\brief Finalisation flags
        uint64_t f[2] = { 0 };

        //!\brief Input buffer
        uint8_t buffer[128] = { 0 };

        //!\brief Current buffered byte count
        size_t buffer_length = 0;

        //!\brief Requested output size in bytes
        size_t output_length = 64;
    };

    //!\brief Hash input data with unkeyed BLAKE2b
    std::vector<uint8_t> hash(const uint8_t* input, size_t input_length, size_t output_length);

} // namespace OPERON::Algos::Hash::Blake2b::Core

#endif