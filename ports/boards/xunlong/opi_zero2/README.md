# opi_zero2: Orange Pi Zero2 (Allwinner H616) and H616/H618 core boards

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC `sun50i_h616` (AArch64): copy of Zephyr main's `sun50i_h618` (H618 = H616 with more L2), plus an `mmu_regions.c`
  that v4.4 needs. GPIO: Zephyr main's Allwinner driver (backport).
- Core: Cortex-A53 #0, entered at EL2 non-secure from U-Boot (TF-A BL31 at 0x40000000), `CONFIG_ARMV8_A_NS=y`.
  GIC-400 0x03021000, timer 24 MHz, UART0 0x05000000. RAM used: 0x40080000.. (511 MB; Zero2 has 512 MB / 1 GB).
- LED: `led0` = PC13 (green). For the user's unknown H616/H618 core boards: check UART0 pins and change `led0`.
- Toolchain: `aarch64-zephyr-elf` (added to the SDK 1.0.1 for this board and `qemu_cortex_a53`).

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello opi_zero2       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/opi_zero2/zephyr/zephyr.bin
scripts/app bootcmd uart-hello opi_zero2       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board opi_zero2        # app from apps/_template_soc
```

## Boot / load

Mainline U-Boot (meta-sunxi orange-pi-zero2):

```
=> fatload mmc 0:1 0x40080000 zephyr.bin
=> dcache flush; go 0x40080000
```

Results: `../../../../BOARDS.md`.
