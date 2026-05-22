// -------------------------------------------------------------
//
//!\file core.cpp
//!\brief Core ChaCha20-Poly1305 implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "algos/aead/chacha20poly1305/core.hpp"

// system
#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <stdexcept>

// internal
#include "utils.hpp"

namespace {

    constexpr uint32_t CHACHA_CONSTANT_0 = 0x61707865U;
    constexpr uint32_t CHACHA_CONSTANT_1 = 0x3320646eU;
    constexpr uint32_t CHACHA_CONSTANT_2 = 0x79622d32U;
    constexpr uint32_t CHACHA_CONSTANT_3 = 0x6b206574U;

    uint32_t load32_le(const uint8_t* input)
    {
        return static_cast<uint32_t>(input[0])
             | (static_cast<uint32_t>(input[1]) << 8)
             | (static_cast<uint32_t>(input[2]) << 16)
             | (static_cast<uint32_t>(input[3]) << 24);
    }

    void store32_le(uint8_t* output, uint32_t value)
    {
        output[0] = static_cast<uint8_t>(value & 0xffU);
        output[1] = static_cast<uint8_t>((value >> 8) & 0xffU);
        output[2] = static_cast<uint8_t>((value >> 16) & 0xffU);
        output[3] = static_cast<uint8_t>((value >> 24) & 0xffU);
    }

    void store64_le(uint8_t* output, uint64_t value)
    {
        for (size_t i = 0; i < 8; ++i)
        {
            output[i] = static_cast<uint8_t>((value >> (8U * i)) & 0xffU);
        }
    }

    uint32_t rotl32(uint32_t value, uint32_t bits)
    {
        return (value << bits) | (value >> (32U - bits));
    }

    void quarter_round(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d)
    {
        a += b;
        d ^= a;
        d = rotl32(d, 16);

        c += d;
        b ^= c;
        b = rotl32(b, 12);

        a += b;
        d ^= a;
        d = rotl32(d, 8);

        c += d;
        b ^= c;
        b = rotl32(b, 7);
    }

    std::array<uint8_t, 64> chacha20_block(
        const uint8_t* key,
        uint32_t counter,
        const uint8_t* nonce
    )
    {
        std::array<uint32_t, 16> state = {
            CHACHA_CONSTANT_0,
            CHACHA_CONSTANT_1,
            CHACHA_CONSTANT_2,
            CHACHA_CONSTANT_3,
            load32_le(key + 0),
            load32_le(key + 4),
            load32_le(key + 8),
            load32_le(key + 12),
            load32_le(key + 16),
            load32_le(key + 20),
            load32_le(key + 24),
            load32_le(key + 28),
            counter,
            load32_le(nonce + 0),
            load32_le(nonce + 4),
            load32_le(nonce + 8)
        };
        std::array<uint32_t, 16> working = state;
        std::array<uint8_t, 64> output;

        for (size_t i = 0; i < 10; ++i)
        {
            quarter_round(working[0], working[4], working[8], working[12]);
            quarter_round(working[1], working[5], working[9], working[13]);
            quarter_round(working[2], working[6], working[10], working[14]);
            quarter_round(working[3], working[7], working[11], working[15]);

            quarter_round(working[0], working[5], working[10], working[15]);
            quarter_round(working[1], working[6], working[11], working[12]);
            quarter_round(working[2], working[7], working[8], working[13]);
            quarter_round(working[3], working[4], working[9], working[14]);
        }

        for (size_t i = 0; i < working.size(); ++i)
        {
            working[i] += state[i];
            store32_le(output.data() + (i * 4U), working[i]);
        }

        return output;
    }

    class Poly1305
    {
    public:
        explicit Poly1305(const uint8_t* key)
        {
            std::copy_n(key, m_r.size(), m_r.begin());
            m_r[3] &= 15U;
            m_r[7] &= 15U;
            m_r[11] &= 15U;
            m_r[15] &= 15U;
            m_r[4] &= 252U;
            m_r[8] &= 252U;
            m_r[12] &= 252U;

            std::copy_n(key + 16, m_s.size(), m_s.begin());
        }

        void update(const uint8_t* input, size_t input_length)
        {
            if (input_length == 0)
            {
                return;
            }

            if (m_buffer_used != 0)
            {
                const size_t take = std::min(input_length, m_buffer.size() - m_buffer_used);
                std::memcpy(m_buffer.data() + m_buffer_used, input, take);

                m_buffer_used += take;
                input += take;
                input_length -= take;

                if (m_buffer_used == m_buffer.size())
                {
                    process_block(m_buffer.data(), m_buffer.size());
                    m_buffer_used = 0;
                }
            }

            while (input_length >= m_buffer.size())
            {
                process_block(input, m_buffer.size());
                input += m_buffer.size();
                input_length -= m_buffer.size();
            }

            if (input_length != 0)
            {
                std::memcpy(m_buffer.data(), input, input_length);
                m_buffer_used = input_length;
            }
        }

