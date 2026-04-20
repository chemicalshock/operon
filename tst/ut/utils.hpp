// -------------------------------------------------------------
//
//!\file utils.hpp
//!\brief Shared test utilities for Operon unit tests
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

// system
#include <iomanip>
#include <sstream>
#include <string>

// internal
#include "value.hpp"

namespace TEST_UTILS {

    //!\brief Convert a byte buffer to a lowercase hexadecimal string
    inline std::string buffer_to_hex(const OPERON::Value::Buffer& buffer)
    {
        std::ostringstream stream;

        for (uint8_t byte : buffer)
        {
            stream << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << static_cast<int>(byte);
        }

        return stream.str();
    }

} // namespace TEST_UTILS

#endif // TEST_UTILS_HPP