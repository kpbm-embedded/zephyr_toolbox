# zephyr / ztest-basic

ztest suite for a small FIFO module.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- fixtures (setup/before/after/teardown), `zassert_*` vs `zexpect_*`, skipped cases
- shuffled order (`CONFIG_ZTEST_SHUFFLE`) and a no-shuffle twister variant

## Run
```
../../../scripts/app ztest ztest-basic                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest ztest-basic qemu_riscv32
../../../scripts/app debug ztest-basic qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/ztest-basic/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
On a board the result is printed on the console UART (`twister --device-testing --device-serial /dev/ttyACM0`).

## Expected output
`10 of 10 executed test cases passed` in the twister summary (all variants).
