# __name__

Zephyr app with its own shell command group (from `apps/_template_shell`: `scripts/app new <name> --template shell`).
The commands live in `lib/cmds/` and are tested in ztest through the shell's dummy backend (no UART, no pytest).

| Path | What |
|---|---|
| `lib/cmds/` | `app status`, `app gain <0..100>`, `app start`, `app stop` (argument checks, error codes) |
| `src/main.c` | shell on the console UART (`uart:~$`) |
| `tests/ztest/` | runs commands with `shell_execute_cmd()` on the dummy backend and checks output and return codes |

```sh
scripts/app ztest __name__
scripts/app build __name__ Debug qemu_cortex_m3
scripts/app run ...   # interactive: west build -t run in the build dir, then type `app status`
```

The same commands from the host over the console: `examples/zephyr/pytest-shell` (twister pytest harness) or
Robot Framework (`32_30_robot_framework`).
