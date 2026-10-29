# zephyr / zig-ffi

Zig code in a C Zephyr app over the C ABI.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2.

## What it shows
- `zig build-obj` (zig 0.17 of 32_24_microzig, `BOX_MICROZIG`; caches in `cache/zig` and the build output) for the board's CPU: `thumb-freestanding-eabi`/`cortex_m3`, `riscv32-freestanding-none`/`generic_rv32+m+a+c`
- `export fn` and `extern struct` shared with C; Q16.16 saturating math, Fletcher-16, a moving average
- `-fno-unwind-tables`: otherwise `.ARM.extab` lands as an orphan section and the Zephyr link fails on the warning
- missing zig -> exit 3 (skip)

## Run
```
../../../scripts/app ztest zig-ffi                  # twister on qemu_cortex_m3
../../../scripts/app example zig-ffi qemu_cortex_m3 # build only
```
Report: `$ZEPHYR_OUT/twister/zig-ffi/<board>/twister.xml`.

## On a board
Add the board's CPU in `CMakeLists.txt` (BlackPill: `-mcpu cortex_m4`, `eabihf` with the FPU on).

## Expected output
`3 of 3 executed test cases passed`.
