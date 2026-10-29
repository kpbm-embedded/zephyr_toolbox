# Application-SoC boards: support matrix (2026-10-25)

Zephyr v4.4.2 + this ID's `ports/` module. Board list, examples, boot commands: `examples/zephyr/boards.yaml`
(`scripts/app boards`). Per-board facts and boot/load flow: `ports/boards/<vendor>/<board>/README.md`.

No board was attached (lsusb: no USB-serial/probe; no /dev/ttyUSB*, /dev/ttyACM*), so **nothing ran on hardware**.
"Result" = build + checks below; run tests are on the qemu stand-ins.

| Board | SoC | Core used | Level | Examples built (all OK) | Test method | Result |
|---|---|---|---|---|---|---|
| nanopi_neo | Allwinner H3 | Cortex-A7 #0 | mainline SoC `sun8i_h3`, OOT board | uart-hello, timer, threads-msgq, shell, blink | build + twister build + ELF + qemu_cortex_a9 proxy | builds; HW untested |
| nanopi_duo2 | Allwinner H3 | Cortex-A7 #0 | mainline SoC, OOT board | same 5 | same | builds; HW untested |
| opi_zero2 | Allwinner H616 | Cortex-A53 #0 (AArch64) | OOT (Zephyr main H618 code) | same 5 | build + twister + ELF + qemu_cortex_a53 proxy | builds; HW untested |
| mangopi_mq_r | Allwinner T113-S3 | Cortex-A7 #0 | OOT | same 5 | build + twister + ELF + qemu_cortex_a9 | builds; HW untested |
| luckfox_pico_mini | Rockchip RV1103 | Cortex-A7 | OOT (backport of Zephyr main rv1106) | same 5 | same | builds; HW untested |
| luckfox_lyra | Rockchip RK3506G2 | Cortex-A7 #0 | OOT (Zephyr PR #110923) | same 5 | same | builds; HW untested |
| stm32mp135f_dk | ST STM32MP135F | Cortex-A7 | **mainline** | same 5 + button-irq | build + twister + ELF + qemu_cortex_a9 | builds; HW untested |
| milkv_duo | Sophgo CV1800B | little C906 (M-mode) | OOT (after PR #69594) | same 5 | build + twister + ELF + qemu_riscv64 | builds; HW untested |
| licheerv_nano | Sophgo SG2002 | little C906 (M-mode) | OOT (after PR #69594) | 4 core (no led0: LED pin unconfirmed) | same | builds; HW untested |
| k210_generic | Kendryte K210 | hart 0 (M-mode, priv 1.9.1) | OOT | 4 core (no GPIO: FPIOA mux, no driver) | same | builds; HW untested |
| canmv_k230 | Canaan K230 | CPU0 C908 (M-mode) | OOT (after PR #74169) | 4 core (no led0) | same | builds; HW untested |
| k230d_bpi_zero | Canaan K230D | CPU0 C908 (M-mode) | OOT | 4 core (no led0) | same | builds; HW untested |

Not offered on any of these boards (no Zephyr driver for the SoC here): pwm-fade, adc-read, i2c-scan, usb-cdc,
ble-beacon, settings (flash). Not feasible / not done: H3/H616 SMP; T113 HiFi4 DSP; RV1103 RISC-V MCU core
(undocumented); RK3506 Cortex-M0 (vendor AMP only); CV18xx big core (Zephyr has no S-mode, would have to replace
OpenSBI); K230 big core CPU1; K210 hart 1, FPIOA/GPIO, clock control.

## Runs and checks

- **Build matrix** `scripts/app matrix` (-j8, RAM guard): 69/69 OK = 57 hardware-board builds + 12 qemu builds.
  Results: `$ZEPHYR_OUT/BOARDS.tsv`.
- **Twister build-only** (`west twister -p <12 boards> --board-root ports/boards -T examples/zephyr --build-only
  --tag soc_ports`, uses the new `sample.yaml` files): 57/57 built, 0 errors, 0 warnings (after adding
  `ports/dts/bindings/vendor-prefixes.txt`).
- **qemu stand-ins** (twister with console harness): 12/12 passed — uart-hello, timer, threads-msgq, shell on
  qemu_cortex_a9 (ARMv7-A proxy for the A7 SoCs), qemu_cortex_a53 (H616), qemu_riscv64 (C906/C908/K210).
  The shell was also driven over stdin (`demo hello zephyr`, `kernel uptime`): answers on all three
  (`build/qemu_runs/`). Zephyr v4.4 has no `qemu_cortex_a7`; qemu cannot model these SoCs themselves.
- **ELF/image check** (uart-hello): every image is linked inside its board's RAM window, the binary starts at
  the load address used in the boot commands, and its first word is a branch to the reset code
  (ARM `ldr pc,[pc,#24]`, AArch64 `b`, RISC-V entry = load address), so U-Boot `go <load>` hits the vector:

| Board | Load / entry | RAM window (zephyr,sram) | bin |
|---|---|---|---|
| nanopi_neo / duo2 | 0x40000000 / 0x40002600 | 0x40000000 +256 MB / +512 MB | 24.7 KB |
| opi_zero2 | 0x40080000 / 0x400810b4 | 0x40080000 +511 MB | 37.0 KB |
| mangopi_mq_r | 0x40000000 / 0x400026f8 | 0x40000000 +64 MB | 24.7 KB |
| luckfox_pico_mini | 0x0 / 0x26f8 | 0x0 +64 MB | 24.7 KB |
| luckfox_lyra | 0x03e00000 / 0x03e026f8 | 0x03e00000 +1 MB | 28.8 KB |
| stm32mp135f_dk | 0xc0000000 / 0xc0002704 | code 0xc0000000, data 0xd0000000 | 45.3 KB |
| milkv_duo | 0x83f40000 = entry | 0x83f40000 +768 KB | 13.5 KB |
| licheerv_nano | 0x8fe00000 = entry | 0x8fe00000 +2 MB | 13.5 KB |
| k210_generic | 0x80000000 = entry | 0x80000000 +6 MB | 11.6 KB |
| canmv_k230 / k230d_bpi_zero | 0x0 = entry | 0x0 +64 MB | 14.6 KB |

- Drivers selected as intended (`.config`): `GPIO_ALLWINNER` (H3/H616/T113), `GPIO_ROCKCHIP_V2` (RV1103/RK3506),
  `GPIO_DW` (CV1800B), `THEAD_CLINT_TIMER` (CV18xx, K230), `RISCV_MACHINE_TIMER` (K210).
- Regression: MCU app `zephyr-blinky-blackpill` Release still builds; template copies (`app new`, both
  templates) built, host tests 3/3, and a plain `cmake` build of `apps/_template_soc` (no ZEPHYR_EXTRA_MODULES
  in the environment, as CLion does it) builds for mangopi_mq_r.

## Unverified on hardware (check first when a board is attached)

U-Boot `go` behaviour (caches/MMU/security state) on each board; console UART pins/baud (Lyra 1.5 Mbaud);
LED pins and that their pins are in GPIO function after U-Boot (no pinmux drivers); K210 CPU clock (390 MHz
assumed, sets UARTHS divisor and timer); CV18xx little-core start via FIP `BLCP_2ND` and the UART0 sharing with
Linux; K230 load at 0x0 with the vendor U-Boot; RV1103 needs the Luckfox U-Boot `go` Thumb-bit patch.
Reading the console is safe: `scripts/app monitor /dev/ttyUSB0 115200`. Loading/flashing writes RAM or media
and is not done without asking; commands: `scripts/app bootcmd <example> <board>`.
