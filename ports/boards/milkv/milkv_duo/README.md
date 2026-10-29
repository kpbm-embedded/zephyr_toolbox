# milkv_duo: Milk-V Duo 64M (Sophgo CV1800B), little C906 core

Out-of-tree board of the `ports/` Zephyr module in 32_03_zephyr. Status: **builds; not tested on hardware**.

## Facts

- SoC `cv1800b` (series `cv18xx`, this ID), after the unmerged Zephyr PR #69594. Zephyr runs on the **little C906**
  (700 MHz, M-mode, no MMU), the core the vendor SDK gives to FreeRTOS; Linux keeps the big C906.
- PLIC 0x70000000, CLINT mtimecmp 0x74004000 with mtime read from the `time` CSR (this ID's `thead_clint_timer.c`),
  25 MHz. RAM: the SDK's RTOS window 0x83f40000 + 768 KB (keep it reserved in Linux).
- Console: UART0 (shared with U-Boot/Linux; there is no pinctrl driver yet, UART1 would need pinmux). LED: `led0` = GPIOC24.
- Not done: big-core Zephyr (needs OpenSBI-less M-mode boot or S-mode support, which Zephyr lacks).

## Build

```sh
cd <32_03_zephyr>
scripts/app example uart-hello milkv_duo       # or blink/timer/threads-msgq/shell; image: <out>/examples/<ex>/milkv_duo/zephyr/zephyr.bin
scripts/app bootcmd uart-hello milkv_duo       # prints the image and the load commands below (nothing is written)
scripts/app new myapp --board milkv_duo        # app from apps/_template_soc
```

## Boot / load

The FSBL (BL2) starts the little core from the FIP image's `BLCP_2ND` entry. With Sophgo `fiptool.py`
(github.com/sophgo/fiptool, also used by the Yocto build):

```
fiptool.py genfip fip.bin --OLD_FIP=fip.bin --BLCP_2ND=zephyr.bin --BLCP_2ND_RUNADDR=0x83f40000
```
Flag names as in cvitek `fiptool.py` (check `fiptool.py genfip -h`; not run here). Write the new fip.bin to the SD boot partition (this step writes a device; not done here).

Results: `../../../../BOARDS.md`.
