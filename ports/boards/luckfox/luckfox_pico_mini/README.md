# luckfox_pico_mini: Luckfox Pico Mini A/B, Pico (Rockchip RV1103)

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC `rv1106`: backport of Zephyr main `soc/rockchip/rv1106` + `rv1106.dtsi` (PR #103787, board `luckfox/pico_ultra`),
  with this ID's common early init and GPIO v2 banks + driver (`gpio_rockchip_v2.c`, no pinmux).
- Core: Cortex-A7, GIC-400 0xff1f1000, timer 24 MHz, DDR 64 MB at 0x0 (Zephyr is linked at 0x0, as upstream).
- Console: UART2 0xff4c0000 (the board's debug UART, 115200). LED: `led0` = GPIO3_C6 (work LED, Luckfox SDK DT).
- The RV1103's small RISC-V MCU core is not used (no public docs/boot flow; Luckfox uses it only for fast boot).

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello luckfox_pico_mini       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/luckfox_pico_mini/zephyr/zephyr.bin
scripts/app bootcmd uart-hello luckfox_pico_mini       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board luckfox_pico_mini        # app from apps/_template_soc
```

## Boot / load

Rockchip/Luckfox U-Boot. Upstream (pico_ultra doc) needs two U-Boot changes: `do_go_exec()` must not set the Thumb bit
(`entry | 1` under `CONFIG_CPU_V7`) and `CONFIG_CMD_CACHE=y`. Then:

```
=> dcache off; icache off
=> fatload mmc 1:1 0x0 zephyr.bin     # or: tftp 0x0 zephyr.bin
=> go 0x0
```

Results: `../../../../BOARDS.md`.
