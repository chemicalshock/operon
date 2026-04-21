// -------------------------------------------------------------
//
//!\file core.cpp
//!\brief Core Argon2 implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "algos/hash/argon2/core.hpp"

// system
#include <cstring>
#include <stdexcept>

// internal
#include "hash.hpp"

namespace
{
    constexpr uint32_t ARGON2_SYNC_POINTS = 4;
    constexpr uint32_t ARGON2_BLOCK_SIZE = 1024;
    constexpr uint32_t ARGON2_WORDS_IN_BLOCK = 128;
    constexpr uint32_t ARGON2_MIN_LANES = 1;
    constexpr uint32_t ARGON2_MIN_MEMORY_BLOCKS = 2 * ARGON2_SYNC_POINTS;

    //
    //!\brief Clear one block
    //
    void clear_block(OPERON::Algos::Hash::Argon2::Core::Block& block)
    {
        std::memset(block.v, 0, sizeof(block.v));
    }

    //
    //!\brief Rotate right 64-bit
    //
    static inline uint64_t rotr64(uint64_t x, uint32_t n)
    {
        return (x >> n) | (x << (64 - n));
    }

    //
    //!\brief Argon2/BLAKE2 G function on four words
    //
    static inline uint64_t lower32_product(uint64_t a, uint64_t b)
    {
        return
            static_cast<uint64_t>(static_cast<uint32_t>(a)) *
            static_cast<uint64_t>(static_cast<uint32_t>(b));
    }

    //
    //!\brief Argon2/BLAKE2 G function on four words
    //
    static inline void G_round(uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d)
    {
        a = a + b + 2ULL * lower32_product(a, b);
        d = rotr64(d ^ a, 32);

        c = c + d + 2ULL * lower32_product(c, d);
        b = rotr64(b ^ c, 24);

        a = a + b + 2ULL * lower32_product(a, b);
        d = rotr64(d ^ a, 16);

        c = c + d + 2ULL * lower32_product(c, d);
        b = rotr64(b ^ c, 63);
    }

    //
    //!\brief One BLAKE2 round without message words
    //
    static inline void blake2_round_nomsg(
        uint64_t& v0, uint64_t& v1, uint64_t& v2, uint64_t& v3,
        uint64_t& v4, uint64_t& v5, uint64_t& v6, uint64_t& v7,
        uint64_t& v8, uint64_t& v9, uint64_t& v10, uint64_t& v11,
        uint64_t& v12, uint64_t& v13, uint64_t& v14, uint64_t& v15
    )
    {
        G_round(v0, v4, v8, v12);
        G_round(v1, v5, v9, v13);
        G_round(v2, v6, v10, v14);
        G_round(v3, v7, v11, v15);

        G_round(v0, v5, v10, v15);
        G_round(v1, v6, v11, v12);
        G_round(v2, v7, v8, v13);
        G_round(v3, v4, v9, v14);
    }

    //
    //!\brief Apply permutation P to block (RFC 9106 compliant)
    //
    static void P(OPERON::Algos::Hash::Argon2::Core::Block& x)
    {
        for (size_t row = 0; row < 8; ++row)
        {
            const size_t base = row * 16;

            blake2_round_nomsg(
                x.v[base + 0], x.v[base + 1], x.v[base + 2], x.v[base + 3],
                x.v[base + 4], x.v[base + 5], x.v[base + 6], x.v[base + 7],
                x.v[base + 8], x.v[base + 9], x.v[base + 10], x.v[base + 11],
                x.v[base + 12], x.v[base + 13], x.v[base + 14], x.v[base + 15]
            );
        }

        for (size_t column = 0; column < 8; ++column)
        {
            const size_t base = column * 2;

            blake2_round_nomsg(
                x.v[base + 0], x.v[base + 1],
                x.v[base + 16], x.v[base + 17],
                x.v[base + 32], x.v[base + 33],
                x.v[base + 48], x.v[base + 49],
                x.v[base + 64], x.v[base + 65],
                x.v[base + 80], x.v[base + 81],
                x.v[base + 96], x.v[base + 97],
                x.v[base + 112], x.v[base + 113]
            );
        }
    }

    //
    //!\brief Argon2 block compression G
    //
    static void G(
        OPERON::Algos::Hash::Argon2::Core::Block& out,
        const OPERON::Algos::Hash::Argon2::Core::Block& a,
        const OPERON::Algos::Hash::Argon2::Core::Block& b
    )
    {
        using Block = OPERON::Algos::Hash::Argon2::Core::Block;

        Block r;
        Block z;

        // r = a XOR b
        for (size_t i = 0; i < 128; ++i)
        {
            r.v[i] = a.v[i] ^ b.v[i];
            z.v[i] = r.v[i]; // copy to z
        }

        // z = P(r)
        P(z);

        // out = z XOR r
        for (size_t i = 0; i < 128; ++i)
        {
            out.v[i] = z.v[i] ^ r.v[i];
        }
    }

