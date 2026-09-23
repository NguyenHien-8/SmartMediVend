#pragma once

#include <cstddef>
#include <deque>
#include <memory>

namespace smv {
// All methods called under the Application gate mutex; no audio decoding while
// waiting for STT. This keeps ordering of buffered compressed audio packets.
template <typename Packet>
class AudioPlaybackGate {
public:
    enum class Verdict { Waiting, Forward, Local, Fault };
    static constexpr size_t kMaxPackets = 150;  // 9 s at 60 ms/packet
    static constexpr size_t kMaxBytes = 160 * 1024;

    void Arm() {
        packets_.clear();
        bytes_ = 0;
        started_ = false;
        output_ready_ = false;
        verdict_ = Verdict::Waiting;
    }
    void StartTts() { if (verdict_ != Verdict::Local && verdict_ != Verdict::Fault) started_ = true; }
    void Ready() { output_ready_ = true; }
    void Decide(bool local) {
        if (verdict_ != Verdict::Waiting) return;
        verdict_ = local ? Verdict::Local : Verdict::Forward;
        if (local) Clear();
    }
    void Fail() { verdict_ = Verdict::Fault; Clear(); }

    // AUDIO EARLY BEFORE tts/start is dropped; without server turn IDs it is
    // impossible to distinguish this from stale packets from an earlier reply.
    void Receive(std::unique_ptr<Packet> packet) {
        if (!packet || !started_ || verdict_ == Verdict::Local || verdict_ == Verdict::Fault) return;
        if (packets_.size() >= kMaxPackets ||
            packet->payload.size() > kMaxBytes - bytes_) {
            Fail();
            return;
        }
        bytes_ += packet->payload.size();
        packets_.push_back(std::move(packet));
    }
    std::unique_ptr<Packet> PopReady() {
        if (verdict_ != Verdict::Forward || !output_ready_ || packets_.empty()) return {};
        auto result = std::move(packets_.front());
        bytes_ -= result->payload.size();
        packets_.pop_front();
        return result;
    }
    Verdict verdict() const { return verdict_; }
    bool started() const { return started_; }
    bool has_pending() const { return !packets_.empty(); }
    size_t pending() const { return packets_.size(); }

private:
    void Clear() { packets_.clear(); bytes_ = 0; }
    Verdict verdict_ = Verdict::Waiting;
    bool started_ = false;
    bool output_ready_ = false;
    size_t bytes_ = 0;
    std::deque<std::unique_ptr<Packet>> packets_;
};
}  // namespace smv
