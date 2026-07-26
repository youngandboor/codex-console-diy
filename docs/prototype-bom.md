# Phase 0/1 prototype BOM

Buy only the protocol-bench parts first. The integrated battery PCB should wait
until USB recognition is proven on the actual ChatGPT Desktop build.

## Phase 0: one-control protocol bench

| Item | Quantity | Notes |
| --- | ---: | --- |
| ESP32-S3-DevKitC-1-N8R8 | 1 | Purchased variant; use native USB for HID, PSRAM is optional for this milestone |
| Data-capable USB cable | 1 | USB-C or Micro-B according to the board revision |
| Momentary button | 2 | Current bench firmware maps GPIO4/GPIO5 to Agent 1/Agent 2 |
| Breadboard/jumpers | 1 set | For later encoder and joystick checks |

## Phase 1: full input and lighting breadboard

| Item | Quantity | Notes |
| --- | ---: | --- |
| MX-compatible switches | 12 | Choose feel first; hot-swap sockets come with the PCB |
| EC11 push encoder | 1 | Detent count and shaft length depend on the enclosure |
| Two-axis joystick breakout | 1 | Spring-centered, two analog outputs; validate mechanics before PCB |
| Addressable RGB breakout/strip | 20 pixels | Six Agent, six Command, eight perimeter pixels |
| 1N4148 DO-35 diodes | 16 | Through-hole parts for the breadboard; final PCB uses SMD 1N4148W |
| 10 kohm resistors | assorted | Pull-ups and bench configuration |
| 100 nF and 10 uF capacitors | assorted | Local decoupling for breadboard checks |

## PCB-alpha power parts

These are architecture selections, not a purchase order. Confirm package and
assembly availability with the PCB supplier before schematic capture.

| Function | Preferred part/class | Reason |
| --- | --- | --- |
| Charger and load sharing | BQ24074 | Single-cell charger with power path |
| Fuel gauge | MAX17048 | Low-power 1-cell state-of-charge reporting |
| 3.3 V rail | 500 mA+ buck-boost regulator | Stable MCU rail across the LiPo range |
| 5 V RGB rail | Enable-controlled boost regulator | Stable LED behavior and true shutdown |
| RGB level shift | 74AHCT1G125-class buffer | Clean 3.3 V to 5 V data edge |
| USB protection | Low-capacitance USB 2.0 ESD array | Protect D+ and D- without excessive loading |
| Battery | Protected 1S LiPo, 1,500-2,000 mAh | Final dimensions follow the enclosure |

Use a 500 mA USB input-current limit for the first PCB. LED firmware must cap
brightness and estimated current so the system remains stable while charging.
