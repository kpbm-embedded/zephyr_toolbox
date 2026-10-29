# luckfox_lyra: Luckfox Lyra / Lyra Plus (Rockchip RK3506G2)

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC `rk3506`: from the unmerged Zephyr PR #110923 (Forlinx OK3506B-S12, tested there on hardware with U-Boot `go`),
  as a separate series here; plus timer frequency and GPIO v2 banks.
- Core: Cortex-A7 #0 (of 3), GIC-400 0xff581000, timer 24 MHz, 128 MB DDR; Zephyr runs in 0x03e00000..+1 MB.
- Console: UART0 0xff0a0000 at **1500000** baud (the Lyra's debug UART / fiq-debugger speed). LED: `led0` = GPIO1_A0.
- The RK3506's Cortex-M0 is not used (vendor AMP flow only).

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello luckfox_lyra       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/luckfox_lyra/zephyr/zephyr.bin
scripts/app bootcmd uart-hello luckfox_lyra       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board luckfox_lyra        # app from apps/_template_soc
```

## Boot / load

Luckfox Lyra U-Boot (Yocto `luckfox-lyra`), at 1.5 Mbaud:

```
=> fatload mmc 0:1 0x03e00000 zephyr.bin       # or: loadz 0x03e00000 / tftp 0x03e00000 zephyr.bin
=> dcache flush 0x03e00000 ${filesize}
=> go 0x03e00000
```

Results: `../../../../BOARDS.md`.
