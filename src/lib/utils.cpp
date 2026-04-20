// -------------------------------------------------------------
//
//!\file utils.cpp
//!\brief Shared utility function implementations for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "utils.hpp"

//
//!\brief Compare two byte sequences in constant time
//
bool OPERON::Utils::constant_time_eq(
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