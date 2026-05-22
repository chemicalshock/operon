// -------------------------------------------------------------
//
//!\file test_aead_chacha20poly1305.cpp
//!\brief Tests for ChaCha20-Poly1305 algorithm behaviour
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// shocktest
#include "shocktest/shocktest.hpp"

// system
#include <cctype>
#include <stdexcept>
#include <string>

// SUT
#include "algos/aead/chacha20poly1305/chacha20poly1305.hpp"

// internal
#include "utils.hpp"

namespace {

    OPERON::Result run_encrypt(const std::vector<OPERON::Value>& args)
    {
        OPERON::Context context;
        const OPERON::Algorithm algorithm = OPERON::Algos::Aead::ChaCha20Poly1305::make_encrypt_algorithm();

        return algorithm.function(context, args);
    }

    OPERON::Result run_decrypt(const std::vector<OPERON::Value>& args)
    {
        OPERON::Context context;
        const OPERON::Algorithm algorithm = OPERON::Algos::Aead::ChaCha20Poly1305::make_decrypt_algorithm();

        return algorithm.function(context, args);
    }

    uint8_t hex_nibble(char ch)
    {
        if (ch >= '0' && ch <= '9')
        {
            return static_cast<uint8_t>(ch - '0');
        }

        if (ch >= 'a' && ch <= 'f')
        {
            return static_cast<uint8_t>(10 + (ch - 'a'));
        }

        if (ch >= 'A' && ch <= 'F')
        {
            return static_cast<uint8_t>(10 + (ch - 'A'));
        }

        throw std::runtime_error("Invalid hex character");
    }

