#pragma once
#include <cstdlib>
using esp_err_t = int;
inline constexpr esp_err_t ESP_OK = 0;
inline constexpr esp_err_t ESP_FAIL = -1;
inline constexpr esp_err_t ESP_ERR_INVALID_STATE = 0x103;
inline const char* esp_err_to_name(esp_err_t) { return "injected error"; }
#define ESP_ERROR_CHECK(expr) \
    do {                      \
        if ((expr) != ESP_OK) \
            std::abort();     \
    } while (false)
