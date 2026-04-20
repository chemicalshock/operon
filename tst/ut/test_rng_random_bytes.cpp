// -------------------------------------------------------------
//
//!\file test_rng_random_bytes.cpp
//!\brief Tests for random bytes algorithm behaviour
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
#include "algos/rng/random_bytes/random_bytes.hpp"

namespace {

    OPERON::Result run_random_bytes(const std::vector<OPERON::Value>& args)
    {
        OPERON::Context context;
        const OPERON::Algorithm algorithm = OPERON::Algos::Rng::RandomBytes::make_algorithm();

        return algorithm.function(context, args);
    }

} // namespace

SHOCKTEST_GOODWEATHER(rng_random_bytes_returns_buffer_of_requested_length)
{
    const OPERON::Result result = run_random_bytes({ 16 });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());
    EXPECT_EQ(result.value.as_buffer().size(), static_cast<size_t>(16));
}

SHOCKTEST_GOODWEATHER(rng_random_bytes_allows_zero_length)
{
    const OPERON::Result result = run_random_bytes({ 0 });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());
    EXPECT_EQ(result.value.as_buffer().size(), static_cast<size_t>(0));
}

SHOCKTEST_GOODWEATHER(rng_random_bytes_rejects_missing_arguments)
{
    const OPERON::Result result = run_random_bytes({});

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E001_MISSING_ARGUMENT"));
}

SHOCKTEST_GOODWEATHER(rng_random_bytes_rejects_invalid_type)
{
    const OPERON::Result result = run_random_bytes({ "16" });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E002_INVALID_TYPE"));
}

SHOCKTEST_GOODWEATHER(rng_random_bytes_rejects_negative_length)
{
    const OPERON::Result result = run_random_bytes({ -1 });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E003_INVALID_VALUE"));
}

SHOCKTEST_GOODWEATHER(rng_random_bytes_two_calls_return_correct_lengths)
{
    const OPERON::Result first = run_random_bytes({ 32 });
    const OPERON::Result second = run_random_bytes({ 32 });

    EXPECT_TRUE(first.ok);
    EXPECT_TRUE(second.ok);
    EXPECT_TRUE(first.value.is_buffer());
    EXPECT_TRUE(second.value.is_buffer());
    EXPECT_EQ(first.value.as_buffer().size(), static_cast<size_t>(32));
    EXPECT_EQ(second.value.as_buffer().size(), static_cast<size_t>(32));
}