// -------------------------------------------------------------
//
//!\file crc32.cpp
//!\brief CRC32 algorithm implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

// own header
#include "algos/hash/crc32/crc32.hpp"

// internal
#include "algos/hash/crc32/core.hpp"

//
//!\brief Create the hash.crc32 algorithm
//
OPERON::Algorithm OPERON::Algos::Hash::CRC32::make_algorithm()
{
    OPERON::Algorithm algorithm;

    algorithm.info.name = "hash.crc32";
    algorithm.info.category = "hash";
    algorithm.info.brief = "Generate a CRC32 checksum from input data";
    algorithm.info.parameters = {
        { "input", "Input text to checksum", OPERON::ValueType::String, true }
    };
    algorithm.info.return_type = OPERON::ValueType::Buffer;

    algorithm.function = [](OPERON::Context&, const std::vector<OPERON::Value>& args) -> OPERON::Result
    {
        if (args.empty())
        {
            return OPERON::Result::error(
                "E001_MISSING_ARGUMENT",
                "hash.crc32 requires input"
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
            OPERON::Algos::Hash::CRC32::Core::hash(
                reinterpret_cast<const uint8_t*>(input.data()),
                input.size()
            );

        return OPERON::Result::success(OPERON::Value(digest));
    };

    return algorithm;
}
