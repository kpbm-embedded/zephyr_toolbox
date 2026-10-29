# _template_soc: application-SoC app template

Template for apps on the application SoCs (H3, H616, T113, RV1103, RK3506, STM32MP13, CV1800B/SG2002, K210, K230).
Do not build apps in here; make a copy:

```sh
cd <32_03_zephyr>
scripts/app new myapp --board luckfox_lyra     # board list: scripts/app boards
scripts/app build myapp Debug                  # board from the app's CLion profile (-DBOARD=)
scripts/app bootcmd myapp Debug                # image path + U-Boot/loader commands (prints only)
scripts/app test myapp                         # host unit tests (tests/, GoogleTest)
```

`scripts/app new` copies this folder to `apps/<name>`, renames the build dirs and project names in `.idea/` and `.run/`, and
sets `-DBOARD=` in `.idea/cmake.xml`. Without `--board`, or with an MCU board, it copies the MCU template
`apps/zephyr-blinky-blackpill` instead.

| Path | What |
|---|---|
| `CMakeLists.txt` | adds `../../ports` (the out-of-tree board module) to `ZEPHYR_EXTRA_MODULES`, so CLion/cmake need no options |
| `src/main.c` | banner, led0 blink if the board has `led0`, `info` shell command |
| `prj.conf` | printk, GPIO, shell |
| `boards/` | per-board `<board>.overlay` / `<board>.conf` (examples: `nanopi_neo.overlay`, `luckfox_lyra.conf`) |
| `lib/`, `tests/` | host-tested code (same as the MCU template; `Host-Tests` profile) |
| `.idea/cmake.xml` | profiles Debug / Release / Host-Tests, build dirs on idisk03 under `build/apps/<name>/` |
| `.run/` | Build, Boot commands, Monitor (`/dev/ttyUSB0 115200`; Lyra: 1500000), Host tests |

There is no Flash/Debug run configuration: these boards are loaded by U-Boot/FEL/kflash/fiptool (see
`../../ports/boards/<vendor>/<board>/README.md`).
