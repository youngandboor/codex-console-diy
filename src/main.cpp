#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "protocol/ProtocolEngine.h"
#include "transports/BleTransport.h"
#include "transports/TransportRouter.h"
#include "transports/UsbTransport.h"

namespace {

constexpr uint8_t kBenchButtonPin = 4;
constexpr uint32_t kDebounceMs = 12;

QueueHandle_t incomingQueue = nullptr;
codex::BleTransport ble;
codex::UsbTransport usb;
codex::TransportRouter router(ble, usb);
codex::ProtocolEngine protocol;

bool stableButton = false;
bool sampledButton = false;
uint32_t changedAtMs = 0;
codex::TransportKind buttonTransport = codex::TransportKind::kBle;

void updateSmokeButton() {
  const bool pressed = digitalRead(kBenchButtonPin) == LOW;
  if (pressed != sampledButton) {
    sampledButton = pressed;
    changedAtMs = millis();
  }
  if (sampledButton == stableButton || millis() - changedAtMs < kDebounceMs) {
    return;
  }

  stableButton = sampledButton;
  if (stableButton) {
    buttonTransport = router.active();
  }
  const codex::TransportKind target = buttonTransport;
  protocol.sendKey(target, "AG00", stableButton ? 1 : 0, 0, router);
  Serial.printf("Agent 1 %s over %s\n", stableButton ? "down" : "up",
                target == codex::TransportKind::kUsb ? "USB" : "BLE");
}

}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(kBenchButtonPin, INPUT_PULLUP);

  incomingQueue = xQueueCreate(12, sizeof(codex::IncomingReport));
  if (incomingQueue == nullptr) {
    while (true) {
      delay(1000);
    }
  }

  usb.begin(incomingQueue);
  ble.begin(incomingQueue);
  protocol.setBattery(100, false);
  router.setBattery(100);
  Serial.println("CODEX_CONSOLE_PHASE0_READY");
}

void loop() {
  codex::IncomingReport report;
  while (xQueueReceive(incomingQueue, &report, 0) == pdTRUE) {
    protocol.onReport(report, router);
  }

  updateSmokeButton();
  delay(2);
}
