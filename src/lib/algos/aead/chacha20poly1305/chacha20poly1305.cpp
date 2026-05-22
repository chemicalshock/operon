// -------------------------------------------------------------
//
//!\file chacha20poly1305.cpp
//!\brief ChaCha20-Poly1305 algorithm implementations for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "algos/aead/chacha20poly1305/chacha20poly1305.hpp"

// system
#include <exception>

// module
#include "algos/aead/chacha20poly1305/core.hpp"

namespace {

    OPERON::Result validate_key_and_nonce(
        const OPERON::Value::Buffer& key,
        const OPERON::Value::Buffer& nonce
    )
    {
        if (key.size() != OPERON::Algos::Aead::ChaCha20Poly1305::Core::KEY_SIZE)
        {
            return OPERON::Result::error("E003_INVALID_VALUE", "key must be 32 bytes");
        }

        if (nonce.size() != OPERON::Algos::Aead::ChaCha20Poly1305::Core::NONCE_SIZE)
        {
            return OPERON::Result::error("E003_INVALID_VALUE", "nonce must be 12 bytes");
        }

        return OPERON::Result::success(OPERON::Value());
    }

} // namespace

OPERON::Algorithm OPERON::Algos::Aead::ChaCha20Poly1305::make_encrypt_algorithm()
{
    OPERON::Algorithm algorithm;

    algorithm.info.name = "aead.chacha20poly1305_encrypt";
    algorithm.info.category = "aead";
    algorithm.info.brief = "Encrypt a buffer using ChaCha20-Poly1305 and append the tag";
    algorithm.info.parameters = {
        { "plaintext", "Plaintext buffer", OPERON::ValueType::Buffer, true },
        { "key", "32-byte encryption key", OPERON::ValueType::Buffer, true },
        { "nonce", "12-byte nonce", OPERON::ValueType::Buffer, true },
        { "aad", "Additional authenticated data buffer", OPERON::ValueType::Buffer, false }
    };
    algorithm.info.return_type = OPERON::ValueType::Buffer;

    algorithm.function = [](OPERON::Context&, const std::vector<OPERON::Value>& args) -> OPERON::Result
    {
        if (args.size() < 3)
        {
            return OPERON::Result::error(
                "E001_MISSING_ARGUMENT",
                "aead.chacha20poly1305_encrypt requires 3 arguments: plaintext, key, nonce"
            );
        }

        if (!args[0].is_buffer())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "plaintext must be a buffer");
        }

        if (!args[1].is_buffer())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "key must be a buffer");
        }

        if (!args[2].is_buffer())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "nonce must be a buffer");
        }

        if (args.size() >= 4 && !args[3].is_buffer())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "aad must be a buffer");
        }

        const OPERON::Value::Buffer empty_aad;
        const OPERON::Value::Buffer& plaintext = args[0].as_buffer();
        const OPERON::Value::Buffer& key = args[1].as_buffer();
        const OPERON::Value::Buffer& nonce = args[2].as_buffer();
        const OPERON::Value::Buffer& aad = args.size() >= 4 ? args[3].as_buffer() : empty_aad;
        const OPERON::Result validation = validate_key_and_nonce(key, nonce);

        if (!validation.ok)
        {
            return validation;
        }

        try
        {
            const OPERON::Value::Buffer ciphertext_and_tag = Core::seal(
                plaintext.data(),
                plaintext.size(),
                aad.data(),
                aad.size(),
                key.data(),
                key.size(),
                nonce.data(),
                nonce.size()
            );

            return OPERON::Result::success(OPERON::Value(ciphertext_and_tag));
        }
        catch (const std::exception& ex)
        {
            return OPERON::Result::error("E004_CHACHA20POLY1305_FAILED", ex.what());
        }
        catch (...)
        {
            return OPERON::Result::error("E004_CHACHA20POLY1305_FAILED", "Unknown ChaCha20-Poly1305 failure");
        }
    };

    return algorithm;
}

OPERON::Algorithm OPERON::Algos::Aead::ChaCha20Poly1305::make_decrypt_algorithm()
{
    OPERON::Algorithm algorithm;

    algorithm.info.name = "aead.chacha20poly1305_decrypt";
    algorithm.info.category = "aead";
    algorithm.info.brief = "Verify and decrypt a ChaCha20-Poly1305 ciphertext buffer";
    algorithm.info.parameters = {
        { "ciphertext_and_tag", "Ciphertext with 16-byte authentication tag appended", OPERON::ValueType::Buffer, true },
        { "key", "32-byte encryption key", OPERON::ValueType::Buffer, true },
        { "nonce", "12-byte nonce", OPERON::ValueType::Buffer, true },
        { "aad", "Additional authenticated data buffer", OPERON::ValueType::Buffer, false }
    };
    algorithm.info.return_type = OPERON::ValueType::Buffer;

    algorithm.function = [](OPERON::Context&, const std::vector<OPERON::Value>& args) -> OPERON::Result
    {
        if (args.size() < 3)
        {
            return OPERON::Result::error(
                "E001_MISSING_ARGUMENT",
                "aead.chacha20poly1305_decrypt requires 3 arguments: ciphertext_and_tag, key, nonce"
            );
        }

        if (!args[0].is_buffer())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "ciphertext_and_tag must be a buffer");
        }

        if (!args[1].is_buffer())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "key must be a buffer");
        }

        if (!args[2].is_buffer())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "nonce must be a buffer");
        }

        if (args.size() >= 4 && !args[3].is_buffer())
        {
            return OPERON::Result::error("E002_INVALID_TYPE", "aad must be a buffer");
        }

        const OPERON::Value::Buffer empty_aad;
        const OPERON::Value::Buffer& ciphertext_and_tag = args[0].as_buffer();
        const OPERON::Value::Buffer& key = args[1].as_buffer();
        const OPERON::Value::Buffer& nonce = args[2].as_buffer();
        const OPERON::Value::Buffer& aad = args.size() >= 4 ? args[3].as_buffer() : empty_aad;
        const OPERON::Result validation = validate_key_and_nonce(key, nonce);

        if (!validation.ok)
        {
            return validation;
        }

        if (ciphertext_and_tag.size() < Core::TAG_SIZE)
        {
            return OPERON::Result::error("E003_INVALID_VALUE", "ciphertext_and_tag must be at least 16 bytes");
        }

        try
        {
            OPERON::Value::Buffer plaintext;
            const bool ok = Core::open(
                ciphertext_and_tag.data(),
                ciphertext_and_tag.size(),
                aad.data(),
                aad.size(),
                key.data(),
                key.size(),
                nonce.data(),
                nonce.size(),
                plaintext
            );

            if (!ok)
            {
                return OPERON::Result::error("E005_AUTH_FAILED", "Authentication tag verification failed");
            }

            return OPERON::Result::success(OPERON::Value(plaintext));
        }
        catch (const std::exception& ex)
        {
            return OPERON::Result::error("E004_CHACHA20POLY1305_FAILED", ex.what());
        }
        catch (...)
        {
            return OPERON::Result::error("E004_CHACHA20POLY1305_FAILED", "Unknown ChaCha20-Poly1305 failure");
        }
    };

    return algorithm;
}
