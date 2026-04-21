// -------------------------------------------------------------
//
//!\file argon2.cpp
//!\brief Argon2id algorithm implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "algos/hash/argon2/argon2.hpp"

// internal
#include "algos/hash/argon2/core.hpp"

//
//!\brief Create the hash.argon2id algorithm
//
OPERON::Algorithm OPERON::Algos::Hash::Argon2::make_algorithm()
{
    OPERON::Algorithm algorithm;

    algorithm.info.name = "hash.argon2id";
    algorithm.info.category = "hash";
    algorithm.info.brief = "Generate a raw Argon2id hash from password input";
    algorithm.info.parameters = {
        { "password", "Password text", OPERON::ValueType::String, true },
        { "salt", "Salt text", OPERON::ValueType::String, true },
        { "time_cost", "Number of passes", OPERON::ValueType::Int, true },
        { "memory_cost_kib", "Memory usage in KiB", OPERON::ValueType::Int, true },
        { "parallelism", "Degree of parallelism", OPERON::ValueType::Int, true },
        { "hash_length", "Output hash length in bytes", OPERON::ValueType::Int, true }
    };
    algorithm.info.return_type = OPERON::ValueType::Buffer;

    algorithm.function = [](OPERON::Context&, const std::vector<OPERON::Value>& args) -> OPERON::Result
    {
        using namespace OPERON::Algos::Hash::Argon2::Core;

        if (args.size() < 6)
        {
            return OPERON::Result::error(
                "E001_MISSING_ARGUMENT",
                "hash.argon2id requires 6 arguments: password, salt, time_cost, memory_cost_kib, parallelism, hash_length"
            );
        }

        if (!args[0].is_string())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "password must be a string");
        }

        if (!args[1].is_string())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "salt must be a string");
        }

        if (!args[2].is_int())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "time_cost must be an int");
        }

        if (!args[3].is_int())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "memory_cost_kib must be an int");
        }

        if (!args[4].is_int())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "parallelism must be an int");
        }

        if (!args[5].is_int())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "hash_length must be an int");
        }

        const std::string& password = args[0].as_string();
        const std::string& salt = args[1].as_string();
        const int64_t time_cost = args[2].as_int();
        const int64_t memory_cost_kib = args[3].as_int();
        const int64_t parallelism = args[4].as_int();
        const int64_t hash_length = args[5].as_int();

        if (time_cost <= 0)
        {
            return OPERON::Result::error("E003_INVALID_VALUE", "time_cost must be greater than 0");
        }

        if (memory_cost_kib <= 0)
        {
            return OPERON::Result::error("E003_INVALID_VALUE", "memory_cost_kib must be greater than 0");
        }

        if (parallelism <= 0)
        {
            return OPERON::Result::error("E003_INVALID_VALUE", "parallelism must be greater than 0");
        }

        if (hash_length <= 0)
        {
            return OPERON::Result::error("E003_INVALID_VALUE", "hash_length must be greater than 0");
        }

        try
        {
            Params params;
            params.variant = Variant::Argon2id;
            params.version = 0x13;
            params.time_cost = static_cast<uint32_t>(time_cost);
            params.memory_cost_kib = static_cast<uint32_t>(memory_cost_kib);
            params.parallelism = static_cast<uint32_t>(parallelism);
            params.hash_length = static_cast<uint32_t>(hash_length);

            const OPERON::Value::Buffer digest = hash_raw(
                params,
                reinterpret_cast<const uint8_t*>(password.data()),
                password.size(),
                reinterpret_cast<const uint8_t*>(salt.data()),
                salt.size()
            );

            return OPERON::Result::success(OPERON::Value(digest));
        }
        catch (const std::exception& ex)
        {
            return OPERON::Result::error("E004_ARGON2_FAILED", ex.what());
        }
        catch (...)
        {
            return OPERON::Result::error("E004_ARGON2_FAILED", "Unknown Argon2 failure");
        }
    };

    return algorithm;
}