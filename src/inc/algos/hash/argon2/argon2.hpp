// -------------------------------------------------------------
//
//!\file argon2.hpp
//!\brief Argon2id algorithm factory for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_ARGON2_ARGON2_HPP
#define OPERON_ALGOS_HASH_ARGON2_ARGON2_HPP

// internal
#include "algorithm.hpp"

namespace OPERON::Algos::Hash::Argon2 {

    //!\brief Create the hash.argon2id algorithm
    Algorithm make_algorithm();

} // namespace OPERON::Algos::Hash::Argon2

#endif // OPERON_ALGOS_HASH_ARGON2_ARGON2_HPP