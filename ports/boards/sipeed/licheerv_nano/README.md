# licheerv_nano: Sipeed LicheeRV Nano (Sophgo SG2002), little C906 core

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC `sg2002` (series `cv18xx`, same peripherals as CV1800B). Little C906, RAM window 0x8fe00000 + 2 MB
  (RTOS window of the 256 MB SDK memory map, as in PR #69594's duo256m board).
- Console: UART0. No `led0`: the user LED pin is not confirmed in the sources here, so blink is not offered.

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello licheerv_nano       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/licheerv_nano/zephyr/zephyr.bin
scripts/app bootcmd uart-hello licheerv_nano       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board licheerv_nano        # app from apps/_template_soc
```

## Boot / load

As for the Duo, with `--BLCP_2ND_RUNADDR=0x8fe00000`. The Yocto `licheerv-nano` image uses Sophgo's prebuilt FSBL + fiptool.

Results: `../../../../BOARDS.md`.
