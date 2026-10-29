# _template_ztest

Template of a Zephyr app that ships with a ztest suite. Copy it with
`scripts/app new <name> --ztest [--board <b>]`; this folder itself is not built.

| Path | What |
|---|---|
| `src/main.c` | the app (uses `lib/counter`) |
| `lib/counter/` | the module under test: replace with your own code |
| `tests/ztest/` | ztest image: `lib/` + `src/test_counter.c`, `testcase.yaml` for twister |

```sh
scripts/app build <name> Debug qemu_cortex_m3     # the app
scripts/app ztest <name>                          # the tests on qemu_cortex_m3 (twister)
scripts/app ztest <name> qemu_riscv32 --coverage  # another qemu board, with gcov coverage
scripts/app debug <name> qemu_cortex_m3           # the app under gdb on qemu
```

Mocks: see `examples/zephyr/ztest-fff` (FFF). Shell-level tests: `examples/zephyr/pytest-shell` (pytest harness)
and the Robot Framework tool `32_30_robot_framework` (Renode, qemu, serial).
