#pragma once

#include "protocol/ProtocolEngine.h"
#include "transports/BleTransport.h"
#include "transports/UsbTransport.h"

namespace codex {

class TransportRouter final : public ProtocolSender {
 public:
  TransportRouter(BleTransport& ble, UsbTransport& usb) : ble_(ble), usb_(usb) {}

  TransportKind active() const;
  bool sendJson(TransportKind target, const char* json, size_t length) override;
  void setBattery(uint8_t percentage);

 private:
  Transport& transport(TransportKind kind);

  BleTransport& ble_;
  UsbTransport& usb_;
};

}  // namespace codex

