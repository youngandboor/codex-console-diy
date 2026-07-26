#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "protocol/ProtocolEngine.h"
#include "transports/BleTransport.h"
#include "transports/TransportRouter.h"
#include "transports/UsbTransport.h"

namespace {

constexpr uint32_t kDebounceMs = 12;

struct BenchButton {
  uint8_t pin;
  const char* keyId;
  const char* label;
  bool stable = false;
  bool sampled = false;
  uint32_t changedAtMs = 0;
  codex::TransportKind transport = codex::TransportKind::kBle;
};

BenchButton benchButtons[] = {
    {4, "AG00", "Agent 1"},
    {5, "AG01", "Agent 2"},
};

QueueHandle_t incomingQueue = nullptr;
codex::BleTransport ble;
codex::UsbTransport usb;
codex::TransportRouter router(ble, usb);
codex::ProtocolEngine protocol;

void updateBenchButton(BenchButton& button) {
  const bool pressed = digitalRead(button.pin) == LOW;
  if (pressed != button.sampled) {
    button.sampled = pressed;
    button.changedAtMs = millis();
  }
  if (button.sampled == button.stable ||
      millis() - button.changedAtMs < kDebounceMs) {
    return;
  }

  button.stable = button.sampled;
  if (button.stable) {
    button.transport = router.active();
  }
  protocol.sendKey(button.transport, button.keyId, button.stable ? 1 : 0, 0,
                   router);
  Serial.printf("%s %s over %s\n", button.label,
                button.stable ? "down" : "up",
                button.transport == codex::TransportKind::kUsb ? "USB" : "BLE");
}

void updateBenchButtons() {
  for (BenchButton& button : benchButtons) {
    updateBenchButton(button);
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  for (const BenchButton& button : benchButtons) {
    pinMode(button.pin, INPUT_PULLUP);
  }

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

  updateBenchButtons();
  delay(2);
}
