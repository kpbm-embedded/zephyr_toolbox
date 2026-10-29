# zephyr / sensor-emul

Driver test without hardware: a real sensor driver on an emulated I2C bus.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- Zephyr's Fintek F75303 driver talks I2C to `zephyr,i2c-emul-controller`; the chip's emulator answers
- the test sets temperatures with `emul_sensor_backend_set_channel` and reads them back through the sensor API (local and remote channels, 0.125 C resolution, sample range)
- an app-level thermal monitor (`src/thermal.c`) tested against the emulated chip

## Run
```
../../../scripts/app ztest sensor-emul                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest sensor-emul qemu_riscv32
../../../scripts/app debug sensor-emul qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/sensor-emul/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
Put the sensor node under a real I2C bus (BlackPill: `&i2c1`, PB6/PB7) and drop the emulator controller; the app code does not change.

## Expected output
`5 of 5 executed test cases passed` in the twister summary.
