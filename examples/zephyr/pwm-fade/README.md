# zephyr / pwm-fade

Fades an LED in and out with `pwm_set_pulse_dt` on the `pwm-led0` alias.

Status: builds; untested on hardware. Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`); per-board changes are devicetree overlays in `boards/`, nothing in the Zephyr tree is edited.

## Boards and pins
BlackPill: TIM4_CH1 on PB6, **external LED + resistor needed** (PC13 is not on a PWM timer); Pico / Pico 2: on-board LED GP25 = PWM slice 4 channel B; Pro Micro nRF52840: board DTS pwm-led0 = PWM0 channel 0 (red LED pin from promicro_nrf52840-pinctrl.dtsi).

## Build / flash / monitor
```
. ../../../toolchain/zephyr/env.sh
west build -p auto -b <west-board> -d $ZEPHYR_OUT/examples/pwm-fade/<west-board> .
../../../scripts/app example pwm-fade <west-board>   # the same build via this ID's scripts/app
west flash -d $ZEPHYR_OUT/examples/pwm-fade/<west-board>   # uf2 boards: copy zephyr/zephyr.uf2 to the bootloader drive
../../../scripts/app monitor /dev/ttyACM0 115200   # or /dev/ttyUSB0 with a USB-UART on the console pins
```
West boards per target: blackpill_f411ce, weact_g431=weact_stm32g431_core, weact_h562=weact_stm32h562_core, weact_h743=mini_stm32h743, rpi_pico, rpi_pico2=rpi_pico2/rp2350a/m33, promicro_nrf52840=promicro_nrf52840/nrf52840/uf2, esp32c3=esp32c3_devkitm, esp32s3=esp32s3_devkitc/esp32s3/procpu, ch32v003=ch32v003evt.

## Expected output
LED brightness ramps up and down about every 2 s.

## Pin sources
Pico overlay from zephyr/ws/zephyr/samples/basic/fade_led/boards/rpi_pico.overlay; BlackPill pinctrl tim4_ch1_pb6 from blackpill_f411ce.dts; nRF from promicro_nrf52840_nrf52840_common.dts.
