// -------------------------------------------------------------
//
//!\file utils.hpp
//!\brief Shared utility functions for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_UTILS_HPP
#define OPERON_UTILS_HPP

// system
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace OPERON {

namespace Utils {

    //!\brief Compare two byte sequences in constant time
    inline bool constant_time_eq(
        const uint8_t* a,
        size_t a_length,
        const uint8_t* b,
        size_t b_length
    )
    {
        size_t i;
        uint8_t diff;

        if (a == nullptr || b == nullptr)
        {
            return false;
        }

        if (a_length != b_length)
        {
            return false;
        }

        diff = 0;

        for (i = 0; i < a_length; ++i)
        {
            diff |= static_cast<uint8_t>(a[i] ^ b[i]);
        }

        return diff == 0;
    }

    //!\brief Compare two byte buffers in constant time
    inline bool constant_time_eq(
        const std::vector<uint8_t>& a,
        const std::vector<uint8_t>& b
    )
    {
        return constant_time_eq(
            a.data(),
            a.size(),
            b.data(),
            b.size()
        );
    }

    //!\brief Compare two strings in constant time
    inline bool constant_time_eq(
        const std::string& a,
        const std::string& b
    )
    {
        return constant_time_eq(
            reinterpret_cast<const uint8_t*>(a.data()),
            a.size(),
            reinterpret_cast<const uint8_t*>(b.data()),
            b.size()
        );
    }

    //!\brief Generate cryptographically suitable random bytes
    inline std::vector<uint8_t> random_bytes(size_t length)
    {
        std::vector<uint8_t> output;
        std::random_device random_device;

        output.resize(length);

        for (size_t i = 0; i < length; ++i)
        {
            output[i] = static_cast<uint8_t>(random_device() & 0xFFU);
        }

        return output;
    }

} // namespace Utils

} // namespace OPERON

#endif // OPERON_UTILS_HPP