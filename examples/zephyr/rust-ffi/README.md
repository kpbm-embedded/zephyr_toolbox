# zephyr / rust-ffi

A no_std Rust static library called from a C Zephyr app over the C ABI.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2.

## What it shows
- no zephyr-lang-rust, no bindgen: `CMakeLists.txt` runs cargo (`--locked`) for the board's target (thumbv7m-none-eabi, riscv32imac-unknown-none-elf) and links `libzffi.a`
- `#[repr(C)]` struct shared with C (a PI controller), `extern "C"` functions, NULL handling on the Rust side
- `#[panic_handler]` calls back into C (`k_panic`)
- cargo from 32_17_embedded_rust (`BOX_EMBEDDED_RUST`); missing -> exit 3 (skip)

## Run
```
../../../scripts/app ztest rust-ffi                  # twister on qemu_cortex_m3
../../../scripts/app example rust-ffi qemu_cortex_m3 # build only
```
Report: `$ZEPHYR_OUT/twister/rust-ffi/<board>/twister.xml`.

## On a board
Map the board's Rust target in `CMakeLists.txt` (BlackPill with the FPU off: thumbv7em-none-eabi, which 32_17 does not install; with `CONFIG_FPU=y`: thumbv7em-none-eabihf).

## Expected output
`3 of 3 executed test cases passed`.
