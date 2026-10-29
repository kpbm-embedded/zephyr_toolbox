# __name__

Zephyr C app with a Zig module over the C ABI (from `apps/_template_zig`: `scripts/app new <name> --template zig`).
Zig (32_24_microzig's zig 0.17, reached by `scripts/app` through `BOX_MICROZIG`; missing -> exit 3, skip) compiles
`zig/*.zig` to an object for the board's CPU; the app and its ztest suite link it.

| Path | What |
|---|---|
| `zig/filter.zig`, `zig/filter.h` | the Zig code (`export fn`, `extern struct`) and its C declarations |
| `zig/zig.cmake` | `zig build-obj` rule: target/CPU per board (Cortex-M3, RV32IMAC; add yours), no unwind tables |
| `src/main.c` | uses the filter |
| `tests/ztest/` | ztest suite for the Zig code |

```sh
scripts/app ztest __name__                       # qemu_cortex_m3 (also qemu_riscv32)
scripts/app build __name__ Debug qemu_cortex_m3
```

Board CPUs: BlackPill F411 / WeAct G431/H743: `-target thumb-freestanding-eabi -mcpu cortex_m4` (add an
`elseif(CONFIG_CPU_CORTEX_M4)` branch; with the FPU on use `eabihf`). More: `examples/zephyr/zig-ffi`.
