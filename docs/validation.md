# Validation plan

## Phase 0: protocol bench

- Build and run native framing tests.
- Enumerate VID `0x303A`, PID `0x8360`, usage page `0xFF00`, report ID `6`.
- Verify ChatGPT Desktop exposes Settings > Codex Micro over BLE.
- Repeat over native USB. Treat failure here as a protocol investigation, not a
  PCB problem.
- Press and release Agent 1 at least 100 times on each transport; confirm no
  stuck state.
- Plug and unplug USB while BLE is connected; confirm exactly one action per
  physical edge.

## Phase 1: full input and lighting breadboard

- Scan all matrix positions with diode direction and ghosting tests.
- Verify 10-15 ms debounce without delaying encoder steps.
- Verify encoder CW, CCW, tap, and 500 ms host-interpreted hold.
- Calibrate joystick center at boot and apply deadzone plus directional
  hysteresis.
- Replay all observed Agent colors and breathing states on the six Agent LEDs.
- Apply a hard LED-current and brightness ceiling.

## Phase 2: battery carrier and PCB alpha

- Test USB load sharing at empty, half, and full battery states.
- Measure peak 3.3 V droop during BLE transmit plus an RGB update.
- Verify charge status, battery percentage, low-battery dimming, and shutdown.
- Measure active, idle-lit, idle-dark, BLE advertising, and deep-sleep current.
- Perform at least 20 BLE reconnects and 20 USB enumerate/unplug cycles.
- Forget and re-pair after every HID descriptor or PnP identity change.

