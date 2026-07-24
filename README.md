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
- a development-board smoke input using the BOOT button as Agent 1;
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
port. Pressing the BOOT button after startup sends Agent 1 press/release events.
Do not reuse GPIO0 for a production key because it is a boot strapping pin.

## Project boundaries

- `src/protocol`: transport-independent HID framing and Codex RPC behavior.
- `src/transports`: BLE HOGP and ESP32-S3 native USB HID.
- `src/main.cpp`: Phase 0 orchestration only.
- `docs/hardware-v1.md`: interaction layout, electrical architecture, and PCB
  gates.
- `docs/development-plan.zh-CN.md`: parallel firmware, product, and PCB plan.
- `docs/purchase-checklist.zh-CN.md`: staged beginner purchase checklist and
  order gates.
- `docs/validation.md`: tests required before moving from breadboard to PCB.

The original Core2 project remains a read-only behavioral reference in the
sibling `codex-micro-4-core2` directory.
