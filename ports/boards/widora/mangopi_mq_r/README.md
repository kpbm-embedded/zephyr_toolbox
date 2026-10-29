# mangopi_mq_r: MangoPi MQ-R (Allwinner T113-S3)

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC `sun8i_t113` (this ID): 2x Cortex-A7, GIC-400 0x03021000, timer 24 MHz, UART0-5 0x02500000 + n*0x400,
  PIO 0x02000000 (D1 layout: banks 0x30 apart, pull at +0x24 -> `allwinner,pull-offset`), 128 MB DDR3 at 0x40000000
  (Zephyr uses the first 64 MB). The HiFi4 DSP is not used.
- Early init (`ports/soc/common/armv7a_uboot_go.c`): cleans/disables U-Boot's MMU/caches, VBAR -> Zephyr vectors
  (address 0 is the BROM, so nothing is copied there).
- Console: UART3 (PB6/PB7 debug pads on the MQ-R, as in Linux), 115200. LED: `led0` = PD22 (blue, active low).
- The other T113 boards (Ebyte ECK31, ECK30 T113-i, FET113i-S): same SoC; change the console UART and `led0`.

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello mangopi_mq_r       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/mangopi_mq_r/zephyr/zephyr.bin
scripts/app bootcmd uart-hello mangopi_mq_r       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board mangopi_mq_r        # app from apps/_template_soc
```

## Boot / load

U-Boot from the Yocto `mangopi-mq-r-t113` image:

```
=> fatload mmc 0:1 0x40000000 zephyr.bin
=> go 0x40000000
```
Without an SD card, over USB FEL (sunxi-tools on the host; writes RAM only): `sunxi-fel -p uboot u-boot-sunxi-with-spl.bin write 0x40000000 zephyr.bin`, then `go 0x40000000` at the U-Boot prompt.

Results: `../../../../BOARDS.md`.
