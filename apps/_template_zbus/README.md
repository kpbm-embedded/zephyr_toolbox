# __name__

Zephyr app built on zbus (from `apps/_template_zbus`: `scripts/app new <name> --template zbus`).
Modules talk only through channels: `lib/bus/` defines them, `src/main.c` publishes readings, a listener
keeps the latest value and a subscriber thread reacts to commands.

| Path | What |
|---|---|
| `lib/bus/` | channels (`reading_chan`, `cmd_chan`), validator, listener, subscriber thread |
| `src/main.c` | publishes a reading every 500 ms, prints what the observers saw |
| `tests/ztest/` | ztest suite for `lib/bus` (twister, qemu) |

```sh
scripts/app ztest __name__                      # tests on qemu_cortex_m3
scripts/app build __name__ Debug qemu_cortex_m3 # the app
```

More zbus features (message subscribers, runtime observers): `examples/zephyr/zbus-pubsub`.
