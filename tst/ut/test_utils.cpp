// -------------------------------------------------------------
//
//!\file test_utils.cpp
//!\brief Tests for shared utility helpers
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// shocktest
#include "shocktest/shocktest.hpp"

// SUT
#include "../../src/inc/utils.hpp"

SHOCKTEST_GOODWEATHER(utils_buffer_display_preserves_plain_text)
{
    const std::vector<uint8_t> buffer = { 'h', 'e', 'l', 'l', 'o', '\n' };

    EXPECT_TRUE(OPERON::Utils::buffer_is_text(buffer));
    EXPECT_EQ(OPERON::Utils::buffer_to_display_string(buffer), std::string("hello\n"));
}

SHOCKTEST_GOODWEATHER(utils_buffer_display_hex_encodes_binary_data)
{
    const std::vector<uint8_t> buffer = { 0x00, 0x41, 0x42, 0xFF };

    EXPECT_FALSE(OPERON::Utils::buffer_is_text(buffer));
    EXPECT_EQ(OPERON::Utils::buffer_to_display_string(buffer), std::string("004142ff"));
}
