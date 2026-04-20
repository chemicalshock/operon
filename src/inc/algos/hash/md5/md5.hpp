// -------------------------------------------------------------
//
//!\file md5.hpp
//!\brief MD5 algorithm factory for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_MD5_MD5_HPP
#define OPERON_ALGOS_HASH_MD5_MD5_HPP

// internal
#include "algorithm.hpp"

namespace OPERON::Algos::Hash::MD5 {

    //!\brief Create the hash.md5 algorithm
    Algorithm make_algorithm();

} // namespace OPERON::Algos::Hash::MD5

#endif // OPERON_ALGOS_HASH_MD5_MD5_HPP