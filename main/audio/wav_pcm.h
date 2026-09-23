#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>

// Minimal, allocation-free RIFF/WAVE reader for embedded, little-endian PCM.
// No filesystem, decompression or automatic resampling is involved.
namespace smv {
struct WavPcmData {
    size_t offset = 0;
    size_t size = 0;
};

inline uint16_t WavLe16(const uint8_t* p) {
    return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}
inline uint32_t WavLe32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}
inline bool WavFourCC(const uint8_t* p, const char* id) {
    return std::memcmp(p, id, 4) == 0;
}

// Only 24 kHz / mono / signed 16-bit PCM is accepted: the board's I2S TX rate.
// Reject all other encodings rather than playing bytes at the wrong rate.
inline bool ParseWavPcm24kMono16(std::string_view blob, WavPcmData& result) {
    result = {};
    if (blob.size() < 12) return false;
    const auto* p = reinterpret_cast<const uint8_t*>(blob.data());
    if (!WavFourCC(p, "RIFF") || !WavFourCC(p + 8, "WAVE")) return false;
    const uint32_t riff_size = WavLe32(p + 4);
    if (riff_size < 4 || static_cast<size_t>(riff_size) > blob.size() - 8) return false;
    const size_t end = static_cast<size_t>(riff_size) + 8;
    bool has_format = false;
    bool has_data = false;
    WavPcmData found;
    for (size_t at = 12; at < end;) {
        if (end - at < 8) return false;
        const uint32_t length = WavLe32(p + at + 4);
        const size_t data_at = at + 8;
        if (static_cast<size_t>(length) > end - data_at) return false;
        if (WavFourCC(p + at, "fmt ") && !has_format) {
            if (length < 16) return false;
            const uint8_t* fmt = p + data_at;
            if (WavLe16(fmt) != 1 || WavLe16(fmt + 2) != 1 ||
                WavLe32(fmt + 4) != 24000 || WavLe32(fmt + 8) != 48000 ||
                WavLe16(fmt + 12) != 2 || WavLe16(fmt + 14) != 16) return false;
            has_format = true;
        } else if (WavFourCC(p + at, "data") && !has_data) {
            if (length == 0 || (length & 1U) != 0) return false;
            found = {data_at, static_cast<size_t>(length)};
            has_data = true;
        }
        // RIFF chunks have a pad byte when their data length is odd.
        const size_t padded = static_cast<size_t>(length) + (length & 1U);
        if (padded > end - data_at) return false;
        at = data_at + padded;
    }
    if (!has_format || !has_data) return false;
    result = found;
    return true;
}
}  // namespace smv
