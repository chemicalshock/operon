// -------------------------------------------------------------
//
//!\file md5.cpp
//!\brief MD5 algorithm implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

// own header
#include "algos/hash/md5/md5.hpp"

// internal
#include "algos/hash/md5/core.hpp"

//
//!\brief Create the hash.md5 algorithm
//
OPERON::Algorithm OPERON::Algos::Hash::MD5::make_algorithm()
{
    OPERON::Algorithm algorithm;

    algorithm.info.name = "hash.md5";
    algorithm.info.category = "hash";
    algorithm.info.brief = "Generate an MD5 hash (insecure, for legacy use only)";
    algorithm.info.parameters = {
        { "input", "Input text to hash", OPERON::ValueType::String, true }
    };
    algorithm.info.return_type = OPERON::ValueType::Buffer;

    algorithm.function = [](OPERON::Context&, const std::vector<OPERON::Value>& args) -> OPERON::Result
    {
        if (args.empty())
        {
            return OPERON::Result::error(
                "E001_MISSING_ARGUMENT",
                "hash.md5 requires input"
            );
        }

        if (!args[0].is_string())
        {
            return OPERON::Result::error(
                "E002_INVALID_TYPE",
                "input must be a string"
            );
        }

        const std::string& input = args[0].as_string();

        const OPERON::Value::Buffer digest =
            OPERON::Algos::Hash::MD5::Core::hash(
                reinterpret_cast<const uint8_t*>(input.data()),
                input.size()
            );

        return OPERON::Result::success(OPERON::Value(digest));
    };

    return algorithm;
}