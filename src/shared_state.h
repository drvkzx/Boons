#pragma once

#include <Windows.h>
#include <cstdint>

#include "boons.h"

namespace boonpanel {

inline constexpr std::uint32_t kSharedMagic = 0x42504E4Cu; // "BPNL"
inline constexpr std::uint32_t kSharedVersion = 2;
inline constexpr std::size_t kMaxTrackedStacks = 128;

// Shared between the Nexus-loaded copy and the ArcDPS-loaded copy of this DLL.
// Sequence is a Windows interlocked seqlock counter: odd means writer active.
struct SharedSnapshot {
    volatile LONG sequence;
    std::uint32_t magic;
    std::uint32_t version;
    std::uint32_t feed_seen;
    std::uint32_t track_count[kBoonCount];
    std::uint64_t expires_at_ms[kBoonCount][kMaxTrackedStacks];
};

class SharedState final {
public:
    SharedState() = default;
    ~SharedState() = default;

    SharedState(const SharedState&) = delete;
    SharedState& operator=(const SharedState&) = delete;

    bool Open() noexcept;
    void Close() noexcept;
    bool IsOpen() const noexcept { return view_ != nullptr; }

    void Publish(const std::uint32_t* track_count,
                 const std::uint64_t* flattened_expiries,
                 bool feed_seen) noexcept;

    bool Read(SharedSnapshot& out) const noexcept;

private:
    HANDLE mapping_ = nullptr;
    SharedSnapshot* view_ = nullptr;
};

std::uint64_t TickNowMs() noexcept;

} // namespace boonpanel
