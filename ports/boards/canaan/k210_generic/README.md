# k210_generic: Kendryte K210 generic (Sipeed Maix Bit/Dock/Go class)

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC `k210` (this ID). Zephyr never had K210 support; the port is minimal: RV64GC hart 0 in M-mode, CLINT 0x02000000
  (mtime 7.8 MHz = 390 MHz / 50), PLIC 0x0C000000, UARTHS 0x38000000 (SiFive UART driver, 390 MHz clock), 6 MB SRAM.
- The K210 implements privileged spec 1.9.1: PMP is off and nothing writes `mcounteren`; FPU is not enabled.
- Not done: GPIO/blink (GPIOHS is SiFive-compatible but every pin goes through the FPIOA mux; no driver), clocks
  (assumes the boot ROM's 390 MHz), hart 1 (parked by Zephyr's reset code).

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello k210_generic       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/k210_generic/zephyr/zephyr.bin
scripts/app bootcmd uart-hello k210_generic       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board k210_generic        # app from apps/_template_soc
```

## Boot / load

Over the board's USB-UART with kflash (ISP mode; `--sram` loads into SRAM and runs, nothing is flashed):

```
kflash.py -p /dev/ttyUSB0 -b 1500000 --sram zephyr.bin
```
Flash persistently (writes the SPI flash; ask first): `kflash.py -p /dev/ttyUSB0 -b 1500000 zephyr.bin`.

Results: `../../../../BOARDS.md`.