        std::array<uint8_t, 16> finish()
        {
            if (m_buffer_used != 0)
            {
                process_block(m_buffer.data(), m_buffer_used);
                m_buffer_used = 0;
            }

            std::array<uint8_t, 16> tag = {};
            uint32_t carry = 0;

            for (size_t i = 0; i < tag.size(); ++i)
            {
                const uint32_t sum = static_cast<uint32_t>(m_acc[i]) + m_s[i] + carry;

                tag[i] = static_cast<uint8_t>(sum & 0xffU);
                carry = sum >> 8U;
            }

            return tag;
        }

    private:
        static constexpr std::array<uint8_t, 17> MODULUS = {
            0xfbU, 0xffU, 0xffU, 0xffU,
            0xffU, 0xffU, 0xffU, 0xffU,
            0xffU, 0xffU, 0xffU, 0xffU,
            0xffU, 0xffU, 0xffU, 0xffU,
            0x03U
        };

        static void normalize(std::array<uint32_t, 40>& value)
        {
            for (size_t i = 0; i < value.size(); ++i)
            {
                const uint32_t carry = value[i] >> 8U;

                value[i] &= 0xffU;

                if (carry == 0 || (i + 1) >= value.size())
                {
                    continue;
                }

                value[i + 1] += carry;
            }
        }

        static bool greater_or_equal_modulus(const std::array<uint32_t, 40>& value)
        {
            for (size_t i = MODULUS.size(); i-- > 0;)
            {
                if (value[i] > static_cast<uint32_t>(MODULUS[i]))
                {
                    return true;
                }

                if (value[i] < static_cast<uint32_t>(MODULUS[i]))
                {
                    return false;
                }
            }

            return true;
        }

        static void subtract_modulus(std::array<uint32_t, 40>& value)
        {
            uint32_t borrow = 0;

            for (size_t i = 0; i < MODULUS.size(); ++i)
            {
                const uint32_t subtrahend = static_cast<uint32_t>(MODULUS[i]) + borrow;

                if (value[i] >= subtrahend)
                {
                    value[i] -= subtrahend;
                    borrow = 0;
                }
                else
                {
                    value[i] = static_cast<uint32_t>(256U + value[i] - subtrahend);
                    borrow = 1;
                }
            }
        }

        static void reduce(std::array<uint32_t, 40>& value)
        {
            normalize(value);

            for (;;)
            {
                bool has_high_bytes = false;

                for (size_t i = MODULUS.size(); i < value.size(); ++i)
                {
                    if (value[i] != 0)
                    {
                        has_high_bytes = true;
                        break;
                    }
                }

                if (!has_high_bytes && value[16] < 4U)
                {
                    break;
                }

                std::array<uint32_t, 24> high = {};

                for (size_t i = 0; i < high.size(); ++i)
                {
                    const uint32_t lower = value[16 + i];
                    const uint32_t upper = (16 + i + 1U < value.size()) ? value[17 + i] : 0U;

                    high[i] = ((lower >> 2U) | ((upper & 0x03U) << 6U)) & 0xffU;
                }

                value[16] &= 0x03U;

                for (size_t i = MODULUS.size(); i < value.size(); ++i)
                {
                    value[i] = 0;
                }

                uint32_t carry = 0;

                for (size_t i = 0; i < high.size(); ++i)
                {
                    const uint32_t sum = value[i] + (high[i] * 5U) + carry;

                    value[i] = sum & 0xffU;
                    carry = sum >> 8U;
                }

                for (size_t i = high.size(); i < value.size() && carry != 0; ++i)
                {
                    const uint32_t sum = value[i] + carry;

                    value[i] = sum & 0xffU;
                    carry = sum >> 8U;
                }
            }

            while (greater_or_equal_modulus(value))
            {
                subtract_modulus(value);
            }
        }

        void process_block(const uint8_t* block, size_t block_length)
        {
            std::array<uint8_t, 16> padded = {};
            const uint8_t* data = block;
            std::array<uint8_t, 17> block_value = {};
            std::array<uint32_t, 40> product = {};
            uint32_t carry = 0;

            if (block_length != 16)
            {
                std::memcpy(padded.data(), block, block_length);
                padded[block_length] = 1U;
                data = padded.data();
            }
            else
            {
                block_value[16] = 1U;
            }

            std::copy_n(data, 16, block_value.begin());

            for (size_t i = 0; i < m_acc.size(); ++i)
            {
                const uint32_t sum = static_cast<uint32_t>(m_acc[i]) + block_value[i] + carry;

                m_acc[i] = static_cast<uint8_t>(sum & 0xffU);
                carry = sum >> 8U;
            }

            for (size_t i = 0; i < m_acc.size(); ++i)
            {
                for (size_t j = 0; j < m_r.size(); ++j)
                {
                    product[i + j] += static_cast<uint32_t>(m_acc[i]) * m_r[j];
                }
            }

            reduce(product);

            for (size_t i = 0; i < m_acc.size(); ++i)
            {
                m_acc[i] = static_cast<uint8_t>(product[i]);
            }
        }

        std::array<uint8_t, 16> m_r = {};
        std::array<uint8_t, 17> m_acc = {};
        std::array<uint8_t, 16> m_s = {};
        std::array<uint8_t, 16> m_buffer = {};
        size_t m_buffer_used = 0;
    };

