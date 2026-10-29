# zephyr / settings-nvs

Settings subsystem on NVS, on a simulated flash partition.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- `SETTINGS_STATIC_HANDLER_DEFINE` with set/get/commit/export for an `app/` subtree
- `settings_save_one`, `settings_save` (export all), `settings_load`, `settings_load_subtree`, `settings_delete`, runtime get/set
- NVS garbage collection: 600 writes into a 16 KiB partition of 1 KiB sectors
- `app.overlay`: flash simulator (`zephyr,sim-flash`) with `storage_partition`, chosen as `zephyr,settings-partition`

## Run
```
../../../scripts/app ztest settings-nvs                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest settings-nvs qemu_riscv32
../../../scripts/app debug settings-nvs qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/settings-nvs/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
Remove `app.overlay`: BlackPill F411, WeAct G431/H562/H743, Pico and the nRF52840 boards have a `storage_partition` in their DTS.

## Expected output
`6 of 6 executed test cases passed` in the twister summary.
