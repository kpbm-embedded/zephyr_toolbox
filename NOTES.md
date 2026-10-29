# 32_03_zephyr notes

## 2026_10_24
- Reorg wrap-up. `toolchain/zephyr/env.sh` resolves its physical path (`cd -P`). Before this, when `mcu` reached it through its `tool/zephyr` symlink, `../host` resolved to mcu's host layer.
- Repaired `toolchain/host/patchelf-0.18.0/patchelf` (ELF interpreter to this ID's buildtools) and the `_sysconfigdata` prefixes in `toolchain/{zephyr,host}/python/*`.
- New `scripts/app` (build/test/flash/example/monitor, only this ID's toolchain). Output: $ZEPHYR_OUT.
- New `toolchain/clion/` (hardlinked copy of McuHostTests.cmake + GoogleTest) so the apps' host tests no longer need the mcu ID. `apps/*/tests/CMakeLists.txt` finds it with REALPATH.
- `apps/zephyr-blinky-blackpill`: CLion build dirs moved to idisk03 at this ID. The run configs use `../../scripts/app`, and the debug openocd now comes from the Zephyr SDK hosttools (the old `toolchain/tools` path never existed here). README rewritten.
- Example READMEs: build/flash/monitor lines now use this ID's paths and `scripts/app`. New `README.md` for the ID.
- Verified: app Debug build (157 steps, ~10 s), host tests 3/3, `scripts/app example blink`, `mcu build/selfcheck zephyr`.
- 2026_10_24 toolchain host/zephyr post-reorg check (report: 00_09_meta/02_logs/2026_10_24_mcu_plugins_C_report.md). Fixed: buildtools `ldconfig` (static) loader sections re-pointed to this ID's sysroot (new inode). 213 stale `.pyc` in `zephyr/python` and `zephyr/.venv` recompiled with this ID's python. 10 stale uv interpreter-cache entries moved to Trash. All tools run via `toolchain/zephyr/env.sh` (cmake, ninja, west, SDK gcc/gdb, hosttools dtc/qemu/openocd/bossac). The SDK openocd needs `shutdown`, not `exit`, before init (behaviour of that build, not a bug). Fresh `scripts/app build zephyr-blinky-blackpill Release` is OK (157/157); build.ninja references only this ID. Text self-containment grep: empty.
- 2026_10_24 embedded extras (00_09_meta/02_logs/2026_10_24_embedded_extras_report.md): verification build `$ZEPHYR_OUT/_fresh_mcuC` moved to Trash (logged). Not changed: `toolchain/host/lib/libusb-1.0.so.0.6.0` still has old DWARF/`__FILE__` paths (cosmetic).

## 2026_10_25
- Application SoCs (report: 00_09_meta/02_logs/2026_10_24_zephyr_soc_report.md). New `ports/` Zephyr module: 11 out-of-tree boards (nanopi_neo, nanopi_duo2, opi_zero2, mangopi_mq_r, luckfox_pico_mini, luckfox_lyra, milkv_duo, licheerv_nano, k210_generic, canmv_k230, k230d_bpi_zero), SoCs sun50i_h616, sun8i_t113, rv1106, rk3506, cv18xx (cv1800b/sg2002 little C906), k210, k230; drivers gpio_allwinner (Zephyr main backport + pull-offset), gpio_rockchip_v2 (new), thead_clint_timer (new). Mainline: stm32mp135f_dk, SoC sun8i_h3. Sources of facts: Linux/Rockchip/Luckfox DTs and Zephyr main/PRs #103787 #110502 #110923 #69594 #74169 (see ports/README.md).
- Zephyr SDK 1.0.1: added `gnu/aarch64-zephyr-elf` (toolchain_gnu_linux-x86_64_aarch64-zephyr-elf.tar.xz, sha256 01c0cfe1daaab2d2a0f71165f11a066d983ef97e51c4a3753186afa6ae55c24b OK vs the release sha256.sum) for H616 and qemu_cortex_a53. New directory, nothing hardlinked was edited.
- Mainline sun8i_h3 sets SYS_CLOCK_HW_CYCLES_PER_SEC from the CPU clock although the arch timer runs at 24 MHz; the NanoPi defconfigs override it to 24000000.
- scripts/app: new `new`, `boards`, `bootcmd`, `matrix`, `run`; ZEPHYR_EXTRA_MODULES=ports; JOBS default 8 (was nproc), RAM guard; `build` takes the board from the app's .idea/cmake.xml when none is given; `images` no longer fails on boards without .hex. New template apps/_template_soc. examples/zephyr/boards.yaml + sample.yaml per core example (example.toml untouched, mcu unaffected).
- Tests: matrix 69/69 builds OK, twister 57/57 built (0 warnings), qemu twister 12/12 passed, ELF load/entry vs RAM checked; no board attached, nothing run on hardware. Test apps/trial builds moved to Trash (logged). Backups: 00_09_meta/02_logs/config_backups/2026_10_24_225005_zephyr_soc/.
- 2026_10_25 readability pass (report: 00_09_meta/02_logs/2026_10_25_readability_embedded_core_report.md): scripts/app split into one function per command, help text with env/examples; usage errors exit 2 with a message (`app build` without an app was silent, `app example` without a name built the whole examples dir).
- 2026_10_25 readability pass 2 (report: 00_09_meta/02_logs/2026_10_25_readability2_embedded_core_report.md): `scripts/app where`; missing example or bad profile is a usage error (exit 2); one example_build_dir() for example/bootcmd/matrix/run (bootcmd and matrix used the raw board name, so a qualified board like soc/cpu was looked up in a different dir than `example` built it in). Descriptor commands += test, where. README: `box run zephyr ...`, native n/a.

## 2026-10-29
- Zephyr's own test framework: examples `ztest-basic` (fixtures, zassert/zexpect, skip, shuffle; 10 cases incl. a no-shuffle
  variant), `ztest-fff` (FFF fakes, 3 cases), `pytest-shell` (twister pytest harness, 3 cases); template `apps/_template_ztest`
  (`scripts/app new <name> --ztest`). New commands `ztest`, `twister`, `debug`. All pass on qemu_cortex_m3; a broken assertion
  in a template copy fails the run (exit 1, the failed case named); the template's tests and app debug also pass.
- Twister refuses to run while pytest-twister-harness is pip-installed (it puts the plugin on PYTHONPATH itself); the
  `build/` and `egg-info` an install leaves in `scripts/pylib/pytest-twister-harness` register the plugin twice ("Plugin already
  registered"). Both moved to the Trash (logged). Venv unchanged in the end (`uv pip freeze` identical).
- This SDK has no x86 toolchain: qemu_x86 is out; examples allow qemu_cortex_m3, qemu_riscv32 (and qemu_cortex_a53).
- Coverage: only boards with HAS_COVERAGE_SUPPORT (qemu_riscv32/riscv64, mps2/an385; not qemu_cortex_m3); `--gcov-tool` of the
  board's SDK triple; gcovr 8.7 crashes (KeyError in remove_internal_functions) unless `include-internal-functions`: set in
  `gcovr.cfg` at the ID root (twister runs gcovr with `-r <ID>`), which also leaves the Zephyr tree out. ztest-basic: fifo.c 100%.
- debug: qemu's gdb stub is fixed at :1234 (qemu.cmake `-s`); the server runs in its own session and the EXIT trap kills the group
  (a local pid variable was unset by the time the trap ran).

## 2026-10-29 (GitHub preparation: examples, templates, setup, Rust/Zig)
- 13 new examples, all twister-tested on qemu: zbus-pubsub, log-backend, settings-nvs, smf-pump, workq-timers,
  littlefs-flashsim, sensor-emul, can-loopback, dt-kconfig (3 variants), input-keys (qemu_cortex_m3 + qemu_riscv32);
  rust-app (zephyr-lang-rust, qemu_cortex_m3), rust-ffi, zig-ffi (both qemu boards). README per example (also for the
  three ztest/pytest examples of the morning). The ten C examples also build for their example.toml target
  (blackpill_f411ce, can-loopback weact_stm32g431_core): 10/10.
- 13 new templates (`scripts/app new <name> --template <kind> [--dir <parent>]`, `__name__` placeholders, `templates`
  lists them): zbus, smf, shell, settings, logging, workq, sensor, can, littlefs, module (out-of-tree driver + binding
  + Kconfig + tests + sample), testplan (twister console harness + variants/filter), rust, zig. Default `new` unchanged.
- `scripts/setup` / `scripts/app setup|doctor`: pinned, sha256-checked install of everything gitignored under
  toolchain/ (buildtools 6.0.4, cmake 4.4.4, ninja 1.13.2, uv 0.13.5, CPython 3.13.17, venv from
  `scripts/requirements.lock` with hashes, Zephyr v4.4.2 narrow/depth 1 + hal_espressif blobs, SDK 1.0.1 with the same
  toolchains, GoogleTest 1.18.0) and zephyr-lang-rust 6823b690 into `modules/lang-rust`. In this Box tree `setup --check`
  reports everything done and `setup` changes nothing (an existing non-link host/buildtools is kept).
- CLion files with absolute paths (`.idea/cmake.xml`, `.run/Debug.run.xml`) are now rendered from `*.in`
  (`@ID_ROOT@`, `@ZEPHYR_OUT@`) by setup and `new`; the rendered files here are byte-identical to the old ones.
  Hard paths in READMEs/NOTES/BOARDS replaced by `$BOX`, `$ZEPHYR_OUT`, relative paths. `mcu-project.json` gitignored.
- Rust/Zig reached, not copied: 32_17_embedded_rust (rustup/cargo 1.99.1), 32_22_rust_next (esp-clang libclang for
  bindgen: the OS libclang.so.23 misses libLLVM.so.23), 32_24_microzig (zig 0.17); exit 3 = skip when missing.
  Found: zephyr-lang-rust maps qemu_riscv32 to riscv32i-unknown-none-elf (not installed in 32_17), so rust-app runs on
  qemu_cortex_m3 only; its channels panic when the last end is dropped (worker serves forever, ends forgotten);
  zig objects need -fno-unwind-tables (orphan .ARM.extab fails the link); zassert_within evaluates its argument twice.
  Bend: nothing added: Bend 2 emits hosted POSIX C (no bare-metal target), so it cannot run in a Zephyr image.
  (No BEAM/AtomVM items were added; AtomVM has no Zephyr platform either.)
- Other problems: idisk03 was full for a while (other IDs); tests ran with ZEPHYR_OUT=/var/tmp/zephyr-box/out meanwhile.
- Tests (new: tests/smoke.sh, tests/examples.sh, tests/templates.sh): smoke 26/26; examples 31/31 runs (16 examples x
  qemu_cortex_m3/qemu_riscv32, rust-app riscv32 n/a), 185 test cases passed; templates 60/60 steps (16 templates: new,
  no __name__ left, tests, build). `_template_soc` failed outside apps/ (../../ports missing): the path is now only added
  when it exists (scripts/app exports ports/ anyway). No board attached: nothing ran on hardware.
