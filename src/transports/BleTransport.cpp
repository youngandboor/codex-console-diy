#include "transports/BleTransport.h"

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLESecurity.h>

#include <algorithm>
#include <cstring>
#include <string>

namespace codex {
namespace {

constexpr char kDeviceName[] = "Codex Micro";
constexpr char kManufacturer[] = "Work Louder";
constexpr uint16_t kVendorId = 0x303A;
constexpr uint16_t kProductId = 0x8360;

constexpr uint16_t swapBytes(uint16_t value) {
  return static_cast<uint16_t>((value << 8) | (value >> 8));
}

const uint8_t kReportMap[] = {
    0x06, 0x00, 0xFF, 0x09, 0x01, 0xA1, 0x01, 0x85, 0x06, 0x15,
    0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x3F, 0x09, 0x01,
    0x81, 0x02, 0x95, 0x3F, 0x09, 0x02, 0x91, 0x02, 0xC0,
};

class SecurityCallbacks final : public BLESecurityCallbacks {
 public:
  bool onSecurityRequest() override { return true; }
  uint32_t onPassKeyRequest() override { return 0; }
  void onPassKeyNotify(uint32_t) override {}
  bool onConfirmPIN(uint32_t) override { return true; }
  void onAuthenticationComplete(esp_ble_auth_cmpl_t result) override {
    Serial.printf("BLE pairing %s\n", result.success ? "complete" : "failed");
  }
};

}  // namespace

class BleTransport::ServerCallbacks final : public BLEServerCallbacks {
 public:
  explicit ServerCallbacks(BleTransport& owner) : owner_(owner) {}
  void onConnect(BLEServer*) override { owner_.onConnected(true); }
  void onDisconnect(BLEServer*) override {
    owner_.onConnected(false);
    BLEDevice::startAdvertising();
  }

 private:
  BleTransport& owner_;
};

class BleTransport::OutputCallbacks final : public BLECharacteristicCallbacks {
 public:
  explicit OutputCallbacks(BleTransport& owner) : owner_(owner) {}
  void onWrite(BLECharacteristic* characteristic) override {
    const std::string value = characteristic->getValue();
    owner_.onOutput(reinterpret_cast<const uint8_t*>(value.data()), value.size());
  }

 private:
  BleTransport& owner_;
};

void BleTransport::begin(QueueHandle_t incomingQueue) {
  incomingQueue_ = incomingQueue;
  BLEDevice::init(kDeviceName);
  BLEDevice::setSecurityCallbacks(new SecurityCallbacks());

  auto* security = new BLESecurity();
  security->setCapability(ESP_IO_CAP_NONE);
  security->setAuthenticationMode(ESP_LE_AUTH_BOND);

  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks(*this));

  hid_ = new BLEHIDDevice(server);
  hid_->manufacturer()->setValue(kManufacturer);
  hid_->pnp(0x02, swapBytes(kVendorId), swapBytes(kProductId), swapBytes(0x0101));
  hid_->hidInfo(0x00, 0x01);
  hid_->reportMap(const_cast<uint8_t*>(kReportMap), sizeof(kReportMap));
  input_ = hid_->inputReport(kReportId);
  output_ = hid_->outputReport(kReportId);
  output_->setCallbacks(new OutputCallbacks(*this));
  hid_->startServices();
  hid_->setBatteryLevel(batteryPercentage_);

  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->setAppearance(GENERIC_HID);
  advertising->addServiceUUID(hid_->hidService()->getUUID());
  advertising->setScanResponse(true);
  advertising->setMinPreferred(0x06);
  advertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();
}

bool BleTransport::connected() const {
  return connected_.load();
}

bool BleTransport::sendReport(const ReportBody& report) {
  if (!connected_.load() || input_ == nullptr) {
    return false;
  }
  input_->setValue(const_cast<uint8_t*>(report.data()), report.size());
  input_->notify();
  return true;
}

void BleTransport::setBattery(uint8_t percentage) {
  batteryPercentage_ = std::min<uint8_t>(percentage, 100);
  if (connected_.load() && hid_ != nullptr) {
    hid_->setBatteryLevel(batteryPercentage_);
  }
}

void BleTransport::onConnected(bool connected) {
  connected_.store(connected);
  Serial.printf("BLE host %s\n", connected ? "connected" : "disconnected");
}

void BleTransport::onOutput(const uint8_t* data, size_t length) {
  if (incomingQueue_ == nullptr || data == nullptr || length == 0 ||
      length > kReportBodySize + 1) {
    return;
  }
  IncomingReport report;
  report.source = TransportKind::kBle;
  report.length = static_cast<uint8_t>(length);
  std::memcpy(report.bytes, data, length);
  xQueueSend(incomingQueue_, &report, 0);
}

}  // namespace codex