    //
    //!\brief Read little-endian 64-bit
    //
    static uint64_t load64(const uint64_t x)
    {
        return x;
    }

    //
    //!\brief Derive J1/J2 from previous block (Argon2d style)
    //
    static void get_j1_j2_from_block(
        const OPERON::Algos::Hash::Argon2::Core::Block& prev,
        uint32_t& j1,
        uint32_t& j2
    )
    {
        const uint64_t x = load64(prev.v[0]);
        j1 = static_cast<uint32_t>(x & 0xFFFFFFFFu);
        j2 = static_cast<uint32_t>((x >> 32) & 0xFFFFFFFFu);
    }

    //
    //!\brief Choose reference lane (Argon2id rules)
    //
    static uint32_t get_ref_lane(
        uint32_t j2,
        uint32_t current_lane,
        uint32_t pass,
        uint32_t slice,
        uint32_t lanes
    )
    {
        if (pass == 0 && slice == 0)
        {
            return current_lane;
        }
        return j2 % lanes;
    }

    //
    //!\brief Compute the size of the valid reference area
    //
    static uint32_t get_reference_area_size(
        uint32_t pass,
        uint32_t slice,
        uint32_t index_in_segment,
        uint32_t lane_length,
        uint32_t segment_length,
        uint32_t current_lane,
        uint32_t ref_lane
    )
    {
        if (pass == 0)
        {
            if (current_lane == ref_lane)
            {
                return slice * segment_length + index_in_segment - 1;
            }

            if (index_in_segment == 0)
            {
                return slice * segment_length - 1;
            }

            return slice * segment_length;
        }

        if (current_lane == ref_lane)
        {
            return lane_length - segment_length + index_in_segment - 1;
        }

        if (index_in_segment == 0)
        {
            return lane_length - segment_length - 1;
        }

        return lane_length - segment_length;
    }

    //
    //!\brief Map J1 into a relative position inside the reference area
    //
    static uint32_t get_relative_position(uint32_t j1, uint32_t reference_area_size)
    {
        const uint64_t x = j1;
        const uint64_t y = (x * x) >> 32;
        const uint64_t z = (static_cast<uint64_t>(reference_area_size) * y) >> 32;

        return reference_area_size - 1 - static_cast<uint32_t>(z);
    }

    //
    //!\brief Compute absolute reference index inside the chosen lane
    //
    static uint32_t get_absolute_ref_index(
        uint32_t pass,
        uint32_t slice,
        uint32_t relative_position,
        uint32_t lane_length,
        uint32_t segment_length
    )
    {
        uint32_t start_position;

        if (pass == 0)
        {
            start_position = 0;
        }
        else
        {
            start_position = ((slice + 1) * segment_length) % lane_length;
        }

        return (start_position + relative_position) % lane_length;
    }

    //
    //!\brief Return true if this position uses Argon2i-style addressing in Argon2id
    //
    static bool use_argon2i_addressing(
        OPERON::Algos::Hash::Argon2::Core::Variant variant,
        uint32_t pass,
        uint32_t slice
    )
    {
        if (variant == OPERON::Algos::Hash::Argon2::Core::Variant::Argon2i)
        {
            return true;
        }

        if (variant == OPERON::Algos::Hash::Argon2::Core::Variant::Argon2id)
        {
            return (pass == 0 && slice < 2);
        }

        return false;
    }

    //
    //!\brief Store a 64-bit value in little-endian
    //
    static void store64_le(uint8_t* dst, uint64_t value)
    {
        dst[0] = static_cast<uint8_t>((value >> 0)  & 0xFFU);
        dst[1] = static_cast<uint8_t>((value >> 8)  & 0xFFU);
        dst[2] = static_cast<uint8_t>((value >> 16) & 0xFFU);
        dst[3] = static_cast<uint8_t>((value >> 24) & 0xFFU);
        dst[4] = static_cast<uint8_t>((value >> 32) & 0xFFU);
        dst[5] = static_cast<uint8_t>((value >> 40) & 0xFFU);
        dst[6] = static_cast<uint8_t>((value >> 48) & 0xFFU);
        dst[7] = static_cast<uint8_t>((value >> 56) & 0xFFU);
    }

