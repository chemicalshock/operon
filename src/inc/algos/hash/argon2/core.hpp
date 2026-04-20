// -------------------------------------------------------------
//
//!\file core.hpp
//!\brief Core Argon2 implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGOS_HASH_ARGON2_CORE_HPP
#define OPERON_ALGOS_HASH_ARGON2_CORE_HPP

// system
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace OPERON::Algos::Hash::Argon2::Core {

    enum class Variant
    {
        Argon2d = 0,
        Argon2i = 1,
        Argon2id = 2
    };

    struct Params
    {
        //!\brief Argon2 variant
        Variant variant = Variant::Argon2id;

        //!\brief Argon2 version
        uint32_t version = 0x13;

        //!\brief Number of passes
        uint32_t time_cost = 3;

        //!\brief Memory usage in KiB
        uint32_t memory_cost_kib = 65536;

        //!\brief Degree of parallelism
        uint32_t parallelism = 1;

        //!\brief Output hash length in bytes
        uint32_t hash_length = 32;

        //!\brief Optional secret value
        std::vector<uint8_t> secret;

        //!\brief Optional associated data
        std::vector<uint8_t> associated_data;
    };

    struct Block
    {
        //!\brief 1024-byte block as 128 64-bit words
        uint64_t v[128];
    };

    struct Instance
    {
        //!\brief Active parameters
        Params params;

        //!\brief Total number of memory blocks
        uint32_t memory_blocks = 0;

        //!\brief Number of blocks per lane
        uint32_t lane_length = 0;

        //!\brief Number of blocks per segment
        uint32_t segment_length = 0;

        //!\brief Allocated memory matrix
        std::vector<Block> memory;
    };

    //!\brief Validate Argon2 parameters and input sizes
    void validate_params(const Params& params, size_t password_length, size_t salt_length);

    //!\brief Initialise instance layout and allocate memory
    void initialise_instance(Instance& instance, const Params& params);

    //!\brief Initialise the first two blocks of each lane
    void initialise_first_blocks(
        Instance& instance,
        const uint8_t* password, size_t password_length,
        const uint8_t* salt, size_t salt_length
    );

    //!\brief Fill the memory matrix
    void fill_memory_blocks(Instance& instance);

    //!\brief Finalise the memory matrix into the output hash
    std::vector<uint8_t> finalise_tag(const Instance& instance, uint32_t hash_length);

    //!\brief Compute a raw Argon2 hash
    std::vector<uint8_t> hash_raw(
        const Params& params,
        const uint8_t* password, size_t password_length,
        const uint8_t* salt, size_t salt_length
    );

} // namespace OPERON::Algos::Hash::Argon2::Core

#endif // OPERON_ALGOS_HASH_ARGON2_CORE_HPP