# nanopi_duo2: FriendlyElec NanoPi Duo2 (Allwinner H3)

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- Same SoC support as `nanopi_neo` (mainline `sun8i_h3` + this ID's GPIO banks). DRAM 512 MB at 0x40000000.
- Console: UART0 (pins on the 2x16 header, 115200). LED: `led0` = PA10 (green status); PL10 (red power) is on R_PIO (not described).

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello nanopi_duo2       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/nanopi_duo2/zephyr/zephyr.bin
scripts/app bootcmd uart-hello nanopi_duo2       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board nanopi_duo2        # app from apps/_template_soc
```

## Boot / load

As for the NEO: `fatload mmc 0:1 0x40000000 zephyr.bin; go 0x40000000` from the Yocto `nanopi-duo2` image's U-Boot.

Results: `../../../../BOARDS.md`.
