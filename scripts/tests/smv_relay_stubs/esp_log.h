#pragma once
template <typename... Args>
inline void RelayTestLog(const char*, const char*, Args...) {}
#define ESP_LOGI(...) RelayTestLog(__VA_ARGS__)
#define ESP_LOGW(...) RelayTestLog(__VA_ARGS__)
#define ESP_LOGE(...) RelayTestLog(__VA_ARGS__)
