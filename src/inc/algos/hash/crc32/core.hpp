// -------------------------------------------------------------
//
//!\file core.hpp
//!\brief Core CRC32 implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_CRC32_CORE_HPP
#define OPERON_ALGOS_HASH_CRC32_CORE_HPP

// system
#include <cstddef>
#include <cstdint>
#include <vector>

namespace OPERON::Algos::Hash::CRC32::Core {

    //!\brief Compute CRC32 checksum bytes
    std::vector<uint8_t> hash(const uint8_t* input, size_t length);

} // namespace OPERON::Algos::Hash::CRC32::Core

#endif // OPERON_ALGOS_HASH_CRC32_CORE_HPP
