// -------------------------------------------------------------
//
//!\file crc32.hpp
//!\brief CRC32 algorithm factory for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_CRC32_CRC32_HPP
#define OPERON_ALGOS_HASH_CRC32_CRC32_HPP

// internal
#include "algorithm.hpp"

namespace OPERON::Algos::Hash::CRC32 {

    //!\brief Create the hash.crc32 algorithm
    Algorithm make_algorithm();

} // namespace OPERON::Algos::Hash::CRC32

#endif // OPERON_ALGOS_HASH_CRC32_CRC32_HPP
