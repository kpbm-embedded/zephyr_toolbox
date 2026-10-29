# zephyr / rust-app

Rust app on Zephyr with zephyr-lang-rust.

Status: passes on qemu_cortex_m3 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2.

## What it shows
- `rust_cargo_application()`: cargo builds `src/lib.rs` (crate `rustapp`) for the board's Rust target; `rust_main()` is the entry point
- a worker thread from `#[zephyr::thread]`, bounded channels between threads, `printkln!`, `zephyr::kconfig`
- channels backed by Zephyr queues cannot be freed yet (zephyr-lang-rust panics on drop): the worker serves forever, `rust_main` forgets its ends
- toolchain: rustup/cargo of 32_17_embedded_rust (`BOX_EMBEDDED_RUST`, else the sibling folder); module `modules/lang-rust` (`scripts/app setup --only rust`, pinned commit); bindgen's libclang from `LIBCLANG_PATH` or 32_22_rust_next's esp-clang; crates in `cache/cargo`, versions in `Cargo.lock`. Missing -> exit 3 (skip)

## Run
```
../../../scripts/app ztest rust-app                  # twister on qemu_cortex_m3
../../../scripts/app example rust-app qemu_cortex_m3 # build only
```
Report: `$ZEPHYR_OUT/twister/rust-app/<board>/twister.xml`.

## On a board
BlackPill F411, WeAct G431/H562/H743, nRF52840: target thumbv7em-none-eabihf (installed in 32_17). qemu_riscv32 is not run: zephyr-lang-rust picks riscv32i-unknown-none-elf, which 32_17 does not install.

## Expected output
console lines `rust-app on qemu_cortex_m3`, `sum of squares 1..10 = 385`, `crc16 = 0x29b1`, `rust-app: PASS` (twister: 1 of 1 passed).
