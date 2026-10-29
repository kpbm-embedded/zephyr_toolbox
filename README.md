# 32_03_zephyr - Zephyr RTOS

Zephyr v4.4.2 (west workspace) with Zephyr SDK 1.0.1 GNU toolchains (arm, riscv64, xtensa esp32/esp32s3) and SDK hosttools
(dtc, openocd). Use it to build, test and flash Zephyr apps and examples for the owned MCU boards and, through the `ports/`
module, for the application SoCs of the Yocto boards (Allwinner H3/H616/T113, Rockchip RV1103/RK3506, ST STM32MP13, Sophgo
CV1800B/SG2002, Kendryte K210, Canaan K230).

Status (2026-10-29): **ready** for building and qemu runs; 28 examples (16 with ztest/pytest/console tests that pass
on qemu), 16 app templates (`scripts/app new --template <kind>`), Rust and Zig through the toolchains of 32_17/32_24;
**nothing has run on real hardware** (no board attached).
**Self-contained:** it uses only this folder, plus its build output on idisk03 at the same ID path. A fresh clone installs
its toolchain with `scripts/app setup` (pinned, sha256-checked; see [Fresh clone](#fresh-clone)).

## What's included

| Component | Version | Notes |
|---|---|---|
| Zephyr | v4.4.2 | west workspace `toolchain/zephyr/ws` |
| Zephyr SDK | 1.0.1 | arm, riscv64, xtensa esp32/esp32s3, plus `aarch64-zephyr-elf` (H616, qemu_cortex_a53) |
| west | v1.5.0 | in `.venv` with cmake, ninja, esptool; CPython 3.13 |
| SDK openocd | 0.12.0 (2026-03-25) | needs `shutdown`, not `exit`, before init |
| GoogleTest | 1.18.0 | host unit tests, `toolchain/clion` |
| `ports/` module | 11 out-of-tree boards, 7 SoCs | matrix and results: [BOARDS.md](BOARDS.md) |
| Apps / examples | `zephyr-blinky-blackpill` + 15 `apps/_template_*`; 28 examples | tables below |

Boards built: BlackPill F411, WeAct G431/H562/H743, Pico/Pico 2, nRF52840 Pro Micro, ESP32-C3/S3, CH32V003, and the SoC boards in BOARDS.md.

## Fresh clone

```sh
git clone <repo> zephyr-box && cd zephyr-box
scripts/app setup            # ~4 GB download, ~11 GB installed under toolchain/ (all gitignored); rerun to finish
scripts/app doctor           # tools, versions, venv
tests/smoke.sh && tests/examples.sh && tests/templates.sh
```

`scripts/app setup` (= `scripts/setup`) installs, each pinned and checked: Yocto buildtools-extended 6.0.4 (git, make, gcc),
CMake 4.4.4, ninja 1.13.2, uv 0.13.5 into `toolchain/host`; uv CPython 3.13.17 and the venv (`scripts/requirements.lock`:
west 1.5.0 + Zephyr's requirements, 168 packages, every file sha256-pinned) into `toolchain/zephyr`; Zephyr v4.4.2 (tag
commit checked) with `west update --narrow` depth 1 and the hal_espressif blobs; Zephyr SDK 1.0.1 minimal + hosttools +
arm, aarch64, riscv64, xtensa esp32/esp32s3 toolchains (sha256 from the release); GoogleTest 1.18.0; zephyr-lang-rust at
a pinned commit (`modules/lang-rust`); the CLion files
of `apps/` (`*.in` -> files with this checkout's paths). Needs only bash, curl, tar, xz and sha256sum from the OS.
`--check` shows what is missing without changing anything; `--only host,python,west,sdk,gtest,rust,ide` runs single steps.
Downloads stay in `cache/` (`ZEPHYR_CACHE`); `ZEPHYR_WEST_CACHE=<other west workspace>` clones the modules from a local copy.
Not done by setup: `setup.sh -c` of the SDK (it would register a CMake package in `~/.cmake`; `ZEPHYR_SDK_INSTALL_DIR` is
used instead), the udev rules (need sudo), the RUNPATH patch of the SDK gdb (it uses the OS libncursesw).

## Quick start

```sh
cd $BOX/30_39_tools/32_embedded/32_03_zephyr
scripts/app help
scripts/app build zephyr-blinky-blackpill Release       # -> $ZEPHYR_OUT/apps/.../zephyr/zephyr.{elf,bin,hex}
scripts/app example blink rpi_pico                      # an example for another board
scripts/app run hello qemu_x86                          # run on qemu, no hardware needed
scripts/app flash zephyr-blinky-blackpill Debug         # with a board attached
```

## Commands

`scripts/app <command> [args]`. The board is: argument > `$BOARD` > the app's CLion profile (`-DBOARD=` in `.idea/cmake.xml`) > `blackpill_f411ce`.

| Command | What it does | Example |
|---|---|---|
| `build <app> [Debug\|Release] [board]` | build to `$ZEPHYR_OUT/apps/<app>/cmake-build-<profile>/zephyr/zephyr.{elf,bin,hex}` | `scripts/app build zephyr-blinky-blackpill Release` |
| `test <app>` | host unit tests (`tests/`, vendored GoogleTest, CTest) | `scripts/app test zephyr-blinky-blackpill` |
| `flash <app> [Debug\|Release]` | `west flash` (board's default runner; openocd from the SDK) | `scripts/app flash zephyr-blinky-blackpill Debug` |
| `example <example> [board]` | build `examples/zephyr/<example>` to `$ZEPHYR_OUT/examples/<example>/<board>/` | `scripts/app example blink rpi_pico` |
| `monitor [port] [baud]` | serial console (default `/dev/ttyUSB0` 115200) | `scripts/app monitor /dev/ttyACM0 115200` |
| `new <name> [--board <b>] [--template <kind>] [--dir <parent>]` | new `apps/<name>` (or `<parent>/<name>`): no template: MCU board -> `zephyr-blinky-blackpill`, SoC board -> `_template_soc`; `--template <kind>` -> `apps/_template_<kind>` (`__name__` replaced); `--ztest` = `--template ztest` | `scripts/app new mybus --template zbus` |
| `templates` | template kinds with a one-line description | `scripts/app templates` |
| `setup [--check] [--only <steps>]` | install the pinned toolchain into `toolchain/` (fresh clone; see above) | `scripts/app setup --check` |
| `doctor` | check tools, versions, Zephyr tag, venv (works before setup too: lists missing steps) | `scripts/app doctor` |
| `boards` | application-SoC boards, examples, proxies, boot commands | `scripts/app boards` |
| `bootcmd <app\|example> [Debug\|Release\|board]` | image + U-Boot/FEL/kflash/fiptool commands (prints only) | `scripts/app bootcmd blink luckfox_lyra` |
| `matrix [board ...]` | build every supported example x board (+ qemu proxies) -> `BOARDS.tsv` | `scripts/app matrix` |
| `run <example> <qemu board>` | build and run on qemu (`west build -t run`) | `scripts/app run hello qemu_x86` |
| `ztest <test> [board] [--coverage] [args]` | a ztest/pytest suite (example, app's `tests/ztest`, or folder) under twister; default `qemu_cortex_m3`; `--coverage`: gcovr html/txt of this ID's sources (`qemu_riscv32`, `qemu_riscv64`, `mps2/an385`) | `scripts/app ztest ztest-basic` |
| `twister [args]` | `west twister` with `ports/boards`, output in `$ZEPHYR_OUT/twister/run` | `scripts/app twister -p qemu_cortex_m3 -T examples/zephyr --tag ztest` |
| `debug <app\|example> [board] [--server\|--batch]` | debug build (`-Og`, thread info) + SDK gdb: qemu through its gdb stub (:1234), hardware through `west debug` (openocd); `--batch`: stop at `main`, backtrace, quit | `scripts/app debug ztest-basic qemu_cortex_m3 --batch` |
| `where` | key paths: ID root, toolchain, `ZEPHYR_BASE`, build output, apps, examples, ports | `scripts/app where` |
| `help` | command overview (also `-h`, `--help`) | `scripts/app help` |

`<app>` is a folder under `apps/` or a path. A `/` in a qualified board name (`soc/cpu cluster`) becomes `_` in example build
dirs (`example`, `bootcmd`, `matrix` and `run` use the same dir). Usage errors (missing app/example, bad profile, unknown
command) exit 2. From anywhere: `box run zephyr <command> ...` runs `scripts/app` with the same arguments.

## Examples and templates

Examples live in `examples/zephyr/<ex>/` (`README.md`, `example.toml` for `mcu`, `testcase.yaml` or `sample.yaml` for
twister); `examples/zephyr/boards.yaml` is the SoC board table read by `scripts/app`. Build one: `scripts/app example <ex> [board]`.

| Example | Shows | Check (no hardware) |
|---|---|---|
| blink, button-irq, uart-hello, pwm-fade, adc-read, i2c-scan, timer, threads-msgq, shell, settings, usb-cdc, ble-beacon | peripherals on the owned boards | build (`scripts/app matrix`, mcu build check); qemu runs for uart-hello, timer, threads-msgq, shell |
| ztest-basic | ztest: fixtures, zassert/zexpect, skip, shuffle | `scripts/app ztest ztest-basic` |
| ztest-fff | FFF fakes | ztest |
| pytest-shell | twister pytest harness against the shell | ztest (pytest) |
| zbus-pubsub | zbus: validator, listener, subscriber, message subscriber, runtime observer | ztest, 6 cases |
| log-backend | logging levels, runtime filter per backend, hexdump, custom capture backend | ztest, 5 |
| settings-nvs | settings on NVS on a simulated flash partition, export/load/delete, NVS GC | ztest, 6 |
| smf-pump | hierarchical state machine: parent state, initial transition, propagation | ztest, 7 |
| workq-timers | system/own work queue, delayable work debouncer, periodic/one-shot timers | ztest, 6 |
| littlefs-flashsim | LittleFS on simulated flash, fstab automount, dirs, rename, remount | ztest, 6 |
| sensor-emul | F75303 driver on an emulated I2C bus, emulator backend API, app-level monitor | ztest, 5 |
| can-loopback | loopback CAN controller, mask filters, extended IDs, async send, a frame protocol | ztest, 6 |
| dt-kconfig | app-local binding, overlays, `DT_INST_FOREACH`, app Kconfig, Kconfig from DT, 3 twister variants | ztest, 18 |
| input-keys | gpio-keys on an emulated GPIO controller, debounce, input callback, software events | ztest, 5 |
| rust-app | Rust app with zephyr-lang-rust: `rust_main`, `#[zephyr::thread]`, channels | console harness, qemu_cortex_m3 |
| rust-ffi | no_std Rust static library over the C ABI in a C app, `repr(C)` struct, panic handler | ztest, 3 |
| zig-ffi | Zig object (`zig build-obj`) over the C ABI, `extern struct`, Q16.16 math | ztest, 3 |

Templates: `scripts/app new <name> --template <kind> [--dir <parent>]`; `scripts/app templates` lists them. Each
`__name__` template is an app (`src/`, `lib/`) plus `tests/ztest` that pass on qemu_cortex_m3/qemu_riscv32.

| Kind | Folder | What you get |
|---|---|---|
| mcu (default) | `zephyr-blinky-blackpill` | MCU app with CLion profiles, host unit tests (GoogleTest), flash/debug run configs |
| soc | `_template_soc` | application-SoC app (`--board <SoC board>`), boot-command run config |
| ztest | `_template_ztest` | app + lib + ztest suite |
| zbus | `_template_zbus` | modules talking over zbus channels (listener, subscriber thread, validator) |
| smf | `_template_smf` | flat SMF state machine fed by a message queue, one owner thread |
| shell | `_template_shell` | own shell command group; tests through the shell dummy backend |
| settings | `_template_settings` | persistent config on settings/NVS; qemu overlays add a simulated flash, boards use their `storage_partition` |
| logging | `_template_logging` | log module with a Kconfig level option, deferred mode, `log` shell command; tests count levels |
| workq | `_template_workq` | periodic sampler on an own work queue + ring buffer to a consumer |
| sensor | `_template_sensor` | sensor API via the `ambient-temp0` alias, moving average; qemu: emulated F75303 |
| can | `_template_can` | CAN node: heartbeat, command frames, RX filter -> queue; qemu: loopback controller |
| littlefs | `_template_littlefs` | data logger on LittleFS with rotation, fstab mount |
| module | `_template_module` | Zephyr module: out-of-tree driver + binding + Kconfig + API header + tests + sample |
| testplan | `_template_testplan` | twister test plan: console harness for the app, ztest variants (`extra_configs`, `extra_args`, `filter`) |
| rust | `_template_rust` | Zephyr app in Rust (zephyr-lang-rust), console-harness test (qemu_cortex_m3) |
| zig | `_template_zig` | C app + Zig module (`zig/zig.cmake`: `zig_object()`), ztest suite |

Rust, Zig, Bend: Rust and Zig use the toolchains of other IDs, reached and never copied: `BOX_EMBEDDED_RUST`
(32_17_embedded_rust: rustup/cargo, targets thumbv7m/thumbv7em/riscv32imac...), `BOX_RUST_NEXT` (32_22_rust_next:
only its esp-clang libclang for bindgen, unless `LIBCLANG_PATH` is set), `BOX_MICROZIG` (32_24_microzig: zig 0.17);
else the sibling folder. Missing -> the command exits 3 (skip) and says what to install. zephyr-lang-rust is pinned by
`scripts/app setup` (step rust) into `modules/lang-rust`, outside the west workspace. Rust on qemu_riscv32 with
zephyr-lang-rust needs riscv32i-unknown-none-elf, which 32_17 does not install (rust-ffi uses riscv32imac and runs).
Bend: not supported here: Bend 2 emits C for a hosted POSIX system (pthreads, mmap, 64-bit terms), with no
bare-metal target, so it cannot run inside a Zephyr image on these MCUs or qemu boards; Bend proofs/host tools belong
to 32_31_bend.

More: Robot Framework tests (qemu, Renode, serial) and twister's `harness: robot`: `32_30_robot_framework`
(`robot-box zephyr ...`). Debugger: `scripts/app debug <app|example> [board]`. By hand: `west build -b <board> -d <dir> <src>`
(SoC boards: `-DZEPHYR_EXTRA_MODULES=$PWD/ports`). `mcu` (`32_01_mcu`) reaches this ID through symlinks: `mcu build zephyr <target> [example]`.

## Configuration and environment variables

| Variable | Default | Meaning |
|---|---|---|
| `ZEPHYR_OUT` | `$BOX_V3/30_39_tools/32_embedded/32_03_zephyr/build` | build output base |
| `BOARD` | (none) | default board |
| `JOBS` | 8 | parallel build jobs |
| `MIN_FREE_GB` | 8 | builds wait while `MemAvailable` is below this (GB) |
| `ZEPHYR_EXTRA_MODULES` | `<this ID>/ports` | exported by `scripts/app`; set it yourself for manual `west build` |
| `ZEPHYR_BASE`, `ZEPHYR_TOOLCHAIN_VARIANT=zephyr`, `ZEPHYR_SDK_INSTALL_DIR` | set by `env.sh` | Zephyr tree, toolchain variant, SDK location |

## Where files go

| What | Where |
|---|---|
| Source | this ID: `apps/`, `examples/zephyr/`, `ports/` |
| Toolchain, west workspace, SDK | `toolchain/zephyr/` (`ws/`, `sdk/zephyr-sdk` -> `zephyr-sdk-1.0.1`, `python/`, `.venv`), host layer in `toolchain/host/` |
| Build output | `$BOX_V3/30_39_tools/32_embedded/32_03_zephyr/build/`: `apps/<app>/cmake-build-<debug\|release\|host-tests>/`, `examples/<example>/<board>/`, `BOARDS.tsv` |
| Logs | in the build directories (ninja/west output); twister: `$ZEPHYR_OUT/twister/<test>/<board>/`; self tests: `$ZEPHYR_OUT/selftest/` |
| Downloads (setup) | `cache/` (`ZEPHYR_CACHE`), gitignored |
| In git | sources, docs, `scripts/`, `tests/`, the env/doc files of `toolchain/`; everything installed or built is in `.gitignore` |

## Native mode

No native file exists for this ID, so `box native enable` does not apply. `scripts/app` sources the toolchain itself;
`box run zephyr <command>` reaches it from anywhere. To work with `west` directly, enter the environment per shell (bash or zsh):

```sh
source $BOX/30_39_tools/32_embedded/32_03_zephyr/toolchain/zephyr/env.sh
west --version; echo $ZEPHYR_BASE $ZEPHYR_SDK_INSTALL_DIR
```

`env.sh` resolves its physical path, sources `../host/env.sh`, activates `.venv`, sets `ZEPHYR_BASE`,
`ZEPHYR_TOOLCHAIN_VARIANT=zephyr`, `ZEPHYR_SDK_INSTALL_DIR` and puts the SDK hosttools (dtc, openocd, ...) on `PATH`.

## Tests

`tests/smoke.sh` (launcher, setup --check, doctor, usage errors, `new` round trips), `tests/examples.sh` (every example
with a testcase.yaml under twister on qemu_cortex_m3 and qemu_riscv32), `tests/templates.sh` (every template: `new`, its
tests, its build; output under `$ZEPHYR_OUT/selftest`). Also `scripts/app test <app>` (host unit tests, GoogleTest/CTest),
`scripts/app matrix` (all examples x SoC boards), `scripts/app twister`.
2026-10-29 (NOTES.md): see the dated entry for the counts.
Last known (NOTES.md, 2026-10-25): host tests 3/3; matrix 69/69 builds OK; twister 57/57 built (0 warnings); qemu twister 12/12 passed;
`mcu build/selfcheck zephyr` OK; `mcu` build check of zephyr 80/80. No run on hardware.

## Troubleshooting

| Symptom | Fix |
|---|---|
| Old `mcu` host layer used instead of this ID's | `env.sh` must resolve physically (`cd -P`); already fixed, source the `env.sh` of this ID |
| Build waits and prints nothing | `MIN_FREE_GB` guard: free RAM is below 8 GB; lower `MIN_FREE_GB` or close programs |
| `app build` / `app example` with no name | usage error, exit 2 (older versions built the whole folder) |
| Editing a file under `toolchain/` has no effect or changes `mcu` | hardlinked copies: check `stat -c %h <file>`; if above 1, write a new file and `mv` it over |
| H3 timer runs at the wrong rate | mainline `sun8i_h3` takes the CPU clock; the NanoPi defconfigs override `SYS_CLOCK_HW_CYCLES_PER_SEC` to 24000000 |
| openocd script hangs on `exit` | use `shutdown` before init (behaviour of the SDK's openocd build) |
| Flash: probe permission denied | udev rules `toolchain/zephyr/99-zephyr-probes.rules` (needs sudo to install into `/etc/udev/rules.d`) |

## Limitations / needs you

- No board attached: flash and run on hardware untested; SoC boards run only on qemu stand-ins.
- Not offered on the SoC boards (no driver): pwm-fade, adc-read, i2c-scan, usb-cdc, ble-beacon, settings. Not done: H3/H616 SMP, T113 DSP, RV1103 RISC-V core, K230 big core, K210 FPIOA/GPIO.
- udev rules need sudo once. Open items: `$BOX/00_09_meta/02_logs/2026_10_25_open_items.md`.

## Layout

| Path | What |
|---|---|
| `toolchain/zephyr/` | `env.sh`, `ws/`, `sdk/`, `python/`, `.venv`, `sdk.sh` (the `mcu` plugin), `README.md` |
| `toolchain/host/` | this ID's copy of the host layer: Yocto buildtools (gcc/make/git), cmake, ninja, uv, CPython, private libs |
| `toolchain/clion/` | `cmake/McuHostTests.cmake`; GoogleTest 1.18.0 in `third_party/` (installed by setup) for the apps' host unit tests |
| `ports/` | Zephyr module: out-of-tree boards, SoCs, drivers, DTs (`ports/README.md`; each board has a README with its boot flow) |
| `apps/<app>/` | your applications (CLion-ready: `.idea/`, `.run/`; `cmake.xml`/`Debug.run.xml` rendered from `*.in`); `apps/_template_*` the templates |
| `examples/zephyr/<ex>/` | the 28 examples |
| `modules/lang-rust` | zephyr-lang-rust (setup step rust, gitignored) |
| `BOARDS.md` | SoC board matrix and test results |
| `scripts/app` | the driver; `scripts/setup` the pinned installer; `scripts/requirements.lock` the venv pins |
| `tests/` | `smoke.sh`, `examples.sh`, `templates.sh` |

`toolchain/host` and `toolchain/clion` are hardlinked copies of the `mcu` versions; files that needed their own paths are separate inodes.
Their READMEs describe `mcu` commands because the copies came from there.

## Scripts

- `scripts/app` - build, test, flash, monitor apps and examples; `new`, `templates`, `boards`, `bootcmd`, `matrix`, `run` (qemu),
  `ztest`, `twister`, `debug`, `setup`, `doctor`, `where`.
- `scripts/setup` - pinned, sha256-checked install of `toolchain/` for a fresh clone (`--check`, `--only`, `--doctor`).
  `scripts/app help` lists the commands, environment variables and examples; exit codes: 0 ok, 1 failure, 2 usage error.
  Described in `.box.toml` (`[tool]` + `[scripts.app]`); `box describe zephyr` shows it.

## Links

- [BOARDS.md](BOARDS.md), `ports/README.md`, `toolchain/zephyr/README.md`, [NOTES.md](NOTES.md)
- Reports in `$BOX/00_09_meta/02_logs/`: `2026_10_24_zephyr_soc_report.md`, `2026_10_24_mcu_plugins_C_report.md`, `2026_10_25_readability_embedded_core_report.md`
- Sibling tool `32_01_mcu` (the `mcu` launcher uses this ID's toolchain).
