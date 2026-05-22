// -------------------------------------------------------------
//
//!\file core.hpp
//!\brief Core ChaCha20-Poly1305 implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_AEAD_CHACHA20POLY1305_CORE_HPP
#define OPERON_ALGOS_AEAD_CHACHA20POLY1305_CORE_HPP

// system
#include <cstddef>
#include <cstdint>
#include <vector>

namespace OPERON::Algos::Aead::ChaCha20Poly1305::Core {

    constexpr size_t KEY_SIZE = 32;
    constexpr size_t NONCE_SIZE = 12;
    constexpr size_t TAG_SIZE = 16;

    //!\brief Encrypt plaintext and append the authentication tag
    std::vector<uint8_t> seal(
        const uint8_t* plaintext,
        size_t plaintext_length,
        const uint8_t* aad,
        size_t aad_length,
        const uint8_t* key,
        size_t key_length,
        const uint8_t* nonce,
        size_t nonce_length
    );

    //!\brief Verify the authentication tag and decrypt the ciphertext
    bool open(
        const uint8_t* ciphertext_and_tag,
        size_t ciphertext_and_tag_length,
        const uint8_t* aad,
        size_t aad_length,
        const uint8_t* key,
        size_t key_length,
        const uint8_t* nonce,
        size_t nonce_length,
        std::vector<uint8_t>& plaintext
    );

} // namespace OPERON::Algos::Aead::ChaCha20Poly1305::Core

#endif // OPERON_ALGOS_AEAD_CHACHA20POLY1305_CORE_HPP
