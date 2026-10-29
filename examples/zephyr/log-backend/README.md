# zephyr / log-backend

Logging: levels, filters and a custom backend that captures formatted output.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- a module with compile-time level INF: `LOG_DBG` is compiled out (the test shows the runtime filter cannot bring it back)
- a custom backend (`src/capture_backend.c`, `LOG_BACKEND_DEFINE` + `log_output`) formats messages into RAM; a flash/BLE/network backend has the same shape
- runtime filtering per backend and source (`log_filter_set`), backend enable/disable, `LOG_HEXDUMP_INF`
- immediate mode, so a message is in the backend when the `LOG_*` call returns

## Run
```
../../../scripts/app ztest log-backend                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest log-backend qemu_riscv32
../../../scripts/app debug log-backend qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/log-backend/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
On a board the UART backend prints the same messages on the console.

## Expected output
`5 of 5 executed test cases passed` in the twister summary.
