// -------------------------------------------------------------
//
//!\file builtins.cpp
//!\brief Built-in algorithm registration for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "operon.hpp"

// internal
#include "algos/hash/blake2b/blake2b.hpp"
#include "algos/hash/crc32/crc32.hpp"
#include "algos/hash/md5/md5.hpp"
#include "algos/test/echo/echo.hpp"
#include "algos/rng/random_bytes/random_bytes.hpp"
#include "algos/hash/argon2/argon2.hpp"

//
//!\brief Register built-in algorithms with the runtime
//
bool OPERON::Operon::register_builtins()
{
    bool ok = true;

    ok = register_algorithm(Algos::Test::make_echo_algorithm()) && ok;
    ok = register_algorithm(Algos::Hash::Blake2b::make_algorithm()) && ok;
    ok = register_algorithm(Algos::Hash::CRC32::make_algorithm()) && ok;
    ok = register_algorithm(Algos::Hash::MD5::make_algorithm()) && ok;
    ok = register_algorithm(Algos::Rng::RandomBytes::make_algorithm()) && ok;
    ok = register_algorithm(Algos::Hash::Argon2::make_algorithm()) && ok;
    return ok;
}
