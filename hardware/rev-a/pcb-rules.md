# PCB Rev A rules

## Stackup and fabrication

- Four layers, 1.6 mm finished thickness, 1 oz outer copper.
- L1: components and short critical signals.
- L2: uninterrupted ground plane except the certified module antenna keepout.
- L3: power distribution and slow signals.
- L4: hot-swap sockets, reverse RGB, diodes and non-critical routing.
- Default minimum track/space 0.15/0.15 mm; use 0.20 mm where density permits.
- Signal vias 0.45/0.20 mm finished; do not use via-in-pad for the Alpha.
- All QFN exposed pads require paste-window reduction and ground thermal vias based on the
  manufacturer's current land pattern.

## USB

- Route GPIO19/GPIO20 as a 90 Ohm differential pair using the PCB supplier's actual stackup.
- Keep pair length mismatch below 0.5 mm and avoid vias. Do not guess the final width/gap before
  selecting the fabricator impedance table.
- Place the 22 Ohm series footprints at the ESP32 module, and the ESD array at the connector.
- Provide 5.1 kOhm Rd resistors on both USB-C CC pins. Do not connect USB-C SuperSpeed pins.
- Set the BQ24074 input-current limit conservatively for a USB host and verify suspend/configured
  behavior during schematic review; do not assume every USB-C source permits 1.5 A or 3 A.
- Keep switching inductors and the RGB data edge away from the USB pair.

## RF

- Put the ESP32-S3-WROOM-1 antenna at the rear board edge.
- Copy the current Espressif module land pattern and antenna keepout from the official datasheet.
- No copper, traces, battery, screws, metal plate or shield above or below the antenna keepout.
- Use a plastic rear wall near the antenna; the switch plate must stop before the keepout.

## Power

- VBUS feeds BQ24074 IN through protection; BQ24074 OUT feeds the 3.3 V and 5 V converters.
- The physical power switch controls converter EN pins. Charging and fuel measurement remain
  available while the user-facing electronics are off.
- Use a dedicated 3.3 V buck-boost for ESP32 and analog controls. Do not power ESP32 directly from
  the LiPo or the charger's dynamic system rail.
- The 5 V RGB rail must default off at reset. GPIO42 enables it only after firmware sets a current
  ceiling.
- Size the RGB rail for 1.5 A, but cap firmware brightness so normal operation remains below 700 mA.
- Use wide pours for BAT/SYS/5V_LED, short switching loops, and the exact regulator reference
  layouts. Keep both switch nodes away from joystick ADC and the antenna.
- Put a removable 0 Ohm current-measurement link between the battery connector and system.

## Inputs and lighting

- Use 1N4148W diodes on all populated matrix controls, with one common orientation documented on
  silkscreen.
- Put test pads on all four rows, all four columns, encoder A/B, joystick X/Y, RGB data, 3V3,
  5V_LED, SYS, BAT and GND.
- Reverse-mount SK6812MINI-E pixels require verified cutouts and orientation marks on both sides.
- Maintain a continuous ground reference beneath joystick ADC traces; add optional 1 kOhm/100 nF
  low-pass footprints at the MCU end.

## Mechanical and service

- Board outline is 148 x 110 mm inside a 154 x 116 mm enclosure.
- Use five M3 supports. No unsupported span may exceed 80 mm near frequently pressed keys.
- Keep the 70 x 40 x 7 mm battery envelope in a separate lower tray with a rigid insulating sheet.
- Expose BOOT and RESET through recessed pinholes; expose UART TX/RX/GND/3V3 as labeled pads.
- Add clear pin-1, diode direction, LED direction, battery polarity and connector labels.

## Release checks

Rev A may be ordered only after ERC and DRC are clean, every custom footprint is checked against a
manufacturer drawing at 1:1 scale, the USB pair is recalculated for the chosen stackup, and the
complete assembly passes a 3D interference review. Order five boards, assemble one without a
battery first, and current-limit the first power-up.
