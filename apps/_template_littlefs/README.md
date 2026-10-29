# __name__

Zephyr data logger on LittleFS (from `apps/_template_littlefs`: `scripts/app new <name> --template littlefs`):
fixed-size records appended to `/lfs/log.bin`, rotated to `/lfs/log.old` at a size limit, so the newest data
always fits. The file system is mounted at boot from a devicetree fstab entry.

| Path | What |
|---|---|
| `lib/datalog/` | `datalog_append()`, `datalog_count()`, `datalog_read()`, rotation |
| `src/main.c` | logs a record every 100 ms, prints counts |
| `boards/qemu_*.overlay` | qemu: simulated flash, 32 KiB partition, fstab `/lfs` (automount) |
| `tests/ztest/` | append/read back, rotation, survives unmount/mount |

On a board give the fstab entry a real partition (e.g. the last 64 KiB of internal flash, or an SPI NOR on
the WeAct H562/H743 boards) in `boards/<board>.overlay`.

```sh
scripts/app ztest __name__
scripts/app build __name__ Debug qemu_cortex_m3
```
