# zephyr / dt-kconfig

App configuration through devicetree and Kconfig.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- an app-local binding (`dts/bindings/acme,thermostat.yaml`; the app dir is a `DTS_ROOT`) with required, default and enum properties
- `app.overlay` adds nodes; `status = "disabled"` keeps one out; a table built with `DT_INST_FOREACH_STATUS_OKAY`
- app Kconfig options (`Kconfig` with `source "Kconfig.zephyr"`), one defaulting from devicetree (`$(dt_nodelabel_enabled,zone_server)`)
- three twister variants of one source: default, `extra_configs`, `EXTRA_DTC_OVERLAY_FILE=third.overlay` (18 cases in total)

## Run
```
../../../scripts/app ztest dt-kconfig                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest dt-kconfig qemu_riscv32
../../../scripts/app debug dt-kconfig qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/dt-kconfig/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
Configuration only; board files go in `boards/<board>.overlay` and `boards/<board>.conf`.

## Expected output
`18 of 18 executed test cases passed` in the twister summary (3 variants x 6 cases).
