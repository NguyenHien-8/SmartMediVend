#pragma once

#include <cstddef>
#include <cstdint>

// SmartMediVend ONLY: raise locally decoded MP3 speech before the shared
// NoAudioCodec applies the user's output-volume setting (70 => ~49% amplitude).
// Fixed, memoryless gain: no envelope follower that can progressively duck
// the clip. The soft knee avoids clipping the already near-full-scale peaks.
// Never apply this to microphone samples, Xiaozhi Opus/TTS or alerts.
namespace smv {
inline int16_t BoostLocalMp3Sample(int16_t sample) {
    constexpr uint32_t kGainNum = 9;       // +5.1 dB for quiet speech
    constexpr uint32_t kGainDen = 5;
    constexpr uint32_t kKnee = 22000;
    constexpr uint32_t kCeiling = 31129;   // below 0 dBFS; MAX98357A headroom
    constexpr uint32_t kHeadroom = kCeiling - kKnee;

    const uint32_t magnitude = sample < 0 ? static_cast<uint32_t>(-static_cast<int32_t>(sample))
                                          : static_cast<uint32_t>(sample);
    const uint32_t boosted = magnitude * kGainNum / kGainDen;
    const uint32_t limited = boosted <= kKnee
        ? boosted
        : kKnee + static_cast<uint32_t>((static_cast<uint64_t>(boosted - kKnee) * kHeadroom)
                                         / (boosted - kKnee + kHeadroom));
    return static_cast<int16_t>(sample < 0 ? -static_cast<int32_t>(limited)
                                          : static_cast<int32_t>(limited));
}

inline void BoostLocalMp3Pcm(int16_t* samples, size_t count) {
    if (!samples) return;
    for (size_t i = 0; i < count; ++i) {
        samples[i] = BoostLocalMp3Sample(samples[i]);
    }
}
} // namespace smv
