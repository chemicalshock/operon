// -------------------------------------------------------------
//
//!\file core.hpp
//!\brief Core MD5 implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_MD5_CORE_HPP
#define OPERON_ALGOS_HASH_MD5_CORE_HPP

// system
#include <cstddef>
#include <cstdint>
#include <vector>

namespace OPERON::Algos::Hash::MD5::Core {

    //!\brief Compute MD5 hash
    std::vector<uint8_t> hash(const uint8_t* input, size_t length);

} // namespace OPERON::Algos::Hash::MD5::Core

#endif // OPERON_ALGOS_HASH_MD5_CORE_HPP