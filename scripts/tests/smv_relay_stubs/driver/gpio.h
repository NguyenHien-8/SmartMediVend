#pragma once
#include <cstdint>
#include "esp_err.h"
using gpio_num_t = int;
enum gpio_mode_t { GPIO_MODE_OUTPUT };
enum gpio_pullup_t { GPIO_PULLUP_DISABLE, GPIO_PULLUP_ENABLE };
enum gpio_pulldown_t { GPIO_PULLDOWN_DISABLE, GPIO_PULLDOWN_ENABLE };
enum gpio_int_type_t { GPIO_INTR_DISABLE };
struct gpio_config_t {
    uint64_t pin_bit_mask;
    gpio_mode_t mode;
    gpio_pullup_t pull_up_en;
    gpio_pulldown_t pull_down_en;
    gpio_int_type_t intr_type;
};
esp_err_t gpio_config(const gpio_config_t* config);
esp_err_t gpio_set_level(gpio_num_t pin, uint32_t level);
