// -------------------------------------------------------------
//
//!\file random_bytes.hpp
//!\brief Random bytes algorithm factory for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_RNG_RANDOM_BYTES_RANDOM_BYTES_HPP
#define OPERON_ALGOS_RNG_RANDOM_BYTES_RANDOM_BYTES_HPP

// internal
#include "algorithm.hpp"

namespace OPERON::Algos::Rng::RandomBytes {

    //!\brief Create the rng.random_bytes algorithm
    Algorithm make_algorithm();

} // namespace OPERON::Algos::Rng::RandomBytes

#endif