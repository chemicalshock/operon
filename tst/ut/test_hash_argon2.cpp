// -------------------------------------------------------------
//
//!\file test_hash_argon2id.cpp
//!\brief Tests for Argon2id core behaviour
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

// system
#include <cstring>

// SUT
#include "algos/hash/argon2/core.hpp"

// internal
#include "utils.hpp"

namespace
{

    OPERON::Value::Buffer run_argon2id_raw()
    {
        using namespace OPERON::Algos::Hash::Argon2::Core;

        Params params;
        params.variant = Variant::Argon2id;
        params.version = 0x13;
        params.time_cost = 3;
        params.memory_cost_kib = 32;
        params.parallelism = 4;
        params.hash_length = 32;
        params.secret = std::vector<uint8_t>(8, 0x03);
        params.associated_data = std::vector<uint8_t>(12, 0x04);

        const std::vector<uint8_t> password(32, 0x01);
        const std::vector<uint8_t> salt(16, 0x02);

        return hash_raw(
            params,
            password.data(), password.size(),
            salt.data(), salt.size()
        );
    }

    OPERON::Value::Buffer get_first_block()
    {
        using namespace OPERON::Algos::Hash::Argon2::Core;

        Params params;
        params.variant = Variant::Argon2id;
        params.version = 0x13;
        params.time_cost = 1;
        params.memory_cost_kib = 32;
        params.parallelism = 1;
        params.hash_length = 32;

        const std::vector<uint8_t> password(32, 0x01);
        const std::vector<uint8_t> salt(16, 0x02);

        Instance instance;

        validate_params(params, password.size(), salt.size());
        initialise_instance(instance, params);

        initialise_first_blocks(
            instance,
            password.data(), password.size(),
            salt.data(), salt.size()
        );

        OPERON::Value::Buffer out(1024);
        std::memcpy(out.data(), instance.memory[0].v, 1024);
        return out;
    }

    OPERON::Value::Buffer get_third_block_after_fill()
    {
        using namespace OPERON::Algos::Hash::Argon2::Core;

        Params params;
        params.variant = Variant::Argon2id;
        params.version = 0x13;
        params.time_cost = 1;
        params.memory_cost_kib = 32;
        params.parallelism = 1;
        params.hash_length = 32;

        const std::vector<uint8_t> password(32, 0x01);
        const std::vector<uint8_t> salt(16, 0x02);

        Instance instance;

        validate_params(params, password.size(), salt.size());
        initialise_instance(instance, params);
        initialise_first_blocks(
            instance,
            password.data(), password.size(),
            salt.data(), salt.size()
        );
        fill_memory_blocks(instance);

        OPERON::Value::Buffer out(1024);
        std::memcpy(out.data(), instance.memory[2].v, 1024);
        return out;
    }    

} // anonymous namespace

SHOCKTEST_GOODWEATHER(argon2_initial_block_not_zero)
{
    const auto block = get_first_block();

    EXPECT_EQ(block.size(), static_cast<size_t>(1024));

    bool all_zero = true;
    for (auto b : block)
    {
        if (b != 0)
        {
            all_zero = false;
            break;
        }
    }

    EXPECT_FALSE(all_zero);
}

SHOCKTEST_GOODWEATHER(argon2_initial_block_deterministic)
{
    const auto a = get_first_block();
    const auto b = get_first_block();

    EXPECT_EQ(a.size(), b.size());
    EXPECT_TRUE(a == b);
}

SHOCKTEST_GOODWEATHER(argon2_third_block_after_fill_not_zero)
{
    const auto block = get_third_block_after_fill();

    EXPECT_EQ(block.size(), static_cast<size_t>(1024));

    bool all_zero = true;
    for (auto b : block)
    {
        if (b != 0)
        {
            all_zero = false;
            break;
        }
    }

    EXPECT_FALSE(all_zero);
}

SHOCKTEST_GOODWEATHER(argon2_third_block_after_fill_deterministic)
{
    const auto a = get_third_block_after_fill();
    const auto b = get_third_block_after_fill();

    EXPECT_EQ(a.size(), b.size());
    EXPECT_TRUE(a == b);
}

SHOCKTEST_GOODWEATHER(argon2_third_block_differs_from_first_two)
{
    const auto first = get_first_block();
    const auto third = get_third_block_after_fill();

    EXPECT_EQ(first.size(), third.size());
    EXPECT_TRUE(first != third);
}

SHOCKTEST_BADWEATHER(hash_argon2id_matches_rfc9106_vector)
{
    const OPERON::Value::Buffer digest = run_argon2id_raw();

    EXPECT_EQ(digest.size(), static_cast<size_t>(32));
    EXPECT_EQ(
        TEST_UTILS::buffer_to_hex(digest),
        std::string("0d640df58d78766c08c037a34a8b53c9d01ef0452d75b65eb52520e96b01e659")
    );
}