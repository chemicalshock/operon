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

namespace OPERON {

namespace Utils {

    //!\brief Compare two byte sequences in constant time
    bool constant_time_eq(
        const uint8_t* a,
        size_t a_length,
        const uint8_t* b,
        size_t b_length
    );

} // namespace Utils

} // namespace OPERON

#endif