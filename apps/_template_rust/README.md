# __name__

Zephyr app written in Rust with zephyr-lang-rust (from `apps/_template_rust`: `scripts/app new <name> --template rust`).
`rust_main()` in `src/lib.rs` is the entry point; Kconfig values, devicetree and kernel objects come through the
`zephyr` crate. The Rust toolchain is 32_17_embedded_rust's (reached by `scripts/app`, `BOX_EMBEDDED_RUST`), the
module is `modules/lang-rust` (`scripts/app setup --only rust`); without them the commands exit 3 (skip).

| Path | What |
|---|---|
| `Cargo.toml`, `Cargo.lock` | crate `rustapp` (the name zephyr-lang-rust links), dependency versions locked |
| `src/lib.rs` | `rust_main`: a worker thread (`#[zephyr::thread]`), a channel, `printkln!` |
| `sample.yaml` | twister console test (qemu_cortex_m3: thumbv7m is a 32_17 target; qemu_riscv32 needs riscv32i, not installed there) |

```sh
scripts/app ztest __name__                          # build + run on qemu_cortex_m3, console lines checked
scripts/app build __name__ Debug qemu_cortex_m3
```

Board targets in 32_17: thumbv7em-none-eabihf (BlackPill F411, WeAct G431/H562/H743, nRF52840), thumbv6m (Pico),
thumbv8m.main (Pico 2). Rust in a C app without this module: `examples/zephyr/rust-ffi`.
