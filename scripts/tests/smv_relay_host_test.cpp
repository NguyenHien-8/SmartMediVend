#include <array>
#include <cstdio>
#include <cstdlib>
#include <string_view>
#include <vector>

#include "driver/gpio.h"
#include "esp_timer.h"

#include "smv_relay.h"

struct esp_timer {
    esp_timer_create_args_t args{};
    bool armed = false;
    int64_t deadline = 0;
};

namespace {
std::array<int, 49> levels{};
std::array<bool, 49> outputs{};
struct Write {
    int pin;
    int level;
    int64_t time;
};
std::vector<Write> writes;
esp_timer timer;
int64_t now_us = 0;
int create_count = 0;
int start_count = 0;
bool fail_create = false;
bool fail_start = false;

void Check(bool ok, const char* message) {
    if (!ok) {
        std::fprintf(stderr, "%s\n", message);
        std::exit(1);
    }
}

void Advance(uint64_t us) {
    now_us += us;
    if (timer.armed && now_us >= timer.deadline) {
        timer.armed = false;
        timer.args.callback(timer.args.arg);
    }
}

void Initialize(smv::MuxRelay& relay) {
    Check(relay.Initialize() == ESP_OK, "initialization failed");
    Check(outputs[17] && levels[17] == 1, "SIG must be an output held HIGH at boot");
    for (int pin : {39, 40, 41, 42})
        Check(outputs[pin], "MUX address pin is not an output");
    Check(!timer.armed, "boot must not start a relay pulse");
    Check(relay.Initialize() == ESP_OK && create_count == 1, "initialization leaked a timer");
}
}  // namespace

esp_err_t gpio_config(const gpio_config_t* config) {
    Check(config->mode == GPIO_MODE_OUTPUT, "relay pins must be outputs");
    for (int pin = 0; pin < 49; ++pin) {
        if ((config->pin_bit_mask & (uint64_t{1} << pin)) == 0)
            continue;
        if (pin == 17)
            Check(levels[pin] == 1, "SIG enabled before preloading inactive HIGH");
        outputs[pin] = true;
    }
    return ESP_OK;
}

esp_err_t gpio_set_level(gpio_num_t pin, uint32_t level) {
    Check(pin == 17 || pin == 39 || pin == 40 || pin == 41 || pin == 42,
          "relay driver touched an unrelated pin");
    Check(level <= 1, "invalid GPIO level");
    if (pin != 17 && outputs[17])
        Check(levels[17] == 1, "channel changed while SIG was LOW");
    levels[pin] = static_cast<int>(level);
    writes.push_back({pin, static_cast<int>(level), now_us});
    return ESP_OK;
}

esp_err_t esp_timer_create(const esp_timer_create_args_t* args, esp_timer_handle_t* handle) {
    ++create_count;
    if (fail_create)
        return ESP_FAIL;
    Check(args->callback && args->arg, "missing off callback");
    Check(args->dispatch_method == ESP_TIMER_TASK, "mutex callback must run in timer task");
    timer.args = *args;
    *handle = &timer;
    return ESP_OK;
}

esp_err_t esp_timer_start_once(esp_timer_handle_t handle, uint64_t timeout_us) {
    ++start_count;
    Check(handle == &timer, "invalid timer handle");
    if (fail_start)
        return ESP_FAIL;
    if (timer.armed)
        return ESP_ERR_INVALID_STATE;
    timer.armed = true;
    timer.deadline = now_us + static_cast<int64_t>(timeout_us);
    return ESP_OK;
}

int64_t esp_timer_get_time() { return now_us; }
void esp_rom_delay_us(uint32_t us) {
    Check(us <= 10, "relay pulse must not busy-wait for 500ms");
    now_us += us;
}

int main(int argc, char** argv) {
    Check(argc == 2, "expected test case");
    auto& relay = smv::MuxRelay::GetInstance();
    const std::string_view test = argv[1];
    if (test == "invalid") {
        Check(!relay.Pulse(1), "uninitialized relay request accepted");
        Check(writes.empty(), "uninitialized request drove GPIO");
        Initialize(relay);
        const auto before = writes.size();
        Check(!relay.Pulse(0) && !relay.Pulse(17) && !relay.Pulse(~0u), "invalid relay accepted");
        Check(writes.size() == before && !timer.armed, "invalid request changed outputs");
    } else if (test == "create_failure") {
        fail_create = true;
        Check(relay.Initialize() != ESP_OK, "timer creation failure ignored");
        Check(outputs[17] && levels[17] == 1, "creation failure left SIG active");
        Check(!relay.Pulse(1) && levels[17] == 1, "pulse allowed without an off timer");
    } else {
        Initialize(relay);
        if (test == "channels") {
            // S3 S2 S1 S0, transcribed independently from the supplied wiring table.
            constexpr const char* bits[] = {"0000", "0001", "0010", "0011", "0100", "0101",
                                            "0110", "0111", "1000", "1001", "1010", "1011",
                                            "1100", "1101", "1110", "1111"};
            unsigned number = 1;
            for (const char* expected : bits) {
                Check(relay.Pulse(number++), "valid relay request rejected");
                Check(levels[42] == expected[0] - '0' && levels[41] == expected[1] - '0' &&
                          levels[40] == expected[2] - '0' && levels[39] == expected[3] - '0',
                      "wrong MUX channel / S0-S3 order");
                Check(levels[17] == 0 && timer.armed, "selected relay not driven LOW");
                const auto low_time = now_us;
                Advance(499999);
                Check(levels[17] == 0, "relay released before 500ms");
                Advance(1);
                Check(levels[17] == 1 && !timer.armed, "relay not released at 500ms");
                Check(writes.back().time - low_time == 500000, "incorrect LOW pulse duration");
            }
        } else if (test == "busy") {
            Check(relay.Pulse(16), "first request rejected");
            Advance(250000);
            const auto before = writes.size();
            Check(!relay.Pulse(1) && !relay.Pulse(16), "busy relay request accepted");
            Check(writes.size() == before && start_count == 1,
                  "busy request changed or extended pulse");
            Advance(250000);
            Check(levels[17] == 1, "busy request delayed release");
            Check(relay.Pulse(1), "next pulse rejected after previous pulse completed");
        } else if (test == "start_failure") {
            fail_start = true;
            Check(!relay.Pulse(3), "timer start failure ignored");
            Check(levels[17] == 1 && !timer.armed, "timer failure left relay energized");
            fail_start = false;
            Check(relay.Pulse(4), "failed pulse prevented recovery");
            Advance(500000);
            Check(levels[17] == 1, "recovered pulse did not end");
        } else {
            Check(false, "unknown test case");
        }
    }
}
