# __name__

Twister test-plan template (from `apps/_template_testplan`: `scripts/app new <name> --template testplan`): one app,
two kinds of twister tests, and the YAML keys most test plans need.

| Path | What |
|---|---|
| `lib/crc/` | the code under test (CRC-16/CCITT) |
| `src/main.c` + `sample.yaml` | the app as a test: twister boots it on qemu and matches its console output (`harness: console`, ordered multi-line regex) |
| `tests/ztest/testcase.yaml` | unit tests in variants: plain, `extra_configs` (asserts off, speed-optimised), `filter` (RISC-V only), `extra_args` (an extra conf file) |

```sh
scripts/app twister -T <app> -p qemu_cortex_m3 -p qemu_riscv32   # every scenario of the app (console + unit)
scripts/app twister -T <app> --list-tests                         # the test cases twister found
scripts/app twister -T <app> -p qemu_cortex_m3 --save-tests plan.json   # write the plan; --load-tests plan.json reruns it
scripts/app ztest __name__                                        # only tests/ztest (default qemu_cortex_m3)
```

Keys used: `tags`, `timeout`, `platform_allow`, `integration_platforms`, `extra_configs`, `extra_args`, `filter`,
`harness`/`harness_config`. A failing test case or a missing console line fails the run (exit 1).
