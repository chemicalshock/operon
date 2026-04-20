// -------------------------------------------------------------
//
//!\file random_bytes.cpp
//!\brief Random bytes algorithm implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "algos/rng/random_bytes/random_bytes.hpp"

// internal
#include "utils.hpp"

//
//!\brief Create the rng.random_bytes algorithm
//
OPERON::Algorithm OPERON::Algos::Rng::RandomBytes::make_algorithm()
{
    OPERON::Algorithm algorithm;

    algorithm.info.name = "rng.random_bytes";
    algorithm.info.category = "rng";
    algorithm.info.brief = "Generate a buffer of random bytes";
    algorithm.info.parameters = {
        { "length", "Number of random bytes to generate", OPERON::ValueType::Int, true }
    };
    algorithm.info.return_type = OPERON::ValueType::Buffer;

    algorithm.function = [](OPERON::Context&, const std::vector<OPERON::Value>& args) -> OPERON::Result
    {
        if (args.empty())
        {
            return OPERON::Result::error(
                "E001_MISSING_ARGUMENT",
                "rng.random_bytes requires length"
            );
        }

        if (!args[0].is_int())
        {
            return OPERON::Result::error(
                "E002_INVALID_TYPE",
                "length must be an int"
            );
        }

        const int64_t length = args[0].as_int();

        if (length < 0)
        {
            return OPERON::Result::error(
                "E003_INVALID_VALUE",
                "length must not be negative"
            );
        }

        const OPERON::Value::Buffer output =
            OPERON::Utils::random_bytes(static_cast<size_t>(length));

        return OPERON::Result::success(OPERON::Value(output));
    };

    return algorithm;
}