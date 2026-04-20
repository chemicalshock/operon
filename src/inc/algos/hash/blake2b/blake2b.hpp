// -------------------------------------------------------------
//
//!\file blake2b.hpp
//!\brief BLAKE2b algorithm factory for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_BLAKE2B_BLAKE2B_HPP
#define OPERON_ALGOS_HASH_BLAKE2B_BLAKE2B_HPP

// internal
#include "algorithm.hpp"

namespace OPERON::Algos::Hash::Blake2b {

    //!\brief Create the hash.blake2b algorithm
    Algorithm make_algorithm();

} // namespace OPERON::Algos::Hash::Blake2b

#endif