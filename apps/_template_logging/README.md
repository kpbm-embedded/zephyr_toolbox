# __name__

Zephyr app set up for logging (from `apps/_template_logging`: `scripts/app new <name> --template logging`):
one log module per source file, its level a Kconfig option (`CONFIG_APP_CORE_LOG_LEVEL`, menuconfig: Application),
deferred logging on the UART, a log shell command to change levels at run time.

| Path | What |
|---|---|
| `Kconfig` | `APP_CORE_LOG_LEVEL` from Zephyr's `Kconfig.template.log_config` |
| `lib/core/` | module `app_core`: logs with `LOG_INF/WRN/ERR/HEXDUMP` |
| `prj.conf` | deferred mode, timestamps, `log` shell command (`log enable dbg app_core`) |
| `tests/ztest/` | a capture backend checks which messages reach the backends at which level |

```sh
scripts/app ztest __name__
scripts/app build __name__ Debug qemu_cortex_m3
west build -d <build> -t menuconfig    # Application -> log level
```

Custom backends and filtering in detail: `examples/zephyr/log-backend`.
