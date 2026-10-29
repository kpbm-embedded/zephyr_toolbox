# zephyr / input-keys

Input subsystem: gpio-keys buttons on an emulated GPIO controller.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- Zephyr's `gpio-keys` driver (interrupt + debounce -> `INPUT_EV_KEY` events) on `zephyr,gpio-emul`; the test presses keys with `gpio_emul_input_set`
- a bouncing press gives one event; an `INPUT_CALLBACK_DEFINE` callback drives a small menu
- software key events with `input_report_key` (no device)

## Run
```
../../../scripts/app ztest input-keys                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest input-keys qemu_riscv32
../../../scripts/app debug input-keys qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/input-keys/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
Map the buttons to real pins (BlackPill KEY: `<&gpioa 0 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>`) and drop the emulated controller.

## Expected output
`5 of 5 executed test cases passed` in the twister summary.
