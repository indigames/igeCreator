#pragma once

#include <cstdint>
#include <type_traits>
#include <fstream>

namespace crc32
{
    constexpr uint32_t crc32(uint32_t iv, unsigned char* buf, uint32_t len);
    uint32_t crc32_from_file(const char* path);
}

// Macro to be used, guarantees hash is computed at compile time.
#define CRC32_CONSTEXPR(A) std::integral_constant<uint32_t, crc32::crc32(0, A, sizeof(A) - 1)>::value
#define CRC32(A) crc32::crc32(0, A, sizeof(A) - 1)
