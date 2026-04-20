// -------------------------------------------------------------
//
//!\file test_util_constant_time_eq.cpp
//!\brief Tests for constant-time equality algorithm behaviour
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
#include "algos/util/constant_time_eq/constant_time_eq.hpp"


namespace {

    OPERON::Result run_constant_time_eq(const std::vector<OPERON::Value>& args)
    {
        OPERON::Context context;
        const OPERON::Algorithm algorithm = OPERON::Algos::Util::ConstantTimeEq::make_algorithm();

        return algorithm.function(context, args);
    }

} // namespace

SHOCKTEST_GOODWEATHER(util_constant_time_eq_returns_true_for_equal_strings)
{
    const OPERON::Result result = run_constant_time_eq({ "hello", "hello" });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_bool());
    EXPECT_TRUE(result.value.as_bool());
}

SHOCKTEST_GOODWEATHER(util_constant_time_eq_returns_false_for_different_strings)
{
    const OPERON::Result result = run_constant_time_eq({ "hello", "world" });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_bool());
    EXPECT_FALSE(result.value.as_bool());
}

SHOCKTEST_GOODWEATHER(util_constant_time_eq_returns_false_for_different_length_strings)
{
    const OPERON::Result result = run_constant_time_eq({ "hello", "helloo" });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_bool());
    EXPECT_FALSE(result.value.as_bool());
}

SHOCKTEST_GOODWEATHER(util_constant_time_eq_returns_true_for_equal_buffers)
{
    const OPERON::Value::Buffer a = { 0x01, 0x02, 0x03, 0x04 };
    const OPERON::Value::Buffer b = { 0x01, 0x02, 0x03, 0x04 };

    const OPERON::Result result = run_constant_time_eq({ a, b });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_bool());
    EXPECT_TRUE(result.value.as_bool());
}

SHOCKTEST_GOODWEATHER(util_constant_time_eq_returns_false_for_different_buffers)
{
    const OPERON::Value::Buffer a = { 0x01, 0x02, 0x03, 0x04 };
    const OPERON::Value::Buffer b = { 0x01, 0x02, 0x03, 0x05 };

    const OPERON::Result result = run_constant_time_eq({ a, b });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_bool());
    EXPECT_FALSE(result.value.as_bool());
}

SHOCKTEST_GOODWEATHER(util_constant_time_eq_rejects_missing_arguments)
{
    const OPERON::Result result = run_constant_time_eq({ "hello" });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E001_MISSING_ARGUMENT"));
}

SHOCKTEST_GOODWEATHER(util_constant_time_eq_rejects_mixed_types)
{
    const OPERON::Value::Buffer buffer = { 0x68, 0x65, 0x6c, 0x6c, 0x6f };

    const OPERON::Result result = run_constant_time_eq({ "hello", buffer });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E002_INVALID_TYPE"));
}

SHOCKTEST_GOODWEATHER(util_constant_time_eq_rejects_invalid_types)
{
    const OPERON::Result result = run_constant_time_eq({ 123, 456 });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E002_INVALID_TYPE"));
}