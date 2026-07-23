#pragma once

#include <cstddef>
#include <cstdint>

#include "protocol/FrameCodec.h"

namespace codex {

enum class TransportKind : uint8_t { kBle = 0, kUsb = 1 };

struct IncomingReport {
  TransportKind source = TransportKind::kBle;
  uint8_t length = 0;
  uint8_t bytes[kReportBodySize + 1]{};
};

class Transport {
 public:
  virtual ~Transport() = default;
  virtual bool connected() const = 0;
  virtual bool sendReport(const ReportBody& report) = 0;
  virtual void setBattery(uint8_t percentage) = 0;
};

}  // namespace codex

