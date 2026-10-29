# zephyr / i2c-scan

Probes addresses 0x08-0x77 with a zero-length write and prints responders every 5 s.

Status: builds; untested on hardware. Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`); per-board changes are devicetree overlays in `boards/`, nothing in the Zephyr tree is edited.

## Boards and pins
BlackPill I2C1 PB8 SCL/PB9 SDA; Pico I2C0 and Pico 2 I2C0 default pins (pico pinctrl dtsi: GP4 SDA / GP5 SCL); Pro Micro nRF52840 I2C0 (pinctrl dtsi); ESP32-C3 / ESP32-S3 I2C0 default pinctrl. Bus selected by `zephyr,user { i2c-bus = <&iN>; }` in the overlay. Needs pull-ups on SDA/SCL.

## Build / flash / monitor
```
. ../../../toolchain/zephyr/env.sh
west build -p auto -b <west-board> -d $ZEPHYR_OUT/examples/i2c-scan/<west-board> .
../../../scripts/app example i2c-scan <west-board>   # the same build via this ID's scripts/app
west flash -d $ZEPHYR_OUT/examples/i2c-scan/<west-board>   # uf2 boards: copy zephyr/zephyr.uf2 to the bootloader drive
../../../scripts/app monitor /dev/ttyACM0 115200   # or /dev/ttyUSB0 with a USB-UART on the console pins
```
West boards per target: blackpill_f411ce, weact_g431=weact_stm32g431_core, weact_h562=weact_stm32h562_core, weact_h743=mini_stm32h743, rpi_pico, rpi_pico2=rpi_pico2/rp2350a/m33, promicro_nrf52840=promicro_nrf52840/nrf52840/uf2, esp32c3=esp32c3_devkitm, esp32s3=esp32s3_devkitc/esp32s3/procpu, ch32v003=ch32v003evt.

## Expected output
`scanning <bus>`, one line per device, `N device(s)`.

## Pin sources
Each board's DTS and *-pinctrl.dtsi under zephyr/ws/zephyr/boards (exact ESP32 pins: see esp32c3_devkitm-pinctrl.dtsi / esp32s3_devkitc-pinctrl.dtsi).
