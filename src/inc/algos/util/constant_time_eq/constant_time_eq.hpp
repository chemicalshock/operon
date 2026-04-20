// -------------------------------------------------------------
//
//!\file constant_time_eq.hpp
//!\brief Constant-time equality algorithm factory for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_UTIL_CONSTANT_TIME_EQ_CONSTANT_TIME_EQ_HPP
#define OPERON_ALGOS_UTIL_CONSTANT_TIME_EQ_CONSTANT_TIME_EQ_HPP

// internal
#include "algorithm.hpp"

namespace OPERON::Algos::Util::ConstantTimeEq {

    //!\brief Create the util.constant_time_eq algorithm
    Algorithm make_algorithm();

} // namespace OPERON::Algos::Util::ConstantTimeEq

#endif // OPERON_ALGOS_UTIL_CONSTANT_TIME_EQ_CONSTANT_TIME_EQ_HPP