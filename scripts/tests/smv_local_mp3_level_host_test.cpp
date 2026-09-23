#include "../../main/boards/smartmedivend-s3/smv_local_mp3_level.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

int main() {
    assert(smv::BoostLocalMp3Sample(0) == 0);
    assert(smv::BoostLocalMp3Sample(1000) == 1800);
    assert(smv::BoostLocalMp3Sample(-1000) == -1800);
    // No overflows, no sign inversion and symmetry even for INT16_MIN.
    int32_t last = 0;
    for (int32_t value = 0; value <= 32767; ++value) {
        const int32_t current = smv::BoostLocalMp3Sample(static_cast<int16_t>(value));
        assert(current >= last && current <= 31129);
        assert(smv::BoostLocalMp3Sample(static_cast<int16_t>(-value)) == -current);
        last = current;
    }
    assert(smv::BoostLocalMp3Sample(INT16_MIN) <= 0);
    assert(smv::BoostLocalMp3Sample(INT16_MAX) <= 31129);
    std::vector<int16_t> same(1024, 4000);
    smv::BoostLocalMp3Pcm(same.data(), same.size());
    assert(std::all_of(same.begin(), same.end(), [](int16_t s) { return s == 7200; }));
    // Separate buffers and playback turns cannot accumulate any state.
    std::vector<int16_t> again(1024, 4000);
    smv::BoostLocalMp3Pcm(again.data(), again.size());
    assert(again == same);
    smv::BoostLocalMp3Pcm(nullptr, 0);
    std::puts("PASS: 32768-level sweep, peak ceiling, polarity, steady gain and replay");
}
