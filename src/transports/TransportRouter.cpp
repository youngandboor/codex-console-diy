#include "transports/TransportRouter.h"

#include <Arduino.h>

#include <string_view>

namespace codex {

TransportKind TransportRouter::active() const {
  return usb_.connected() ? TransportKind::kUsb : TransportKind::kBle;
}

bool TransportRouter::sendJson(TransportKind target, const char* json, size_t length) {
  if (json == nullptr || length == 0) {
    return false;
  }
  Transport& selected = transport(target);
  if (!selected.connected()) {
    return false;
  }

  return FrameEncoder::encode(std::string_view(json, length), [&](const ReportBody& report) {
    const bool sent = selected.sendReport(report);
    if (sent && target == TransportKind::kBle) {
      delay(4);
    }
    return sent;
  });
}

void TransportRouter::setBattery(uint8_t percentage) {
  ble_.setBattery(percentage);
}

Transport& TransportRouter::transport(TransportKind kind) {
  return kind == TransportKind::kUsb ? static_cast<Transport&>(usb_)
                                     : static_cast<Transport&>(ble_);
}

}  // namespace codex