    void validate_core_lengths(size_t key_length, size_t nonce_length, size_t plaintext_length)
    {
        if (key_length != OPERON::Algos::Aead::ChaCha20Poly1305::Core::KEY_SIZE)
        {
            throw std::runtime_error("ChaCha20-Poly1305 key must be 32 bytes");
        }

        if (nonce_length != OPERON::Algos::Aead::ChaCha20Poly1305::Core::NONCE_SIZE)
        {
            throw std::runtime_error("ChaCha20-Poly1305 nonce must be 12 bytes");
        }

        const uint64_t max_length = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) * 64ULL;

        if (static_cast<uint64_t>(plaintext_length) > max_length)
        {
            throw std::runtime_error("ChaCha20-Poly1305 input is too large for a single nonce");
        }
    }

    void chacha20_xor(
        const uint8_t* input,
        size_t input_length,
        uint8_t* output,
        const uint8_t* key,
        const uint8_t* nonce,
        uint32_t initial_counter
    )
    {
        size_t offset = 0;
        uint32_t counter = initial_counter;

        while (offset < input_length)
        {
            const std::array<uint8_t, 64> block = chacha20_block(key, counter, nonce);
            const size_t block_size = std::min<size_t>(64, input_length - offset);

            for (size_t i = 0; i < block_size; ++i)
            {
                output[offset + i] = static_cast<uint8_t>(input[offset + i] ^ block[i]);
            }

            offset += block_size;
            ++counter;
        }
    }

    void poly1305_pad16(Poly1305& poly1305, size_t input_length)
    {
        static constexpr std::array<uint8_t, 16> zeroes = {};
        const size_t remainder = input_length % 16U;

        if (remainder != 0)
        {
            poly1305.update(zeroes.data(), 16U - remainder);
        }
    }

    std::array<uint8_t, 16> compute_aead_tag(
        const uint8_t* ciphertext,
        size_t ciphertext_length,
        const uint8_t* aad,
        size_t aad_length,
        const uint8_t* poly1305_key
    )
    {
        Poly1305 poly1305(poly1305_key);
        std::array<uint8_t, 16> lengths = {};

        poly1305.update(aad, aad_length);
        poly1305_pad16(poly1305, aad_length);

        poly1305.update(ciphertext, ciphertext_length);
        poly1305_pad16(poly1305, ciphertext_length);

        store64_le(lengths.data() + 0, static_cast<uint64_t>(aad_length));
        store64_le(lengths.data() + 8, static_cast<uint64_t>(ciphertext_length));
        poly1305.update(lengths.data(), lengths.size());

        return poly1305.finish();
    }

} // namespace

std::vector<uint8_t> OPERON::Algos::Aead::ChaCha20Poly1305::Core::seal(
    const uint8_t* plaintext,
    size_t plaintext_length,
    const uint8_t* aad,
    size_t aad_length,
    const uint8_t* key,
    size_t key_length,
    const uint8_t* nonce,
    size_t nonce_length
)
{
    validate_core_lengths(key_length, nonce_length, plaintext_length);

    const std::array<uint8_t, 64> first_block = chacha20_block(key, 0U, nonce);
    std::vector<uint8_t> output(plaintext_length + TAG_SIZE);

    chacha20_xor(plaintext, plaintext_length, output.data(), key, nonce, 1U);

    const std::array<uint8_t, 16> tag = compute_aead_tag(
        output.data(),
        plaintext_length,
        aad,
        aad_length,
        first_block.data()
    );

    std::copy(tag.begin(), tag.end(), output.begin() + static_cast<std::ptrdiff_t>(plaintext_length));
    return output;
}

bool OPERON::Algos::Aead::ChaCha20Poly1305::Core::open(
    const uint8_t* ciphertext_and_tag,
    size_t ciphertext_and_tag_length,
    const uint8_t* aad,
    size_t aad_length,
    const uint8_t* key,
    size_t key_length,
    const uint8_t* nonce,
    size_t nonce_length,
    std::vector<uint8_t>& plaintext
)
{
    if (ciphertext_and_tag_length < TAG_SIZE)
    {
        throw std::runtime_error("ChaCha20-Poly1305 ciphertext must include a 16-byte tag");
    }

    const size_t ciphertext_length = ciphertext_and_tag_length - TAG_SIZE;
    validate_core_lengths(key_length, nonce_length, ciphertext_length);

    const uint8_t* ciphertext = ciphertext_and_tag;
    const uint8_t* received_tag = ciphertext_and_tag + ciphertext_length;
    const std::array<uint8_t, 64> first_block = chacha20_block(key, 0U, nonce);
    const std::array<uint8_t, 16> computed_tag = compute_aead_tag(
        ciphertext,
        ciphertext_length,
        aad,
        aad_length,
        first_block.data()
    );

    if (!OPERON::Utils::constant_time_eq(computed_tag.data(), computed_tag.size(), received_tag, TAG_SIZE))
    {
        plaintext.clear();
        return false;
    }

    plaintext.resize(ciphertext_length);
    chacha20_xor(ciphertext, ciphertext_length, plaintext.data(), key, nonce, 1U);
    return true;
}
