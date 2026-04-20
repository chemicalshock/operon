// -------------------------------------------------------------
//
//!\file blake2b.cpp
//!\brief BLAKE2b algorithm implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "algos/hash/blake2b/blake2b.hpp"

// system
#include <exception>

// internal
#include "algos/hash/blake2b/core.hpp"

//
//!\brief Create the hash.blake2b algorithm
//
OPERON::Algorithm OPERON::Algos::Hash::Blake2b::make_algorithm()
{
    OPERON::Algorithm algorithm;

    algorithm.info.name = "hash.blake2b";
    algorithm.info.category = "hash";
    algorithm.info.brief = "Generate a BLAKE2b hash from input data";
    algorithm.info.parameters = {
        { "input", "Input text to hash", OPERON::ValueType::String, true },
        { "hash_length", "Hash output length in bytes", OPERON::ValueType::Int, true }
    };
    algorithm.info.return_type = OPERON::ValueType::Buffer;

    algorithm.function = [](OPERON::Context&, const std::vector<OPERON::Value>& args) -> OPERON::Result
    {
        if (args.size() < 2)
        {
            return OPERON::Result::error(
                "E001_MISSING_ARGUMENT",
                "hash.blake2b requires 2 arguments: input, hash_length"
            );
        }

        if (!args[0].is_string())
        {
            return OPERON::Result::error(
                "E002_INVALID_TYPE",
                "input must be a string"
            );
        }

        if (!args[1].is_int())
        {
            return OPERON::Result::error(
                "E002_INVALID_TYPE",
                "hash_length must be an int"
            );
        }

        const std::string& input = args[0].as_string();
        const int64_t hash_length = args[1].as_int();

        if (hash_length <= 0 || hash_length > 64)
        {
            return OPERON::Result::error(
                "E003_INVALID_VALUE",
                "hash_length must be between 1 and 64"
            );
        }

        try
        {
            const OPERON::Value::Buffer output = OPERON::Algos::Hash::Blake2b::Core::hash(
                reinterpret_cast<const uint8_t*>(input.data()),
                input.size(),
                static_cast<size_t>(hash_length)
            );

            return OPERON::Result::success(OPERON::Value(output));
        }
        catch (const std::exception& ex)
        {
            return OPERON::Result::error(
                "E004_BLAKE2B_FAILED",
                ex.what()
            );
        }
    };

    return algorithm;
}