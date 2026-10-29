# nanopi_neo: FriendlyElec NanoPi NEO / NEO Core (Allwinner H3)

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC support: Zephyr v4.4 mainline `sun8i_h3` (SoC of `xunlong/opi_zero`); this ID adds only the board and the GPIO banks
  (`ports/dts/arm/allwinner/sun8i-h3-gpio.dtsi`, driver backported from Zephyr main).
- Core: Cortex-A7 #0 (AArch32, SVC), GIC-400 0x01c81000, generic timer 24 MHz, DRAM 0x40000000 (256 MB on NEO 256M).
- Console: UART0 0x01c28000 (debug header, 115200 8N1). LED: `led0` = PA10 (blue status).
- Fix vs mainline: `CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC=24000000` in the defconfig (mainline sun8i_h3 takes the CPU clock,
  which would make every delay 40x too short on this board).

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello nanopi_neo       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/nanopi_neo/zephyr/zephyr.bin
scripts/app bootcmd uart-hello nanopi_neo       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board nanopi_neo        # app from apps/_template_soc
```

## Boot / load

Mainline U-Boot (the Yocto `nanopi-neo` image has it, SPL at 8 KiB of the SD card). Copy `zephyr.bin` to the FAT/ext4
boot partition, stop autoboot and:

```
=> fatload mmc 0:1 0x40000000 zephyr.bin      # ext4: ext4load mmc 0:1 ...
=> go 0x40000000
```
U-Boot's `go` stays in secure SVC with MMU/caches as U-Boot left them; the mainline SoC init copes (opi_zero flow).

Results: `../../../../BOARDS.md`.
