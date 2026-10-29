# __name__

Zephyr app with persistent configuration: settings subsystem on NVS (from `apps/_template_settings`:
`scripts/app new <name> --template settings`). A boot counter and a name survive resets.

| Path | What |
|---|---|
| `lib/cfg/` | `cfg` struct, settings handler for the `cfg/` subtree, `cfg_load()`, `cfg_save()` |
| `src/main.c` | loads, increments `cfg/boots`, saves, prints |
| `boards/qemu_*.overlay` | qemu only: a simulated flash with `storage_partition`. Real boards use the `storage_partition` of their DTS (BlackPill F411, WeAct G431/H562/H743, Pico, nRF52840 all have one) |
| `tests/ztest/` | save/load round trip and defaults on a fresh partition |

```sh
scripts/app ztest __name__
scripts/app build __name__ Debug qemu_cortex_m3
scripts/app build __name__ Debug blackpill_f411ce   # the board's own flash partition
```
