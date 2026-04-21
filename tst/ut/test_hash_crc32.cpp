// -------------------------------------------------------------
//
//!\file test_hash_crc32.cpp
//!\brief Tests for CRC32 hash algorithm behaviour
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
#include "algos/hash/crc32/crc32.hpp"

// internal
#include "utils.hpp"

namespace {

    OPERON::Result run_crc32(const std::vector<OPERON::Value>& args)
    {
        OPERON::Context context;
        const OPERON::Algorithm algorithm = OPERON::Algos::Hash::CRC32::make_algorithm();

        return algorithm.function(context, args);
    }

} // namespace

SHOCKTEST_GOODWEATHER(hash_crc32_empty_string_returns_expected_digest)
{
    const OPERON::Result result = run_crc32({ "" });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());

    const OPERON::Value::Buffer& digest = result.value.as_buffer();

    EXPECT_EQ(digest.size(), static_cast<size_t>(4));
    EXPECT_EQ(
        TEST_UTILS::buffer_to_hex(digest),
        std::string("00000000")
    );
}

SHOCKTEST_GOODWEATHER(hash_crc32_abc_returns_expected_digest)
{
    const OPERON::Result result = run_crc32({ "abc" });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());

    const OPERON::Value::Buffer& digest = result.value.as_buffer();

    EXPECT_EQ(digest.size(), static_cast<size_t>(4));
    EXPECT_EQ(
        TEST_UTILS::buffer_to_hex(digest),
        std::string("352441c2")
    );
}

SHOCKTEST_GOODWEATHER(hash_crc32_123456789_returns_expected_digest)
{
    const OPERON::Result result = run_crc32({ "123456789" });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());

    const OPERON::Value::Buffer& digest = result.value.as_buffer();

    EXPECT_EQ(digest.size(), static_cast<size_t>(4));
    EXPECT_EQ(
        TEST_UTILS::buffer_to_hex(digest),
        std::string("cbf43926")
    );
}

SHOCKTEST_GOODWEATHER(hash_crc32_rejects_missing_arguments)
{
    const OPERON::Result result = run_crc32({});

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E001_MISSING_ARGUMENT"));
}

SHOCKTEST_GOODWEATHER(hash_crc32_rejects_invalid_input_type)
{
    const OPERON::Result result = run_crc32({ 123 });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E002_INVALID_TYPE"));
}