    OPERON::Value::Buffer hex_to_buffer(const std::string& hex)
    {
        OPERON::Value::Buffer buffer;

        if ((hex.size() % 2U) != 0U)
        {
            throw std::runtime_error("Hex input must have an even length");
        }

        buffer.reserve(hex.size() / 2U);

        for (size_t i = 0; i < hex.size(); i += 2U)
        {
            if (!std::isxdigit(static_cast<unsigned char>(hex[i])) ||
                !std::isxdigit(static_cast<unsigned char>(hex[i + 1U])))
            {
                throw std::runtime_error("Hex input contains a non-hex character");
            }

            buffer.push_back(static_cast<uint8_t>((hex_nibble(hex[i]) << 4U) | hex_nibble(hex[i + 1U])));
        }

        return buffer;
    }

} // namespace

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_encrypt_matches_rfc8439_vector)
{
    const OPERON::Value::Buffer plaintext = hex_to_buffer(
        "4c616469657320616e642047656e746c656d656e206f662074686520636c617373206f66202739393a204966204920636f756c64206f6666657220796f75206f6e6c79206f6e652074697020666f7220746865206675747572652c2073756e73637265656e20776f756c642062652069742e"
    );
    const OPERON::Value::Buffer key = hex_to_buffer(
        "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"
    );
    const OPERON::Value::Buffer nonce = hex_to_buffer(
        "070000004041424344454647"
    );
    const OPERON::Value::Buffer aad = hex_to_buffer(
        "50515253c0c1c2c3c4c5c6c7"
    );
    const std::string expected_hex =
        "d31a8d34648e60db7b86afbc53ef7ec2"
        "a4aded51296e08fea9e2b5a736ee62d6"
        "3dbea45e8ca9671282fafb69da92728b"
        "1a71de0a9e060b2905d6a5b67ecd3b36"
        "92ddbd7f2d778b8c9803aee328091b58"
        "fab324e4fad675945585808b4831d7bc"
        "3ff4def08e4b7a9de576d26586cec64b"
        "61161ae10b594f09e26a7e902ecbd0600691";

    const OPERON::Result result = run_encrypt({ plaintext, key, nonce, aad });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());
    EXPECT_EQ(TEST_UTILS::buffer_to_hex(result.value.as_buffer()), expected_hex);
}

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_decrypt_matches_rfc8439_vector)
{
    const OPERON::Value::Buffer ciphertext_and_tag = hex_to_buffer(
        "d31a8d34648e60db7b86afbc53ef7ec2"
        "a4aded51296e08fea9e2b5a736ee62d6"
        "3dbea45e8ca9671282fafb69da92728b"
        "1a71de0a9e060b2905d6a5b67ecd3b36"
        "92ddbd7f2d778b8c9803aee328091b58"
        "fab324e4fad675945585808b4831d7bc"
        "3ff4def08e4b7a9de576d26586cec64b"
        "61161ae10b594f09e26a7e902ecbd0600691"
    );
    const OPERON::Value::Buffer key = hex_to_buffer(
        "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"
    );
    const OPERON::Value::Buffer nonce = hex_to_buffer(
        "070000004041424344454647"
    );
    const OPERON::Value::Buffer aad = hex_to_buffer(
        "50515253c0c1c2c3c4c5c6c7"
    );
    const std::string expected_hex =
        "4c616469657320616e642047656e746c656d656e206f662074686520636c617373206f66202739393a204966204920636f756c64206f6666657220796f75206f6e6c79206f6e652074697020666f7220746865206675747572652c2073756e73637265656e20776f756c642062652069742e";

    const OPERON::Result result = run_decrypt({ ciphertext_and_tag, key, nonce, aad });

    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.value.is_buffer());
    EXPECT_EQ(TEST_UTILS::buffer_to_hex(result.value.as_buffer()), expected_hex);
}

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_decrypt_rejects_invalid_tag)
{
    OPERON::Value::Buffer ciphertext_and_tag = hex_to_buffer(
        "d31a8d34648e60db7b86afbc53ef7ec2"
        "a4aded51296e08fea9e2b5a736ee62d6"
        "3dbea45e8ca9671282fafb69da92728b"
        "1a71de0a9e060b2905d6a5b67ecd3b36"
        "92ddbd7f2d778b8c9803aee328091b58"
        "fab324e4fad675945585808b4831d7bc"
        "3ff4def08e4b7a9de576d26586cec64b"
        "61161ae10b594f09e26a7e902ecbd0600691"
    );
    const OPERON::Value::Buffer key = hex_to_buffer(
        "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"
    );
    const OPERON::Value::Buffer nonce = hex_to_buffer(
        "070000004041424344454647"
    );
    const OPERON::Value::Buffer aad = hex_to_buffer(
        "50515253c0c1c2c3c4c5c6c7"
    );

    ciphertext_and_tag.back() ^= 0x01U;

    const OPERON::Result result = run_decrypt({ ciphertext_and_tag, key, nonce, aad });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E005_AUTH_FAILED"));
}

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_encrypt_and_decrypt_allow_omitted_aad)
{
    const OPERON::Value::Buffer plaintext = { 0x00, 0x01, 0x02, 0x03, 0xfe, 0xff };
    const OPERON::Value::Buffer key = hex_to_buffer(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
    );
    const OPERON::Value::Buffer nonce = hex_to_buffer(
        "000000000000004a00000000"
    );

    const OPERON::Result encrypted = run_encrypt({ plaintext, key, nonce });

    EXPECT_TRUE(encrypted.ok);
    EXPECT_TRUE(encrypted.value.is_buffer());

    const OPERON::Result decrypted = run_decrypt({ encrypted.value.as_buffer(), key, nonce });

    EXPECT_TRUE(decrypted.ok);
    EXPECT_TRUE(decrypted.value.is_buffer());
    EXPECT_EQ(
        TEST_UTILS::buffer_to_hex(decrypted.value.as_buffer()),
        TEST_UTILS::buffer_to_hex(plaintext)
    );
}

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_encrypt_and_decrypt_hello_world)
{
    const std::string plaintext = "Hello World";
    const OPERON::Value::Buffer plaintext_buffer(plaintext.begin(), plaintext.end());
    const OPERON::Value::Buffer key = hex_to_buffer(
        "1f1e1d1c1b1a191817161514131211100f0e0d0c0b0a09080706050403020100"
    );
    const OPERON::Value::Buffer nonce = hex_to_buffer(
        "000102030405060708090a0b"
    );
    const OPERON::Value::Buffer aad = {
        0x6d, 0x65, 0x74, 0x61
    };

    const OPERON::Result encrypted = run_encrypt({ plaintext_buffer, key, nonce, aad });

    EXPECT_TRUE(encrypted.ok);
    EXPECT_TRUE(encrypted.value.is_buffer());

    const OPERON::Result decrypted = run_decrypt({ encrypted.value.as_buffer(), key, nonce, aad });

    EXPECT_TRUE(decrypted.ok);
    EXPECT_TRUE(decrypted.value.is_buffer());
    EXPECT_EQ(
        std::string(decrypted.value.as_buffer().begin(), decrypted.value.as_buffer().end()),
        plaintext
    );
}

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_encrypt_rejects_missing_arguments)
{
    const OPERON::Result result = run_encrypt({});

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E001_MISSING_ARGUMENT"));
}

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_encrypt_rejects_invalid_types)
{
    const OPERON::Result result = run_encrypt({ "plaintext", "key", "nonce" });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E002_INVALID_TYPE"));
}

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_encrypt_rejects_invalid_key_length)
{
    const OPERON::Value::Buffer plaintext = { 0x01, 0x02, 0x03 };
    const OPERON::Value::Buffer key = { 0x00, 0x01, 0x02 };
    const OPERON::Value::Buffer nonce = hex_to_buffer("000000000000004a00000000");

    const OPERON::Result result = run_encrypt({ plaintext, key, nonce });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E003_INVALID_VALUE"));
}

SHOCKTEST_GOODWEATHER(aead_chacha20poly1305_decrypt_rejects_short_ciphertext_and_tag)
{
    const OPERON::Value::Buffer ciphertext_and_tag = { 0x01, 0x02, 0x03 };
    const OPERON::Value::Buffer key = hex_to_buffer(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
    );
    const OPERON::Value::Buffer nonce = hex_to_buffer("000000000000004a00000000");

    const OPERON::Result result = run_decrypt({ ciphertext_and_tag, key, nonce });

    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.error_code, std::string("E003_INVALID_VALUE"));
}
