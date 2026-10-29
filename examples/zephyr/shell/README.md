# zephyr / shell

Zephyr shell on the console UART (`zephyr,shell-uart`) with kernel/device commands and a custom `demo` group.

Status: builds; untested on hardware. Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`); per-board changes are devicetree overlays in `boards/`, nothing in the Zephyr tree is edited.

## Boards and pins
Shell UART = console UART (same pins as above). Console UART is the board's zephyr,console (BlackPill USART1 PA9 TX/PA10 RX; G431 USART2 PA2/PA3; Pico/Pico 2 UART0 GP0/GP1; ESP32 UART0).

## Build / flash / monitor
```
. ../../../toolchain/zephyr/env.sh
west build -p auto -b <west-board> -d $ZEPHYR_OUT/examples/shell/<west-board> .
../../../scripts/app example shell <west-board>   # the same build via this ID's scripts/app
west flash -d $ZEPHYR_OUT/examples/shell/<west-board>   # uf2 boards: copy zephyr/zephyr.uf2 to the bootloader drive
../../../scripts/app monitor /dev/ttyACM0 115200   # or /dev/ttyUSB0 with a USB-UART on the console pins
```
West boards per target: blackpill_f411ce, weact_g431=weact_stm32g431_core, weact_h562=weact_stm32h562_core, weact_h743=mini_stm32h743, rpi_pico, rpi_pico2=rpi_pico2/rp2350a/m33, promicro_nrf52840=promicro_nrf52840/nrf52840/uf2, esp32c3=esp32c3_devkitm, esp32s3=esp32s3_devkitc/esp32s3/procpu, ch32v003=ch32v003evt.

## Expected output
Prompt `uart:~$`; try `help`, `demo hello you`, `demo uptime`, `kernel version`, `device list`.

## Pin sources
Board DTS chosen zephyr,shell-uart.
