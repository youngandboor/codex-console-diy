#include "protocol/ProtocolEngine.h"

#include <algorithm>
#include <cstring>

namespace codex {
namespace {

constexpr char kFirmwareVersion[] = "0.1.0-diy-s3";

size_t transportIndex(TransportKind transport) {
  return static_cast<size_t>(transport);
}

void copyEffect(char* destination, size_t capacity, const char* source) {
  if (source == nullptr || capacity == 0) {
    return;
  }
  std::strncpy(destination, source, capacity - 1);
  destination[capacity - 1] = '\0';
}

}  // namespace

void ProtocolEngine::onReport(const IncomingReport& report, ProtocolSender& sender) {
  FrameAssembler& assembler = assemblers_[transportIndex(report.source)];
  const AppendResult appended = assembler.append(report.bytes, report.length);
  if (appended == AppendResult::kInvalid || appended == AppendResult::kOverflow) {
    assembler.clear();
    return;
  }
  if (appended != AppendResult::kAccepted) {
    return;
  }

  request_.clear();
  const std::string_view data = assembler.data();
  const DeserializationError error = deserializeJson(request_, data.data(), data.size());
  if (error == DeserializationError::IncompleteInput) {
    return;
  }
  if (error) {
    assembler.clear();
    return;
  }

  handleRpc(report.source, request_, sender);
  assembler.clear();
}

void ProtocolEngine::sendKey(TransportKind target, const char* key, uint8_t action,
                             int8_t agent, ProtocolSender& sender) {
  StaticJsonDocument<192> message;
  message["method"] = "v.oai.hid";
  JsonObject params = message.createNestedObject("params");
  params["k"] = key;
  params["act"] = action;
  if (agent >= 0) {
    params["ag"] = agent;
  }
  sendDocument(target, message, sender);
}

void ProtocolEngine::sendJoystick(TransportKind target, float angle, float distance,
                                  ProtocolSender& sender) {
  StaticJsonDocument<160> message;
  message["method"] = "v.oai.rad";
  JsonObject params = message.createNestedObject("params");
  params["a"] = angle;
  params["d"] = distance;
  sendDocument(target, message, sender);
}

void ProtocolEngine::setBattery(uint8_t percentage, bool charging) {
  batteryPercentage_ = std::min<uint8_t>(percentage, 100);
  charging_ = charging;
}

void ProtocolEngine::handleRpc(TransportKind source, const JsonDocument& request,
                               ProtocolSender& sender) {
  const char* method = request["method"] | "";
  JsonVariantConst id = request["id"];
  JsonVariantConst params = request["params"];

  if (std::strcmp(method, "sys.version") == 0) {
    StaticJsonDocument<192> response;
    response["id"] = id;
    response.createNestedObject("result")["version"] = kFirmwareVersion;
    sendDocument(source, response, sender);
    return;
  }

  if (std::strcmp(method, "device.status") == 0) {
    StaticJsonDocument<320> response;
    response["id"] = id;
    JsonObject result = response.createNestedObject("result");
    result["version"] = kFirmwareVersion;
    result["profile_index"] = 0;
    result["layer_index"] = 1;
    result["battery"] = batteryPercentage_;
    result["is_charging"] = charging_;
    sendDocument(source, response, sender);
    return;
  }

  if (std::strcmp(method, "v.oai.thstatus") == 0 && params.is<JsonArrayConst>()) {
    updateThreadLighting(params.as<JsonArrayConst>());
    sendSuccess(source, id, sender);
    return;
  }

  if (std::strcmp(method, "v.oai.rgbcfg") == 0 ||
      std::strcmp(method, "lights.preview") == 0 ||
      std::strcmp(method, "host.focused_app") == 0) {
    sendSuccess(source, id, sender);
    return;
  }

  StaticJsonDocument<192> response;
  response["id"] = id;
  JsonObject error = response.createNestedObject("error");
  error["code"] = -32601;
  error["message"] = "Method not found";
  sendDocument(source, response, sender);
}

void ProtocolEngine::sendSuccess(TransportKind target, JsonVariantConst id,
                                 ProtocolSender& sender) {
  StaticJsonDocument<128> response;
  response["id"] = id;
  response.createNestedObject("result")["ok"] = true;
  sendDocument(target, response, sender);
}

void ProtocolEngine::sendDocument(TransportKind target, const JsonDocument& document,
                                  ProtocolSender& sender) {
  char json[1024];
  const size_t required = measureJson(document);
  if (required >= sizeof(json)) {
    return;
  }
  const size_t length = serializeJson(document, json, sizeof(json));
  if (length == 0 || length != required) {
    return;
  }
  sender.sendJson(target, json, length);
}

void ProtocolEngine::updateThreadLighting(JsonArrayConst values) {
  for (JsonObjectConst value : values) {
    const int id = value["id"] | -1;
    if (id < 0 || id >= static_cast<int>(lighting_.threads.size())) {
      continue;
    }
    ThreadLight& light = lighting_.threads[id];
    light.color = value["c"] | light.color;
    light.brightness = std::max(0.0f, std::min(1.0f, value["b"] | light.brightness));
    copyEffect(light.effect, sizeof(light.effect), value["e"] | light.effect);
    light.speed = value["s"] | light.speed;
  }
  lighting_.dirty = true;
}

}  // namespace codex
