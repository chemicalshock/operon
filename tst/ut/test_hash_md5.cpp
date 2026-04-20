// -------------------------------------------------------------
//
//!\file test_hash_md5.cpp
//!\brief Tests for MD5 hash algorithm behaviour
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
#include "algos/hash/md5/md5.hpp"

// internal
#include "utils.hpp"

namespace {

    OPERON::Result run_md5(const std::vector<OPERON::Value>& args)
    {
        OPERON::Context context;
        const OPERON::Algorithm algorithm = OPERON::Algos::Hash::MD5::make_algorithm();

        return algorithm.function(context, args);
    }

} // namespace

SHOCKTEST_GOODWEATHER(hash_md5_empty_string_returns_expected_digest)
{
    const OPERON::Result result = run_md5({ "" });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());

    const OPERON::Value::Buffer& digest = result.value.as_buffer();

    EXPECT_EQ(digest.size(), static_cast<size_t>(16));
    EXPECT_EQ(
        TEST_UTILS::buffer_to_hex(digest),
        std::string("d41d8cd98f00b204e9800998ecf8427e")
    );
}

SHOCKTEST_GOODWEATHER(hash_md5_abc_returns_expected_digest)
{
    const OPERON::Result result = run_md5({ "abc" });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());

    const OPERON::Value::Buffer& digest = result.value.as_buffer();

    EXPECT_EQ(digest.size(), static_cast<size_t>(16));
    EXPECT_EQ(
        TEST_UTILS::buffer_to_hex(digest),
        std::string("900150983cd24fb0d6963f7d28e17f72")
    );
}

SHOCKTEST_GOODWEATHER(hash_md5_rejects_missing_arguments)
{
    const OPERON::Result result = run_md5({});

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E001_MISSING_ARGUMENT"));
}

SHOCKTEST_GOODWEATHER(hash_md5_rejects_invalid_input_type)
{
    const OPERON::Result result = run_md5({ 123 });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E002_INVALID_TYPE"));
}