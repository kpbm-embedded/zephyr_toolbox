# canmv_k230: CanMV-K230 class boards (Canaan K230: 01Studio, Canaan, LCKFB), CPU0

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC `k230` (this ID), after the unmerged Zephyr PR #74169. Zephyr runs on CPU0 (C908, the hart U-Boot runs on) in
  M-mode: the vendor U-Boot is M-mode, so `go` works without OpenSBI.
- PLIC 0xF_0000_0000, CLINT 0xF_0400_0000 (mtimecmp +0x4000, mtime via `time` CSR, 27 MHz), UART0 0x91400000 (50 MHz).
  RAM: 0x0 + 64 MB.
- Console: UART0 (the debug UART U-Boot/Linux use; PR #74169 used UART3, the RT-Smart console). No `led0` (board LED
  pins differ per carrier and are unconfirmed). GPIO0/1 nodes (DesignWare, 0x9140B000/0x9140C000) are disabled.
- Not done: big core (CPU1, 1.6 GHz + RVV) - it is released from reset by software on CPU0; possible later.

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello canmv_k230       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/canmv_k230/zephyr/zephyr.bin
scripts/app bootcmd uart-hello canmv_k230       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board canmv_k230        # app from apps/_template_soc
```

## Boot / load

Vendor U-Boot 2022.10 of the Yocto `k230-canmv-01studio` image (boot ext4 partition p1):

```
=> ext4load mmc 1:1 0x0 zephyr.bin      # SD card = mmc 1 on CanMV boards
=> go 0x0
```

Results: `../../../../BOARDS.md`.
