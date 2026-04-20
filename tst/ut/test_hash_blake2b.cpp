// -------------------------------------------------------------
//
//!\file test_hash_blake2b.cpp
//!\brief Tests for BLAKE2b hash algorithm behaviour
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

// internal
#include "algos/hash/blake2b/blake2b.hpp"
#include "utils.hpp"

namespace {

    OPERON::Result run_blake2b(const std::vector<OPERON::Value>& args)
    {
        OPERON::Context context;
        const OPERON::Algorithm algorithm = OPERON::Algos::Hash::Blake2b::make_algorithm();

        return algorithm.function(context, args);
    }

} // namespace

SHOCKTEST_GOODWEATHER(hash_blake2b_empty_string_returns_expected_digest)
{
    const OPERON::Result result = run_blake2b({ "", 64 });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());

    const OPERON::Value::Buffer& digest = result.value.as_buffer();

    EXPECT_EQ(digest.size(), static_cast<size_t>(64));
    EXPECT_EQ(
        TEST_UTILS::buffer_to_hex(digest),
        std::string(
            "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419"
            "d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce"
        )
    );
}

SHOCKTEST_GOODWEATHER(hash_blake2b_short_digest_returns_requested_length)
{
    const OPERON::Result result = run_blake2b({ "hello", 32 });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());
    EXPECT_EQ(result.value.as_buffer().size(), static_cast<size_t>(32));
}

SHOCKTEST_GOODWEATHER(hash_blake2b_rejects_zero_length)
{
    const OPERON::Result result = run_blake2b({ "hello", 0 });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E003_INVALID_VALUE"));
}

SHOCKTEST_GOODWEATHER(hash_blake2b_rejects_length_above_64)
{
    const OPERON::Result result = run_blake2b({ "hello", 65 });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E003_INVALID_VALUE"));
}

SHOCKTEST_GOODWEATHER(hash_blake2b_rejects_missing_arguments)
{
    const OPERON::Result result = run_blake2b({ "hello" });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E001_MISSING_ARGUMENT"));
}

SHOCKTEST_GOODWEATHER(hash_blake2b_rejects_invalid_input_type)
{
    const OPERON::Result result = run_blake2b({ 123, 64 });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E002_INVALID_TYPE"));
}

SHOCKTEST_GOODWEATHER(hash_blake2b_rejects_invalid_length_type)
{
    const OPERON::Result result = run_blake2b({ "hello", "64" });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E002_INVALID_TYPE"));
}
