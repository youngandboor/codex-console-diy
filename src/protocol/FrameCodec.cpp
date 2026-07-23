#include "protocol/FrameCodec.h"

#include <cstring>

namespace codex {

AppendResult FrameAssembler::append(const uint8_t* report, size_t length) {
  if (report == nullptr || length < 2) {
    return AppendResult::kInvalid;
  }

  const size_t offset = (length >= 3 && report[0] == kReportId) ? 1 : 0;
  if (length < offset + 2 || report[offset] != kMessageType) {
    return AppendResult::kIgnored;
  }

  const size_t payloadLength = report[offset + 1];
  if (payloadLength > kPayloadSize || length < offset + 2 + payloadLength) {
    return AppendResult::kInvalid;
  }

  const auto* payload = reinterpret_cast<const char*>(report + offset + 2);
  constexpr char kTopLevelPrefix[] = "{\"method\"";
  const bool startsTopLevel =
      payloadLength >= sizeof(kTopLevelPrefix) - 1 &&
      std::memcmp(payload, kTopLevelPrefix, sizeof(kTopLevelPrefix) - 1) == 0;
  if (startsTopLevel && size_ != 0) {
    clear();
  }

  size_t payloadStart = 0;
  if (size_ == 0) {
    while (payloadStart < payloadLength && payload[payloadStart] != '{') {
      ++payloadStart;
    }
    if (payloadStart == payloadLength) {
      return AppendResult::kIgnored;
    }
  }

  const size_t bytesToCopy = payloadLength - payloadStart;
  if (bytesToCopy > buffer_.size() - size_) {
    clear();
    return AppendResult::kOverflow;
  }

  std::memcpy(buffer_.data() + size_, payload + payloadStart, bytesToCopy);
  size_ += bytesToCopy;
  return AppendResult::kAccepted;
}

std::string_view FrameAssembler::data() const {
  return std::string_view(buffer_.data(), size_);
}

void FrameAssembler::clear() {
  size_ = 0;
}

}  // namespace codex

