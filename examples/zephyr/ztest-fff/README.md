# zephyr / ztest-fff

ztest with FFF fakes for a driver dependency.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- Fake Function Framework (`zephyr/fff.h`): fake functions, return sequences, call history

## Run
```
../../../scripts/app ztest ztest-fff                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest ztest-fff qemu_riscv32
../../../scripts/app debug ztest-fff qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/ztest-fff/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
Host-independent; runs the same on any board.

## Expected output
`3 of 3 executed test cases passed` in the twister summary.
