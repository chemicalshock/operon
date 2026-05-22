// -------------------------------------------------------------
//
//!\file chacha20poly1305.hpp
//!\brief ChaCha20-Poly1305 algorithm factories for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_AEAD_CHACHA20POLY1305_CHACHA20POLY1305_HPP
#define OPERON_ALGOS_AEAD_CHACHA20POLY1305_CHACHA20POLY1305_HPP

// internal
#include "algorithm.hpp"

namespace OPERON::Algos::Aead::ChaCha20Poly1305 {

    //!\brief Create the aead.chacha20poly1305_encrypt algorithm
    Algorithm make_encrypt_algorithm();

    //!\brief Create the aead.chacha20poly1305_decrypt algorithm
    Algorithm make_decrypt_algorithm();

} // namespace OPERON::Algos::Aead::ChaCha20Poly1305

#endif // OPERON_ALGOS_AEAD_CHACHA20POLY1305_CHACHA20POLY1305_HPP
