# k230d_bpi_zero: Banana Pi BPI-CanMV-K230D-Zero (K230D, 128 MB SiP)

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- Same SoC port as `canmv_k230`; 128 MB in-package DRAM (Zephyr uses 0x0 + 64 MB). Console UART0. No `led0`.

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello k230d_bpi_zero       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/k230d_bpi_zero/zephyr/zephyr.bin
scripts/app bootcmd uart-hello k230d_bpi_zero       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board k230d_bpi_zero        # app from apps/_template_soc
```

## Boot / load

Yocto `k230d-bpi-zero` image's U-Boot: `ext4load mmc 1:1 0x0 zephyr.bin; go 0x0`.

Results: `../../../../BOARDS.md`.
