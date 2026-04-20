// -------------------------------------------------------------
//
//!\file constant_time_eq.cpp
//!\brief Constant-time equality algorithm implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "algos/util/constant_time_eq/constant_time_eq.hpp"

// internal
#include "utils.hpp"

//
//!\brief Create the util.constant_time_eq algorithm
//
OPERON::Algorithm OPERON::Algos::Util::ConstantTimeEq::make_algorithm()
{
    OPERON::Algorithm algorithm;

    algorithm.info.name = "util.constant_time_eq";
    algorithm.info.category = "util";
    algorithm.info.brief = "Compare two strings or buffers in constant time";
    algorithm.info.parameters = {
        { "a", "First string or buffer", OPERON::ValueType::String, true },
        { "b", "Second string or buffer", OPERON::ValueType::String, true }
    };
    algorithm.info.return_type = OPERON::ValueType::Bool;

    algorithm.function = [](OPERON::Context&, const std::vector<OPERON::Value>& args) -> OPERON::Result
    {
        if (args.size() < 2)
        {
            return OPERON::Result::error(
                "E001_MISSING_ARGUMENT",
                "util.constant_time_eq requires 2 arguments: a, b"
            );
        }

        if (args[0].is_string() && args[1].is_string())
        {
            const bool equal = OPERON::Utils::constant_time_eq(
                args[0].as_string(),
                args[1].as_string()
            );

            return OPERON::Result::success(OPERON::Value(equal));
        }

        if (args[0].is_buffer() && args[1].is_buffer())
        {
            const bool equal = OPERON::Utils::constant_time_eq(
                args[0].as_buffer(),
                args[1].as_buffer()
            );

            return OPERON::Result::success(OPERON::Value(equal));
        }

        return OPERON::Result::error(
            "E002_INVALID_TYPE",
            "util.constant_time_eq requires both arguments to be strings or both to be buffers"
        );
    };

    return algorithm;
}