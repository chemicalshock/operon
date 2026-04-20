// -------------------------------------------------------------
//
//!\file hash.cpp
//!\brief Argon2 hashing helpers implementation
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

// own header
#include "algos/hash/argon2/hash.hpp"

// internal
#include "algos/hash/blake2b/core.hpp"

// system
#include <cstring>

//
//!\brief Store 32-bit value in little-endian
//
void OPERON::Algos::Hash::Argon2::Core::store32(uint8_t* dst, uint32_t value)
{
    dst[0] = (value >> 0) & 0xFF;
    dst[1] = (value >> 8) & 0xFF;
    dst[2] = (value >> 16) & 0xFF;
    dst[3] = (value >> 24) & 0xFF;
}

//
//!\brief Store 64-bit value in little-endian
//
void OPERON::Algos::Hash::Argon2::Core::store64(uint8_t* dst, uint64_t value)
{
    for (int i = 0; i < 8; ++i)
    {
        dst[i] = (value >> (8 * i)) & 0xFF;
    }
}

//
//!\brief Compute initial hash H0
//
std::vector<uint8_t> OPERON::Algos::Hash::Argon2::Core::compute_h0(
    const Params& params,
    const uint8_t* password, size_t password_length,
    const uint8_t* salt, size_t salt_length
)
{
    std::vector<uint8_t> buffer;
    uint8_t* ptr;

    const size_t secret_length = params.secret.size();
    const size_t associated_data_length = params.associated_data.size();

    const size_t total_size =
        4 * 6 +
        4 + password_length +
        4 + salt_length +
        4 + secret_length +
        4 + associated_data_length;

    buffer.resize(total_size);
    ptr = buffer.data();

    store32(ptr, params.parallelism); ptr += 4;
    store32(ptr, params.hash_length); ptr += 4;
    store32(ptr, params.memory_cost_kib); ptr += 4;
    store32(ptr, params.time_cost); ptr += 4;
    store32(ptr, params.version); ptr += 4;
    store32(ptr, static_cast<uint32_t>(params.variant)); ptr += 4;

    store32(ptr, static_cast<uint32_t>(password_length)); ptr += 4;
    if (password_length > 0)
    {
        std::memcpy(ptr, password, password_length);
        ptr += password_length;
    }

    store32(ptr, static_cast<uint32_t>(salt_length)); ptr += 4;
    if (salt_length > 0)
    {
        std::memcpy(ptr, salt, salt_length);
        ptr += salt_length;
    }

    store32(ptr, static_cast<uint32_t>(secret_length)); ptr += 4;
    if (secret_length > 0)
    {
        std::memcpy(ptr, params.secret.data(), secret_length);
        ptr += secret_length;
    }

    store32(ptr, static_cast<uint32_t>(associated_data_length)); ptr += 4;
    if (associated_data_length > 0)
    {
        std::memcpy(ptr, params.associated_data.data(), associated_data_length);
        ptr += associated_data_length;
    }

    return OPERON::Algos::Hash::Blake2b::Core::hash(
        buffer.data(),
        buffer.size(),
        64
    );
}

//
//!\brief Variable-length BLAKE2b hash (H') - RFC 9106 compliant
//
std::vector<uint8_t> OPERON::Algos::Hash::Argon2::Core::blake2b_long(
    const uint8_t* input,
    size_t input_length,
    size_t output_length
)
{
    std::vector<uint8_t> out;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> V;

    // buffer = LE32(T) || input
    buffer.resize(4 + input_length);
    store32(buffer.data(), static_cast<uint32_t>(output_length));
    std::memcpy(buffer.data() + 4, input, input_length);

    // Case 1: short output
    if (output_length <= 64)
    {
        return OPERON::Algos::Hash::Blake2b::Core::hash(
            buffer.data(),
            buffer.size(),
            output_length
        );
    }

    // Case 2: long output
    out.resize(output_length);

    // V1
    V = OPERON::Algos::Hash::Blake2b::Core::hash(
        buffer.data(),
        buffer.size(),
        64
    );

    size_t offset = 0;

    // copy first 32 bytes of V1
    std::memcpy(out.data(), V.data(), 32);
    offset += 32;

    // subsequent blocks
    while (offset + 32 <= output_length)
    {
        V = OPERON::Algos::Hash::Blake2b::Core::hash(
            V.data(),
            V.size(),
            64
        );

        std::memcpy(out.data() + offset, V.data(), 32);
        offset += 32;
    }

    // final block
    const size_t remaining = output_length - offset;

    if (remaining > 0)
    {
        V = OPERON::Algos::Hash::Blake2b::Core::hash(
            V.data(),
            V.size(),
            remaining
        );

        std::memcpy(out.data() + offset, V.data(), remaining);
    }

    return out;
}