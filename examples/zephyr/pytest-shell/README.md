# zephyr / pytest-shell

Twister pytest harness: pytest drives the Zephyr shell of a booted image.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- `harness: pytest` with `pytest_root`; the `twister_harness` shell fixture sends commands and checks output
- twister adds the pytest plugin itself: do not pip-install pytest-twister-harness into the venv

## Run
```
../../../scripts/app ztest pytest-shell                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest pytest-shell qemu_riscv32
../../../scripts/app debug pytest-shell qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/pytest-shell/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
A board with a console UART: `twister --device-testing --device-serial /dev/ttyACM0`.

## Expected output
`3 of 3 executed test cases passed` in the twister summary.
