#pragma once

#include <BLECharacteristic.h>
#include <BLEHIDDevice.h>
#include <BLEServer.h>

#include <atomic>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "transports/Transport.h"

namespace codex {

class BleTransport final : public Transport {
 public:
  void begin(QueueHandle_t incomingQueue);
  bool connected() const override;
  bool sendReport(const ReportBody& report) override;
  void setBattery(uint8_t percentage) override;

 private:
  class ServerCallbacks;
  class OutputCallbacks;

  void onConnected(bool connected);
  void onOutput(const uint8_t* data, size_t length);

  QueueHandle_t incomingQueue_ = nullptr;
  BLEHIDDevice* hid_ = nullptr;
  BLECharacteristic* input_ = nullptr;
  BLECharacteristic* output_ = nullptr;
  std::atomic<bool> connected_{false};
  uint8_t batteryPercentage_ = 100;
};

}  // namespace codex
