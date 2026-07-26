# Codex Console DIY

Personal-use ESP32-S3 control surface compatible with Codex Micro features in
ChatGPT Desktop. The target is functional equivalence rather than a mechanical
copy: similar control grouping and lighting, Bluetooth LE, native USB, and a
single-cell rechargeable battery.

This repository is an independent compatibility experiment. It is not
affiliated with or endorsed by OpenAI or Work Louder. The compatibility
identity and undocumented protocol are used only for personal interoperability
testing.

## Current milestone

Phase 0 provides:

- the exact 63-byte vendor HID framing used by the Core2 reference project;
- a bounded, allocation-free receive assembler;
- BLE and native USB HID transports on ESP32-S3;
- USB-first transport routing, with BLE fallback;
- the observed Codex RPC methods and six Agent status values;
- two breadboard inputs on GPIO4/GPIO5 using external Agent 1/Agent 2 buttons;
- native unit tests for framing, fragmentation, validation, and recovery.

Physical key scanning, RGB output, the joystick, encoder, fuel gauge, and sleep
policy are intentionally the next milestone. The USB protocol is implemented
as an informed compatibility hypothesis and still requires validation against
ChatGPT Desktop before a PCB is ordered.

## Build

```sh
pio test -e native
pio run -e esp32-s3-devkitc-1
```

For the development-board smoke test, use an ESP32-S3 DevKitC-1 native USB
port. Connect normally-open buttons from GPIO4 and GPIO5 to a shared GND;
pressing them sends Agent 1 and Agent 2 events respectively. The firmware
enables the GPIO internal pull-ups, so no external resistors are required. The
earlier GPIO0/BOOT bench input was only used for first-board validation and is
no longer used as a user control.

## Project boundaries

- `src/protocol`: transport-independent HID framing and Codex RPC behavior.
- `src/transports`: BLE HOGP and ESP32-S3 native USB HID.
- `src/main.cpp`: Phase 0 orchestration only.
- `docs/hardware-v1.md`: interaction layout, electrical architecture, and PCB
  gates.
- `docs/development-plan.zh-CN.md`: parallel firmware, product, and PCB plan.
- `docs/purchase-checklist.zh-CN.md`: staged beginner purchase checklist and
  order gates.
- `docs/wireless-audio-options.zh-CN.md`: 2.4 GHz dongle and BLE Audio tradeoff
  analysis.
- `docs/hardware-validation-log.zh-CN.md`: dated physical-hardware validation
  results.
- `docs/validation.md`: tests required before moving from breadboard to PCB.
- `hardware/rev-a`: integrated Alpha PCB, enclosure, placement, BOM, and 1:1
  mechanical design baseline.

The original Core2 project remains a read-only behavioral reference in the
sibling `codex-micro-4-core2` directory.
