#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace codex {

constexpr uint8_t kReportId = 6;
constexpr uint8_t kMessageType = 2;
constexpr size_t kReportBodySize = 63;
constexpr size_t kPayloadSize = 61;
constexpr size_t kMaxRpcSize = 4096;

using ReportBody = std::array<uint8_t, kReportBodySize>;

enum class AppendResult : uint8_t {
  kAccepted,
  kIgnored,
  kInvalid,
  kOverflow,
};

class FrameEncoder {
 public:
  template <typename Emit>
  static bool encode(std::string_view json, Emit emit) {
    size_t offset = 0;
    const size_t framedSize = json.size() + 1;

    while (offset < framedSize) {
      ReportBody report{};
      const size_t chunk =
          (framedSize - offset < kPayloadSize) ? framedSize - offset : kPayloadSize;
      report[0] = kMessageType;
      report[1] = static_cast<uint8_t>(chunk);

      for (size_t i = 0; i < chunk; ++i) {
        const size_t source = offset + i;
        report[2 + i] = source < json.size()
                            ? static_cast<uint8_t>(json[source])
                            : static_cast<uint8_t>('\n');
      }

      if (!emit(report)) {
        return false;
      }
      offset += chunk;
    }
    return true;
  }
};

class FrameAssembler {
 public:
  AppendResult append(const uint8_t* report, size_t length);
  std::string_view data() const;
  void clear();

 private:
  std::array<char, kMaxRpcSize> buffer_{};
  size_t size_ = 0;
};

}  // namespace codex

