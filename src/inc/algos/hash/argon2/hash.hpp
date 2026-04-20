// -------------------------------------------------------------
//
//!\file hash.hpp
//!\brief Argon2 hashing helpers (H0 and blake2b_long)
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_ARGON2_HASH_HPP
#define OPERON_ALGOS_HASH_ARGON2_HASH_HPP

// system
#include <cstddef>
#include <cstdint>
#include <vector>

// module
#include "core.hpp"

namespace OPERON::Algos::Hash::Argon2::Core {

    //!\brief Store 32-bit value in little-endian
    void store32(uint8_t* dst, uint32_t value);

    //!\brief Store 64-bit value in little-endian
    void store64(uint8_t* dst, uint64_t value);

    //!\brief Compute initial hash H0
    std::vector<uint8_t> compute_h0(
        const Params& params,
        const uint8_t* password, size_t password_length,
        const uint8_t* salt, size_t salt_length
    );

    //!\brief Variable-length BLAKE2b hash (H')
    std::vector<uint8_t> blake2b_long(
        const uint8_t* input,
        size_t input_length,
        size_t output_length
    );

} // namespace OPERON::Algos::Hash::Argon2::Core

#endif // OPERON_ALGOS_HASH_ARGON2_HASH_HPP