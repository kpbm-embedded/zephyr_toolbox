# zephyr / littlefs-flashsim

LittleFS on a simulated flash partition, mounted from a devicetree fstab entry.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- `zephyr,fstab,littlefs` with `automount`: mounted at `/lfs` during boot, formatted on first use
- write/read, append, `fs_stat`, directories, rename, unlink, a file spanning several blocks
- unmount and mount again: data survives
- `fs_statvfs` shows the geometry of the simulated flash (32 blocks of 1 KiB)

## Run
```
../../../scripts/app ztest littlefs-flashsim                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest littlefs-flashsim qemu_riscv32
../../../scripts/app debug littlefs-flashsim qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/littlefs-flashsim/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
Point the fstab `partition` at real flash (internal, or SPI NOR on the WeAct H562/H743 boards) and drop the simulator node.

## Expected output
`6 of 6 executed test cases passed` in the twister summary.
