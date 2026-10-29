# zephyr / adc-read

Reads one ADC channel through `zephyr,user` io-channels and prints raw value and millivolts.

Status: builds; untested on hardware. Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`); per-board changes are devicetree overlays in `boards/`, nothing in the Zephyr tree is edited.

## Boards and pins
BlackPill ADC1 channel 1 = PA1; Pico / Pico 2 channel 0 = GP26; CH32V003 channel 0 = PA2 (floating by default in the sample).

## Build / flash / monitor
```
. ../../../toolchain/zephyr/env.sh
west build -p auto -b <west-board> -d $ZEPHYR_OUT/examples/adc-read/<west-board> .
../../../scripts/app example adc-read <west-board>   # the same build via this ID's scripts/app
west flash -d $ZEPHYR_OUT/examples/adc-read/<west-board>   # uf2 boards: copy zephyr/zephyr.uf2 to the bootloader drive
../../../scripts/app monitor /dev/ttyACM0 115200   # or /dev/ttyUSB0 with a USB-UART on the console pins
```
West boards per target: blackpill_f411ce, weact_g431=weact_stm32g431_core, weact_h562=weact_stm32h562_core, weact_h743=mini_stm32h743, rpi_pico, rpi_pico2=rpi_pico2/rp2350a/m33, promicro_nrf52840=promicro_nrf52840/nrf52840/uf2, esp32c3=esp32c3_devkitm, esp32s3=esp32s3_devkitc/esp32s3/procpu, ch32v003=ch32v003evt.

## Expected output
`raw N = M mV` twice per second.

## Pin sources
Overlays copied from zephyr/ws/zephyr/samples/drivers/adc/adc_dt/boards/{rpi_pico,rpi_pico2_rp2350a_m33,ch32v003evt}.overlay; BlackPill written to match blackpill_f411ce.dts (adc1_in1_pa1).
