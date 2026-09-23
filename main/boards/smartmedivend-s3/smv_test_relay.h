#pragma once

#include <esp_log.h>

namespace smv {
// BENCH-ONLY: this archive contains no relay GPIO/driver/polarity/power-off
// mapping. Never energize a medication dispensing actuator from STT alone.
inline void ReportTestRelay(unsigned relay) {
    ESP_LOGI("SMV-Relay-Bench", "SIMULATED C%u; physical relay OFF / not configured", relay);
}
}  // namespace smv