    //
    //!\brief Build the Argon2i address input block bytes
    //
    static OPERON::Algos::Hash::Argon2::Core::Block make_argon2i_input_block(
        uint32_t pass,
        uint32_t lane,
        uint32_t slice,
        uint32_t memory_blocks,
        uint32_t time_cost,
        uint32_t variant,
        uint64_t counter
    )
    {
        using Block = OPERON::Algos::Hash::Argon2::Core::Block;

        Block block;
        uint8_t bytes[1024];

        std::memset(bytes, 0, sizeof(bytes));

        store64_le(bytes + 0,  pass);
        store64_le(bytes + 8,  lane);
        store64_le(bytes + 16, slice);
        store64_le(bytes + 24, memory_blocks);
        store64_le(bytes + 32, time_cost);
        store64_le(bytes + 40, variant);
        store64_le(bytes + 48, counter);

        std::memcpy(block.v, bytes, sizeof(bytes));
        return block;
    }

    //
    //!\brief Generate one Argon2i address block
    //
    static OPERON::Algos::Hash::Argon2::Core::Block make_argon2i_address_block(
        uint32_t pass,
        uint32_t lane,
        uint32_t slice,
        uint32_t memory_blocks,
        uint32_t time_cost,
        uint32_t variant,
        uint64_t counter
    )
    {
        using Block = OPERON::Algos::Hash::Argon2::Core::Block;

        Block zero_block;
        Block input_block;
        Block tmp_block;
        Block address_block;

        std::memset(zero_block.v, 0, sizeof(zero_block.v));

        input_block = make_argon2i_input_block(
            pass,
            lane,
            slice,
            memory_blocks,
            time_cost,
            variant,
            counter
        );

        G(tmp_block, zero_block, input_block);
        G(address_block, zero_block, tmp_block);

        return address_block;
    }    
}

//
//!\brief Validate Argon2 parameters and input sizes
//
void OPERON::Algos::Hash::Argon2::Core::validate_params(
    const Params& params,
    size_t password_length,
    size_t salt_length
)
{
    (void)password_length;

    if (params.parallelism < ARGON2_MIN_LANES)
    {
        throw std::runtime_error("Argon2 parallelism must be at least 1");
    }

    if (params.time_cost == 0)
    {
        throw std::runtime_error("Argon2 time_cost must be greater than 0");
    }

    if (params.hash_length == 0)
    {
        throw std::runtime_error("Argon2 hash_length must be greater than 0");
    }

    if (salt_length == 0)
    {
        throw std::runtime_error("Argon2 salt must not be empty");
    }

    if (params.memory_cost_kib < (ARGON2_MIN_MEMORY_BLOCKS * params.parallelism))
    {
        throw std::runtime_error("Argon2 memory_cost_kib is too small for the selected parallelism");
    }
}

//
//!\brief Initialise instance layout and allocate memory
//
void OPERON::Algos::Hash::Argon2::Core::initialise_instance(Instance& instance, const Params& params)
{
    uint32_t memory_blocks;

    instance.params = params;

    memory_blocks = params.memory_cost_kib;

    if (memory_blocks < (ARGON2_MIN_MEMORY_BLOCKS * params.parallelism))
    {
        memory_blocks = ARGON2_MIN_MEMORY_BLOCKS * params.parallelism;
    }

    memory_blocks = (memory_blocks / (params.parallelism * ARGON2_SYNC_POINTS)) *
                    (params.parallelism * ARGON2_SYNC_POINTS);

    instance.memory_blocks = memory_blocks;
    instance.lane_length = memory_blocks / params.parallelism;
    instance.segment_length = instance.lane_length / ARGON2_SYNC_POINTS;
    instance.memory.resize(instance.memory_blocks);

    for (uint32_t i = 0; i < instance.memory_blocks; ++i)
    {
        clear_block(instance.memory[i]);
    }
}

//
//!\brief Initialise the first two blocks of each lane
//
void OPERON::Algos::Hash::Argon2::Core::initialise_first_blocks(
    Instance& instance,
    const uint8_t* password,
    size_t password_length,
    const uint8_t* salt,
    size_t salt_length
)
{
    const Params& params = instance.params;

    // Step 1: compute H0
    const std::vector<uint8_t> h0 =
        compute_h0(params, password, password_length, salt, salt_length);

    std::vector<uint8_t> buffer;
    buffer.resize(h0.size() + 8); // H0 + 2x uint32

    for (uint32_t lane = 0; lane < params.parallelism; ++lane)
    {
        for (uint32_t block_index = 0; block_index < 2; ++block_index)
        {
            uint8_t* ptr = buffer.data();

            // copy H0
            std::memcpy(ptr, h0.data(), h0.size());
            ptr += h0.size();

            // append block index
            store32(ptr, block_index);
            ptr += 4;

            // append lane index
            store32(ptr, lane);

            // compute 1024-byte block
            std::vector<uint8_t> block_bytes =
                blake2b_long(buffer.data(), buffer.size(), ARGON2_BLOCK_SIZE);

            // map into memory
            const uint32_t position =
                lane * instance.lane_length + block_index;

            Block& block = instance.memory[position];

            std::memcpy(block.v, block_bytes.data(), ARGON2_BLOCK_SIZE);
        }
    }
}

