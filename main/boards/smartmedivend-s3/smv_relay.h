#pragma once

#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include <esp_timer.h>

#include <cstdint>
#include <mutex>

#include "BoardPins.h"

namespace smv {
// The board and its off timer live until MCU reset, like Board::GetInstance().
// EN is wired LOW. Unselected MUX outputs need external inactive pull-ups.
class MuxRelay {
public:
    static MuxRelay& GetInstance() {
        static MuxRelay instance;
        return instance;
    }

    esp_err_t Initialize() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (off_timer_)
            return ESP_OK;

        // Load the inactive value BEFORE enabling the output driver. Initialize
        // SIG before address pins so changing the address cannot energize a relay.
        ESP_ERROR_CHECK(gpio_set_level(kSignalPin, 1));
        gpio_config_t signal = {};
        signal.pin_bit_mask = uint64_t{1} << pins::MUX_SIG;
        signal.mode = GPIO_MODE_OUTPUT;
        signal.pull_up_en = GPIO_PULLUP_ENABLE;
        signal.pull_down_en = GPIO_PULLDOWN_DISABLE;
        signal.intr_type = GPIO_INTR_DISABLE;
        ESP_ERROR_CHECK(gpio_config(&signal));

        gpio_config_t address = {};
        for (auto pin : kAddressPins) {
            ESP_ERROR_CHECK(gpio_set_level(pin, 0));
            address.pin_bit_mask |= uint64_t{1} << pin;
        }
        address.mode = GPIO_MODE_OUTPUT;
        address.pull_up_en = GPIO_PULLUP_DISABLE;
        address.pull_down_en = GPIO_PULLDOWN_DISABLE;
        address.intr_type = GPIO_INTR_DISABLE;
        ESP_ERROR_CHECK(gpio_config(&address));

        esp_timer_create_args_t args = {};
        args.callback = [](void* context) { static_cast<MuxRelay*>(context)->TurnOff(); };
        args.arg = this;
        args.dispatch_method = ESP_TIMER_TASK;
        args.name = "smv_relay_off";
        const auto err = esp_timer_create(&args, &off_timer_);
        if (err != ESP_OK) {
            off_timer_ = nullptr;
            return err;
        }
        ESP_LOGI(kTag, "Ready: SIG=GPIO%d HIGH; S0..S3=%d,%d,%d,%d; pulse=500ms LOW", pins::MUX_SIG,
                 pins::MUX_S0, pins::MUX_S1, pins::MUX_S2, pins::MUX_S3);
        return ESP_OK;
    }

    // Called by the application task. Reject overlap; never queue, extend, or
    // interrupt a pulse. The existing STT gate allows only one match per turn.
    bool Pulse(unsigned relay) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!off_timer_ || relay < 1 || relay > 16 || active_relay_ != 0) {
            ESP_LOGW(kTag, "Rejected C%u: ready=%d active=C%u", relay, off_timer_ != nullptr,
                     active_relay_);
            return false;
        }

        const unsigned channel = relay - 1;
        ESP_ERROR_CHECK(gpio_set_level(kSignalPin, 1));
        for (unsigned bit = 0; bit < 4; ++bit) {
            ESP_ERROR_CHECK(gpio_set_level(kAddressPins[bit], (channel >> bit) & 1u));
        }
        // Allow address propagation through the CD74HC4067, not a 500ms busy wait.
        esp_rom_delay_us(2);
        active_relay_ = relay;
        ESP_ERROR_CHECK(gpio_set_level(kSignalPin, 0));
        started_us_ = esp_timer_get_time();
        const auto err = esp_timer_start_once(off_timer_, kPulseUs);
        if (err != ESP_OK) {
            // A failed timer must never leave an actuator energized indefinitely.
            ESP_ERROR_CHECK(gpio_set_level(kSignalPin, 1));
            active_relay_ = 0;
            lock.unlock();
            ESP_LOGE(kTag, "C%u OFF: cannot arm timer: %s", relay, esp_err_to_name(err));
            return false;
        }
        lock.unlock();
        ESP_LOGI(kTag, "C%u ON: MUX C%u, SIG=LOW, pulse=500ms", relay, channel);
        return true;
    }

private:
    MuxRelay() = default;
    MuxRelay(const MuxRelay&) = delete;
    MuxRelay& operator=(const MuxRelay&) = delete;

    static constexpr char kTag[] = "SMV-Relay";
    static constexpr uint64_t kPulseUs = 500000;
    static constexpr gpio_num_t kSignalPin = static_cast<gpio_num_t>(pins::MUX_SIG);
    static constexpr gpio_num_t kAddressPins[] = {
        static_cast<gpio_num_t>(pins::MUX_S0), static_cast<gpio_num_t>(pins::MUX_S1),
        static_cast<gpio_num_t>(pins::MUX_S2), static_cast<gpio_num_t>(pins::MUX_S3)};

    std::mutex mutex_;
    esp_timer_handle_t off_timer_ = nullptr;
    unsigned active_relay_ = 0;
    int64_t started_us_ = 0;

    void TurnOff() {
        std::unique_lock<std::mutex> lock(mutex_);
        ESP_ERROR_CHECK(gpio_set_level(kSignalPin, 1));
        const unsigned relay = active_relay_;
        const auto elapsed_us = esp_timer_get_time() - started_us_;
        active_relay_ = 0;
        lock.unlock();
        // No display, audio or application state changes from the timer task.
        ESP_LOGI(kTag, "C%u OFF: SIG=HIGH, elapsed=%lldus", relay,
                 static_cast<long long>(elapsed_us));
    }
};
}  // namespace smv
