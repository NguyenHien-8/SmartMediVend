#pragma once
#include <cstddef>
#include <string_view>

namespace smv {
enum class LocalVoiceFormat { OggOpus, Mp3 };
struct LocalVoice {
    std::string_view bytes;
    LocalVoiceFormat format = LocalVoiceFormat::OggOpus;
};

#if defined(SMV_LOCAL_MP3_01)
extern const char smv_01_start[] asm("_binary_smv_01_mp3_start");
extern const char smv_01_end[] asm("_binary_smv_01_mp3_end");
inline LocalVoice Sound01() {
    return {{smv_01_start, static_cast<size_t>(smv_01_end - smv_01_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_01_start[] asm("_binary_smv_01_ogg_start");
extern const char smv_01_end[] asm("_binary_smv_01_ogg_end");
inline LocalVoice Sound01() {
    return {{smv_01_start, static_cast<size_t>(smv_01_end - smv_01_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_02)
extern const char smv_02_start[] asm("_binary_smv_02_mp3_start");
extern const char smv_02_end[] asm("_binary_smv_02_mp3_end");
inline LocalVoice Sound02() {
    return {{smv_02_start, static_cast<size_t>(smv_02_end - smv_02_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_02_start[] asm("_binary_smv_02_ogg_start");
extern const char smv_02_end[] asm("_binary_smv_02_ogg_end");
inline LocalVoice Sound02() {
    return {{smv_02_start, static_cast<size_t>(smv_02_end - smv_02_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_03)
extern const char smv_03_start[] asm("_binary_smv_03_mp3_start");
extern const char smv_03_end[] asm("_binary_smv_03_mp3_end");
inline LocalVoice Sound03() {
    return {{smv_03_start, static_cast<size_t>(smv_03_end - smv_03_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_03_start[] asm("_binary_smv_03_ogg_start");
extern const char smv_03_end[] asm("_binary_smv_03_ogg_end");
inline LocalVoice Sound03() {
    return {{smv_03_start, static_cast<size_t>(smv_03_end - smv_03_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_04)
extern const char smv_04_start[] asm("_binary_smv_04_mp3_start");
extern const char smv_04_end[] asm("_binary_smv_04_mp3_end");
inline LocalVoice Sound04() {
    return {{smv_04_start, static_cast<size_t>(smv_04_end - smv_04_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_04_start[] asm("_binary_smv_04_ogg_start");
extern const char smv_04_end[] asm("_binary_smv_04_ogg_end");
inline LocalVoice Sound04() {
    return {{smv_04_start, static_cast<size_t>(smv_04_end - smv_04_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_05)
extern const char smv_05_start[] asm("_binary_smv_05_mp3_start");
extern const char smv_05_end[] asm("_binary_smv_05_mp3_end");
inline LocalVoice Sound05() {
    return {{smv_05_start, static_cast<size_t>(smv_05_end - smv_05_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_05_start[] asm("_binary_smv_05_ogg_start");
extern const char smv_05_end[] asm("_binary_smv_05_ogg_end");
inline LocalVoice Sound05() {
    return {{smv_05_start, static_cast<size_t>(smv_05_end - smv_05_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_06)
extern const char smv_06_start[] asm("_binary_smv_06_mp3_start");
extern const char smv_06_end[] asm("_binary_smv_06_mp3_end");
inline LocalVoice Sound06() {
    return {{smv_06_start, static_cast<size_t>(smv_06_end - smv_06_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_06_start[] asm("_binary_smv_06_ogg_start");
extern const char smv_06_end[] asm("_binary_smv_06_ogg_end");
inline LocalVoice Sound06() {
    return {{smv_06_start, static_cast<size_t>(smv_06_end - smv_06_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_07)
extern const char smv_07_start[] asm("_binary_smv_07_mp3_start");
extern const char smv_07_end[] asm("_binary_smv_07_mp3_end");
inline LocalVoice Sound07() {
    return {{smv_07_start, static_cast<size_t>(smv_07_end - smv_07_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_07_start[] asm("_binary_smv_07_ogg_start");
extern const char smv_07_end[] asm("_binary_smv_07_ogg_end");
inline LocalVoice Sound07() {
    return {{smv_07_start, static_cast<size_t>(smv_07_end - smv_07_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_08)
extern const char smv_08_start[] asm("_binary_smv_08_mp3_start");
extern const char smv_08_end[] asm("_binary_smv_08_mp3_end");
inline LocalVoice Sound08() {
    return {{smv_08_start, static_cast<size_t>(smv_08_end - smv_08_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_08_start[] asm("_binary_smv_08_ogg_start");
extern const char smv_08_end[] asm("_binary_smv_08_ogg_end");
inline LocalVoice Sound08() {
    return {{smv_08_start, static_cast<size_t>(smv_08_end - smv_08_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_09)
extern const char smv_09_start[] asm("_binary_smv_09_mp3_start");
extern const char smv_09_end[] asm("_binary_smv_09_mp3_end");
inline LocalVoice Sound09() {
    return {{smv_09_start, static_cast<size_t>(smv_09_end - smv_09_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_09_start[] asm("_binary_smv_09_ogg_start");
extern const char smv_09_end[] asm("_binary_smv_09_ogg_end");
inline LocalVoice Sound09() {
    return {{smv_09_start, static_cast<size_t>(smv_09_end - smv_09_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_10)
extern const char smv_10_start[] asm("_binary_smv_10_mp3_start");
extern const char smv_10_end[] asm("_binary_smv_10_mp3_end");
inline LocalVoice Sound10() {
    return {{smv_10_start, static_cast<size_t>(smv_10_end - smv_10_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_10_start[] asm("_binary_smv_10_ogg_start");
extern const char smv_10_end[] asm("_binary_smv_10_ogg_end");
inline LocalVoice Sound10() {
    return {{smv_10_start, static_cast<size_t>(smv_10_end - smv_10_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_11)
extern const char smv_11_start[] asm("_binary_smv_11_mp3_start");
extern const char smv_11_end[] asm("_binary_smv_11_mp3_end");
inline LocalVoice Sound11() {
    return {{smv_11_start, static_cast<size_t>(smv_11_end - smv_11_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_11_start[] asm("_binary_smv_11_ogg_start");
extern const char smv_11_end[] asm("_binary_smv_11_ogg_end");
inline LocalVoice Sound11() {
    return {{smv_11_start, static_cast<size_t>(smv_11_end - smv_11_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_12)
extern const char smv_12_start[] asm("_binary_smv_12_mp3_start");
extern const char smv_12_end[] asm("_binary_smv_12_mp3_end");
inline LocalVoice Sound12() {
    return {{smv_12_start, static_cast<size_t>(smv_12_end - smv_12_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_12_start[] asm("_binary_smv_12_ogg_start");
extern const char smv_12_end[] asm("_binary_smv_12_ogg_end");
inline LocalVoice Sound12() {
    return {{smv_12_start, static_cast<size_t>(smv_12_end - smv_12_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_13)
extern const char smv_13_start[] asm("_binary_smv_13_mp3_start");
extern const char smv_13_end[] asm("_binary_smv_13_mp3_end");
inline LocalVoice Sound13() {
    return {{smv_13_start, static_cast<size_t>(smv_13_end - smv_13_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_13_start[] asm("_binary_smv_13_ogg_start");
extern const char smv_13_end[] asm("_binary_smv_13_ogg_end");
inline LocalVoice Sound13() {
    return {{smv_13_start, static_cast<size_t>(smv_13_end - smv_13_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_14)
extern const char smv_14_start[] asm("_binary_smv_14_mp3_start");
extern const char smv_14_end[] asm("_binary_smv_14_mp3_end");
inline LocalVoice Sound14() {
    return {{smv_14_start, static_cast<size_t>(smv_14_end - smv_14_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_14_start[] asm("_binary_smv_14_ogg_start");
extern const char smv_14_end[] asm("_binary_smv_14_ogg_end");
inline LocalVoice Sound14() {
    return {{smv_14_start, static_cast<size_t>(smv_14_end - smv_14_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_15)
extern const char smv_15_start[] asm("_binary_smv_15_mp3_start");
extern const char smv_15_end[] asm("_binary_smv_15_mp3_end");
inline LocalVoice Sound15() {
    return {{smv_15_start, static_cast<size_t>(smv_15_end - smv_15_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_15_start[] asm("_binary_smv_15_ogg_start");
extern const char smv_15_end[] asm("_binary_smv_15_ogg_end");
inline LocalVoice Sound15() {
    return {{smv_15_start, static_cast<size_t>(smv_15_end - smv_15_start)}, LocalVoiceFormat::OggOpus};
}
#endif

#if defined(SMV_LOCAL_MP3_16)
extern const char smv_16_start[] asm("_binary_smv_16_mp3_start");
extern const char smv_16_end[] asm("_binary_smv_16_mp3_end");
inline LocalVoice Sound16() {
    return {{smv_16_start, static_cast<size_t>(smv_16_end - smv_16_start)}, LocalVoiceFormat::Mp3};
}

#else
extern const char smv_16_start[] asm("_binary_smv_16_ogg_start");
extern const char smv_16_end[] asm("_binary_smv_16_ogg_end");
inline LocalVoice Sound16() {
    return {{smv_16_start, static_cast<size_t>(smv_16_end - smv_16_start)}, LocalVoiceFormat::OggOpus};
}
#endif

inline LocalVoice LocalSound(unsigned id) {
    switch (id) {
        case 1: return Sound01();
        case 2: return Sound02();
        case 3: return Sound03();
        case 4: return Sound04();
        case 5: return Sound05();
        case 6: return Sound06();
        case 7: return Sound07();
        case 8: return Sound08();
        case 9: return Sound09();
        case 10: return Sound10();
        case 11: return Sound11();
        case 12: return Sound12();
        case 13: return Sound13();
        case 14: return Sound14();
        case 15: return Sound15();
        case 16: return Sound16();
        default: return {};
    }
}
} // namespace smv
