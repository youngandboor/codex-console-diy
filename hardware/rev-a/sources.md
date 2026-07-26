# Rev A primary references

Use the latest English manufacturer datasheet, package drawing, reference schematic, and layout
guidance at schematic-capture time. Distributor previews are not a substitute for these sources.

- [ESP32-S3-WROOM-1 / WROOM-1U datasheet v1.8](https://documentation.espressif.com/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf)
- [TI BQ24074 product page and BQ2407x datasheet](https://www.ti.com/product/BQ24074)
- [TI TPS63070 buck-boost product page](https://www.ti.com/product/TPS63070)
- [TI TPS61023 5 V boost product page](https://www.ti.com/product/TPS61023)
- [Analog Devices MAX17048 fuel-gauge product page](https://www.analog.com/en/products/max17048.html)
- [TI SN74AHCT1G125 level-buffer product page](https://www.ti.com/product/SN74AHCT1G125)
- [TI TPD2EUSB30 USB ESD product page](https://www.ti.com/product/TPD2EUSB30)
- [Alps Alpine EC11E encoder catalog](https://tech.alpsalpine.com/assets/catalog/product-catalog-ec-01.en.pdf)
- [Alps Alpine RKJXV122400R current joystick series](https://tech.alpsalpine.com/e/products/category/muiti-control-devices/sub/01/series/rkjxv/)
- [Gateron G Pro 3.0 switch series](https://www.gateron.com/products/gateron-g-pro-30-switch-set)
- [ESP32-S3 touch sensor and GPIO mapping](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/api-reference/peripherals/touch_pad.html)
- [ST MP34DT06J optional PDM microphone datasheet](https://www.st.com/resource/en/datasheet/mp34dt06j.pdf)

The joystick baseline intentionally uses `RKJXV122400R`, which Alps Alpine lists as a current
standard part. The older and more commonly referenced `RKJXV1224005` is marked not recommended for
new designs and must not be used as the Rev A purchasing part number.
