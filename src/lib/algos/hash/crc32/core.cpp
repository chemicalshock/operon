// -------------------------------------------------------------
//
//!\file core.cpp
//!\brief Core CRC32 implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------

// own header
#include "algos/hash/crc32/core.hpp"

// system
#include <array>

namespace
{
    constexpr uint32_t CRC32_POLYNOMIAL = 0xEDB88320U;

    constexpr uint32_t make_crc32_table_entry(uint32_t index)
    {
        uint32_t value = index;

        for (size_t bit = 0; bit < 8; ++bit)
        {
            if ((value & 1U) != 0U)
            {
                value = (value >> 1) ^ CRC32_POLYNOMIAL;
            }
            else
            {
                value >>= 1;
            }
        }

        return value;
    }

    constexpr std::array<uint32_t, 256> make_crc32_table()
    {
        std::array<uint32_t, 256> table = {};

        for (size_t i = 0; i < table.size(); ++i)
        {
            table[i] = make_crc32_table_entry(static_cast<uint32_t>(i));
        }

        return table;
    }

    constexpr std::array<uint32_t, 256> CRC32_TABLE = make_crc32_table();
}

std::vector<uint8_t> OPERON::Algos::Hash::CRC32::Core::hash(const uint8_t* input, size_t length)
{
    uint32_t crc = 0xFFFFFFFFU;

    for (size_t i = 0; i < length; ++i)
    {
        const uint8_t byte = input[i];
        const uint32_t index = (crc ^ byte) & 0xFFU;

        crc = (crc >> 8) ^ CRC32_TABLE[index];
    }

    crc ^= 0xFFFFFFFFU;

    return {
        static_cast<uint8_t>((crc >> 24) & 0xFFU),
        static_cast<uint8_t>((crc >> 16) & 0xFFU),
        static_cast<uint8_t>((crc >> 8) & 0xFFU),
        static_cast<uint8_t>(crc & 0xFFU)
    };
}
