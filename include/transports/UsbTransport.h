#pragma once

#include <USBHID.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "transports/Transport.h"

namespace codex {

class UsbTransport final : public USBHIDDevice, public Transport {
 public:
  UsbTransport();
  void begin(QueueHandle_t incomingQueue);
  bool connected() const override;
  bool sendReport(const ReportBody& report) override;
  void setBattery(uint8_t) override {}

  uint16_t _onGetDescriptor(uint8_t* buffer) override;
  void _onOutput(uint8_t reportId, const uint8_t* buffer, uint16_t length) override;

 private:
  QueueHandle_t incomingQueue_ = nullptr;
  USBHID hid_;
};

}  // namespace codex

