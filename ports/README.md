# ports: out-of-tree Zephyr boards and SoCs for the application SoCs

A Zephyr module (`zephyr/module.yml`: board_root, soc_root, dts_root, Kconfig, CMake) used with Zephyr v4.4.2 from
`../toolchain/zephyr`. It is found through `ZEPHYR_EXTRA_MODULES=<this ID>/ports`: `../scripts/app` exports it, and the
app template `../apps/_template_soc/CMakeLists.txt` appends it itself (so CLion and plain `cmake` work too).
Nothing in the west workspace is modified. Board list, examples and boot commands: `../examples/zephyr/boards.yaml`,
`../scripts/app boards`; results: `../BOARDS.md`.

| Path | What | Origin |
|---|---|---|
| `soc/allwinner/sun50i_h616` | H616 (AArch64) | Zephyr main `sun50i_h618`, renamed; + `mmu_regions.c` for v4.4 |
| `soc/allwinner/sun8i_t113` | T113-S3 (Cortex-A7) | new |
| `soc/rockchip/rv1106` | RV1106/RV1103 (Cortex-A7) | Zephyr main (PR #103787), own series here |
| `soc/rockchip/rk3506` | RK3506 (Cortex-A7) | Zephyr PR #110923 (unmerged), own series here |
| `soc/common/armv7a_uboot_go.c` | A7 early init after U-Boot `go`: caches/MMU off, VBAR | PR #110923 RK3506 code, generalised |
| `soc/sophgo/cv18xx` | CV1800B, SG2002 little C906 | after Zephyr PR #69594 (unmerged) |
| `soc/canaan/k210` | K210 | new |
| `soc/canaan/k230` | K230/K230D CPU0 (C908) | after Zephyr PR #74169 (unmerged) |
| `drivers/gpio/gpio_allwinner.c` | Allwinner PIO | Zephyr main (PR #110502) + `allwinner,pull-offset`, optional IRQ |
| `drivers/gpio/gpio_rockchip_v2.c` | Rockchip GPIO v2 (no pinmux/IRQ) | new |
| `drivers/timer/thead_clint_timer.c` | T-Head CLINT timer (mtime via `time` CSR) | v4.4 `riscv_machine_timer.c` adapted |
| `dts/` | SoC dtsi + bindings | facts from Linux/vendor DTs (see each file's header) |
| `boards/<vendor>/<board>/` | 11 boards, each with a `README.md` (facts, boot/load flow) | new |

Mainline boards used as they are: `stm32mp135f_dk` (STM32MP13), `sun8i_h3` SoC for the NanoPis.

When the Zephyr tree is moved to >= 4.5: drop `drivers/gpio/gpio_allwinner.c` + its bindings (upstream has them) and
check `soc/rockchip/rv1106` / `sun50i_h616` against upstream (`rockchip` family and `sun50i_h618` exist there).

## Support level per SoC (why)

| SoC | Core used | Level | Not done / why |
|---|---|---|---|
| H3 | A7 #0 | mainline SoC + OOT board | SMP, R_PIO (PL pins) |
| H616 | A53 #0 (EL1, from EL2) | OOT (upstream code) | clock driver (relies on U-Boot's UART/PIO clocks) |
| T113-S3 | A7 #0 | OOT | HiFi4 DSP (Xtensa, needs Cadence toolchain) |
| RV1103 | A7 | OOT (upstream backport) | RISC-V MCU core: undocumented; pinmux driver |
| RK3506 | A7 #0 | OOT (from PR) | Cortex-M0 (vendor AMP only); pinmux driver |
| STM32MP13 | A7 | mainline | - |
| CV1800B / SG2002 | little C906, M-mode | OOT (after PR) | big C906 / A53: Zephyr has no S-mode, so it would replace OpenSBI; pinctrl |
| K210 | hart 0, M-mode | OOT | FPIOA pinmux, GPIO, clock control, hart 1 |
| K230 / K230D | CPU0 C908, M-mode | OOT (after PR) | big core CPU1, GPIO pins per carrier |

None of the ports has run on hardware yet (no board attached); each README lists what is unverified.