//
//!\brief Fill the memory matrix (Argon2id, single-threaded)
//
void OPERON::Algos::Hash::Argon2::Core::fill_memory_blocks(Instance& instance)
{
    const Params& params = instance.params;

    const uint32_t lanes = params.parallelism;
    const uint32_t segment_length = instance.segment_length;
    const uint32_t lane_length = instance.lane_length;

    for (uint32_t pass = 0; pass < params.time_cost; ++pass)
    {
        for (uint32_t slice = 0; slice < 4; ++slice)
        {
            for (uint32_t lane = 0; lane < lanes; ++lane)
            {
                for (uint32_t i = 0; i < segment_length; ++i)
                {
                    uint32_t index = slice * segment_length + i;

                    // skip first 2 blocks already initialised
                    if (pass == 0 && index < 2)
                    {
                        continue;
                    }

                    const uint32_t current_index = lane * lane_length + index;

                    const uint32_t prev_index =
                        (index == 0)
                        ? (lane * lane_length + lane_length - 1)
                        : (current_index - 1);

                    const Block& prev_block = instance.memory[prev_index];

                    uint32_t j1;
                    uint32_t j2;

                    if (use_argon2i_addressing(params.variant, pass, slice))
                    {
                        const uint64_t counter = 1 + (i / 128);
                        const Block address_block = make_argon2i_address_block(
                            pass,
                            lane,
                            slice,
                            instance.memory_blocks,
                            params.time_cost,
                            static_cast<uint32_t>(params.variant),
                            counter
                        );

                        const uint64_t x = address_block.v[i % 128];
                        j1 = static_cast<uint32_t>(x & 0xFFFFFFFFu);
                        j2 = static_cast<uint32_t>((x >> 32) & 0xFFFFFFFFu);
                    }
                    else
                    {
                        get_j1_j2_from_block(prev_block, j1, j2);
                    }

                    const uint32_t ref_lane =
                        get_ref_lane(j2, lane, pass, slice, lanes);

                    uint32_t reference_area_size =
                        get_reference_area_size(
                            pass,
                            slice,
                            i,
                            lane_length,
                            segment_length,
                            lane,
                            ref_lane
                        );

                    if (reference_area_size == 0)
                    {
                        reference_area_size = 1;
                    }

                    const uint32_t relative_position =
                        get_relative_position(j1, reference_area_size);

                    const uint32_t ref_index_local =
                        get_absolute_ref_index(
                            pass,
                            slice,
                            relative_position,
                            lane_length,
                            segment_length
                        );

                    const uint32_t ref_index =
                        ref_lane * lane_length + ref_index_local;

                    Block& current = instance.memory[current_index];
                    const Block& ref_block = instance.memory[ref_index];

                    Block g_out;
                    G(g_out, prev_block, ref_block);

                    if (pass == 0)
                    {
                        // First pass: overwrite
                        current = g_out;
                    }
                    else
                    {
                        // Subsequent passes: XOR with existing block
                        for (size_t w = 0; w < 128; ++w)
                        {
                            current.v[w] ^= g_out.v[w];
                        }
                    }
                }
            }
        }
    }
}

//
//!\brief Finalise the memory matrix into the output hash
//
std::vector<uint8_t> OPERON::Algos::Hash::Argon2::Core::finalise_tag(
    const Instance& instance,
    uint32_t hash_length
)
{
    Block final_block;
    std::vector<uint8_t> final_bytes(ARGON2_BLOCK_SIZE);

    clear_block(final_block);

    for (uint32_t lane = 0; lane < instance.params.parallelism; ++lane)
    {
        const uint32_t last_index =
            lane * instance.lane_length + (instance.lane_length - 1);

        const Block& lane_last_block = instance.memory[last_index];

        for (size_t i = 0; i < ARGON2_WORDS_IN_BLOCK; ++i)
        {
            final_block.v[i] ^= lane_last_block.v[i];
        }
    }

    std::memcpy(final_bytes.data(), final_block.v, ARGON2_BLOCK_SIZE);

    return blake2b_long(
        final_bytes.data(),
        final_bytes.size(),
        hash_length
    );
}

//
//!\brief Compute a raw Argon2 hash
//
std::vector<uint8_t> OPERON::Algos::Hash::Argon2::Core::hash_raw(
    const Params& params,
    const uint8_t* password,
    size_t password_length,
    const uint8_t* salt,
    size_t salt_length
)
{
    Instance instance;

    validate_params(params, password_length, salt_length);
    initialise_instance(instance, params);
    initialise_first_blocks(instance, password, password_length, salt, salt_length);
    fill_memory_blocks(instance);

    return finalise_tag(instance, params.hash_length);
}
