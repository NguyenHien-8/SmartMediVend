#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace smv {
// The speaker is fixed at 24 kHz PCM. The local MP3 source may be either
// native 24 kHz mono (MPEG-2) or 44.1 kHz mono (MPEG-1); the latter is
// resampled to 24 kHz after decoding. Inspect the first actual MP3 frame,
// skipping an optional ID3v2 tag. Return 0 for an unsupported/truncated file.
inline uint32_t Mp3MonoSampleRate(std::string_view file, size_t* first_frame_offset = nullptr) {
    const auto* p = reinterpret_cast<const uint8_t*>(file.data());
    const size_t size = file.size();
    size_t offset = 0;
    if (size >= 10 && p[0] == 'I' && p[1] == 'D' && p[2] == '3') {
        for (unsigned j = 6; j < 10; ++j) {
            if (p[j] & 0x80) return 0;  // Invalid syncsafe size.
        }
        const size_t tag_size = (size_t(p[6]) << 21) | (size_t(p[7]) << 14) |
                                (size_t(p[8]) << 7) | size_t(p[9]);
        const size_t footer_size = (p[5] & 0x10) ? 10u : 0u;
        if (tag_size > size - 10 || footer_size > size - 10 - tag_size) return 0;
        offset = 10 + tag_size + footer_size;
    }
    const size_t limit = offset + std::min(size - offset, size_t{512});
    for (size_t i = offset; i + 4 <= limit; ++i) {
        if (p[i] != 0xff || (p[i + 1] & 0xe0) != 0xe0) continue;
        const uint8_t version = p[i + 1] & 0x18;
        const uint8_t layer = p[i + 1] & 0x06;
        const uint8_t rate_index = (p[i + 2] >> 2) & 3;
        const uint8_t bitrate_index = (p[i + 2] >> 4) & 15;
        if (layer != 0x02 || rate_index == 3 ||
            bitrate_index == 0 || bitrate_index == 15 ||
            (p[i + 3] & 0xc0) != 0xc0) continue;
        if (version == 0x10 && rate_index == 1) {
            if (first_frame_offset) *first_frame_offset = i;
            return 24000;
        }
        if (version == 0x18 && rate_index == 0) {
            if (first_frame_offset) *first_frame_offset = i;
            return 44100;
        }
    }
    return 0;
}

// Kept for the existing host tests and compatibility with earlier callers.
inline bool Mp3Is24kMono(std::string_view file) {
    return Mp3MonoSampleRate(file) == 24000;
}
}  // namespace smv
