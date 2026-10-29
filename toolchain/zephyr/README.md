# Zephyr (self-contained)

| Item | Version |
|---|---|
| Zephyr | v4.4.2 (west workspace ws/, `west update --narrow --depth=1`) |
| Zephyr SDK | 1.0.1 minimal + arm-zephyr-eabi, aarch64-zephyr-elf (added 2026-10-25, sha256 checked), riscv64-zephyr-elf, xtensa-espressif_esp32/esp32s3 (GCC 14.3.0), hosttools (dtc, openocd 0.12.0) |
| West / Python | west 1.5.0, uv-managed CPython 3.13.17 in python/ (venv .venv, cmake+ninja+esptool from pip) |
| Blobs | `west blobs fetch hal_espressif` done |

Use: `source env.sh` (bash/zsh; resolves its own physical path, then sources ../host/env.sh of this ID). Apps and examples of this ID: `../../scripts/app build|test|flash|example|monitor` (see ../../README.md); output in $ZEPHYR_OUT/.
Fresh clone of this ID (no `mcu`): `../../scripts/app setup` installs ws/, sdk/, python/, .venv and ../host with the same versions (pins and sha256 in `../../scripts/setup`, venv pins in `../../scripts/requirements.lock`).
Through the category's `mcu` tool (it reaches this folder via its `zephyr` symlink): `mcu build zephyr <target> [example]` -> `$MCU_OUT_BASE/zephyr/<target>/<example-slug>`. Default example samples/basic/blinky (esp32*: samples/hello_world, no led0 alias).
Flash: `mcu flash zephyr <target>` (west flash --runner ...) or `west flash -d <build dir>`. Update: `MCU_YES=1 mcu update zephyr`.
OS dependencies: only glibc, base utils, ca-certs; libcheck is clean: SDK gdb binaries resolve libncursesw (and gdb-py libpython3.12) through ../host via RUNPATH; gdb is not used by builds.
udev: 99-zephyr-probes.rules (needs root, not installed).

| Target | Zephyr board | Owned board | Result | Runner |
|---|---|---|---|---|
| blackpill_f411ce | blackpill_f411ce | BlackPill F411 | OK elf/bin/hex | openocd (stlink) |
| weact_g431 | weact_stm32g431_core | G431 core | OK | openocd |
| weact_g030 | weact_stm32g030_core | G031 core (closest) | OK | openocd |
| weact_h562 | weact_stm32h562_core | H562 min | OK | openocd |
| weact_h743 | mini_stm32h743 | H743 min | OK | openocd |
| weact_f405 | weact_stm32f405_core | F405 | OK | openocd |
| bluepill_f103 | stm32_min_dev@blue | F103C8 | OK | openocd |
| promicro_nrf52840 | promicro_nrf52840/nrf52840/uf2 | nice!nano clone | OK elf/bin/uf2 | copy uf2 to bootloader drive |
| rpi_pico | rpi_pico | Pico | OK uf2 | uf2 / openocd (picoprobe) |
| rpi_pico2 | rpi_pico2/rp2350a/m33 | Pico 2 | OK uf2 | uf2 / openocd |
| esp32s3 | esp32s3_devkitc/esp32s3/procpu | S3 devkit | OK (hello_world) elf/bin | esp32 (esptool) |
| esp32c3 | esp32c3_devkitm | C3 SuperMini | OK (hello_world) elf/bin | esp32 (esptool) |
| ch32v003 | ch32v003evt | CH32V003 | OK elf/bin | wlink (WCH-Link) |
| ek_ra6e2 | ek_ra6e2 | RA6E2 | OK elf/bin | jlink |
| mimxrt1010 | mimxrt1010_evk | RT1010 class | OK elf/bin | jlink / pyocd |

Artifacts live in <build>/zephyr/zephyr.{elf,bin,hex,uf2}. Not available upstream: GD32F330, Puya, AT32, nRF5340 HOLYIOT custom, ESP32-P4 (not tried).

Application-SoC boards (H3, H616, T113, RV1103, RK3506, STM32MP13, CV1800B/SG2002, K210, K230): out-of-tree module `../../ports`, matrix in `../../BOARDS.md`.
