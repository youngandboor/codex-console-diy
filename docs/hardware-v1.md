# Hardware v1 specification

## Interaction layout

The control surface keeps the original functional grouping without copying its
case or industrial design:

- six translucent Agent keys in a 3 x 2 block, each with independent RGB;
- six Command keys grouped below the Agent keys;
- a 2U Mic key and five 1U Command keys are preferred for a familiar layout;
- a push rotary encoder in the upper-right control area;
- a two-axis analog joystick beside or below the encoder;
- one separate capacitive or tactile communication control near the lower-left;
- perimeter RGB for recording, processing, connection, and charging effects.

Suggested logical arrangement:

```text
AGENT 1   AGENT 2   AGENT 3                 DIAL
AGENT 4   AGENT 5   AGENT 6               JOYSTICK
FAST      APPROVE   DECLINE    FORK
MIC (2U)             SEND      COMM/PAIR
```

The functional input count is 12 keys plus encoder push. The matrix should be
4 x 4 with one diode per switch, leaving spare positions for a local layer or
diagnostic key. The encoder quadrature lines and joystick axes remain direct
MCU inputs.

## Lighting

Use one addressable chain with explicit logical ranges:

| Range | Count | Purpose |
| --- | ---: | --- |
| Agent RGB | 6 | Host-provided task color and breathing state |
| Command RGB | 6 | Command feedback and local press feedback |
| Perimeter RGB | 8 | Moving voice, processing, pairing, and charge effects |

The firmware must cap global brightness and estimated LED current. Agent state
colors take priority over decorative effects. USB and battery operation use the
same visual policy; plugging in USB must not force all LEDs to full brightness.

## Core electronics

- MCU/module: ESP32-S3-WROOM-1, 8 MB flash is sufficient; PSRAM is optional.
- USB: native ESP32-S3 USB D-/D+ on GPIO19/GPIO20, USB-C device connector,
  5.1 kohm CC pull-downs, ESD protection, and 22/33 ohm series footprints.
- Power: 1-cell protected LiPo, nominal 1,500-2,000 mAh.
- Charger: BQ24074-class charger with power-path/load sharing. Do not connect a
  basic charger module, battery, USB rail, and system load in parallel.
- 3.3 V: regulator rated for at least 500 mA with the decoupling recommended by
  Espressif.
- Fuel gauge: MAX17048 or equivalent 1-cell gauge over I2C.
- RGB logic: dual-supply level shifter between 3.3 V MCU data and the LED rail.
- Controls: MX-compatible hot-swap footprints for ease of DIY assembly, EC11
  push encoder, and a spring-centered two-axis analog joystick.
- Service: physical power switch, reset, boot pads/button, UART pads, battery
  connector, and current-measurement jumper.

## Proposed GPIO allocation

This allocation avoids USB pins, boot strapping pins, and pins that may be
occupied by octal flash/PSRAM variants:

| Function | GPIO |
| --- | --- |
| Matrix rows | 4, 5, 6, 7 |
| Matrix columns | 8, 9, 10, 11 |
| Encoder A/B | 12, 13 |
| Joystick X/Y (ADC) | 1, 2 |
| Communication touch control | 14 |
| Addressable RGB data | 15 |
| Fuel gauge I2C SDA/SCL | 17, 18 |
| USB D-/D+ | 19, 20 |
| Charger status | 16, 21 |
| UART service pads | 43, 44 |

Encoder push occupies one position in the 4 x 4 matrix. Reserve the other
unused matrix positions rather than assigning them to mandatory v1 features.
Place a 510 ohm series footprint at the MCU end of the touch trace.

## Transport policy

Both transports may remain initialized, but only one sends user actions:

1. A mounted and ready USB HID connection has priority.
2. If USB is only supplying power or charging but is not enumerated, BLE stays
   active.
3. After USB unmounts, release all active logical controls before switching to
   BLE so no key remains logically held.
4. RPC responses return over the transport that received the request.
5. Lighting commands from the active transport win; inactive-host lighting is
   ignored after the protocol validation milestone.

## PCB gates

Do not order the integrated battery PCB until all of these pass on a DevKitC-1:

- ChatGPT Desktop recognizes both BLE and USB identities;
- Agent and Command actions work over both transports;
- USB priority does not duplicate a press on BLE;
- disconnect during a held key produces a clean release after reconnection;
- six Agent status colors are received and replayed reliably;
- native USB is revalidated after every descriptor or SDK change.

For Alpha Rev A, the user has explicitly accepted an integrated-board fast track after USB, BLE,
two-button input, and USB/BLE priority were validated. Matrix, RGB, encoder, joystick, and battery
validation may therefore happen on the first five integrated boards. This exception does not waive
ERC, DRC, footprint, USB-impedance, antenna-keepout, battery-polarity, or staged-power-up reviews.
