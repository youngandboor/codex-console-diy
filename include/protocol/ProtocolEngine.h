#pragma once

#include <ArduinoJson.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include "protocol/FrameCodec.h"
#include "transports/Transport.h"

namespace codex {

struct ThreadLight {
  uint32_t color = 0;
  float brightness = 0.0f;
  char effect[16] = "off";
  float speed = 0.0f;
};

struct LightingState {
  std::array<ThreadLight, 6> threads{};
  bool dirty = true;
};

class ProtocolSender {
 public:
  virtual ~ProtocolSender() = default;
  virtual bool sendJson(TransportKind target, const char* json, size_t length) = 0;
};

class ProtocolEngine {
 public:
  void onReport(const IncomingReport& report, ProtocolSender& sender);
  void sendKey(TransportKind target, const char* key, uint8_t action, int8_t agent,
               ProtocolSender& sender);
  void sendJoystick(TransportKind target, float angle, float distance,
                    ProtocolSender& sender);
  void setBattery(uint8_t percentage, bool charging);
  const LightingState& lighting() const { return lighting_; }
  void clearLightingDirty() { lighting_.dirty = false; }

 private:
  void handleRpc(TransportKind source, const JsonDocument& request, ProtocolSender& sender);
  void sendSuccess(TransportKind target, JsonVariantConst id, ProtocolSender& sender);
  void sendDocument(TransportKind target, const JsonDocument& document,
                    ProtocolSender& sender);
  void updateThreadLighting(JsonArrayConst values);

  std::array<FrameAssembler, 2> assemblers_{};
  StaticJsonDocument<4096> request_;
  LightingState lighting_{};
  uint8_t batteryPercentage_ = 100;
  bool charging_ = false;
};

}  // namespace codex
