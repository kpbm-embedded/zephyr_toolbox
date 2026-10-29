# Zephyr RTOS plugin for ../mcu (see ../PLUGIN.md). Sourced by bash with cwd=$SDK_DIR.
SDK_SELFCHECK="blackpill_f411ce samples/basic/blinky"
SDK_DESC="Zephyr RTOS (west workspace + Zephyr SDK GNU toolchains: arm, riscv64, esp32/esp32s3)"

# target | west board (qualifiers) | owned board (BOARDS.md) | default flash runner
_ZTARGETS='blackpill_f411ce|blackpill_f411ce|WeAct BlackPill F411CEU6|openocd
weact_g431|weact_stm32g431_core|WeAct STM32G431CBT6 core|openocd
weact_g030|weact_stm32g030_core|WeAct STM32G031F8P6 core (G030 def, closest)|openocd
weact_h562|weact_stm32h562_core|STM32H562RGT6 min-system (WeAct)|openocd
weact_h743|mini_stm32h743|STM32H743VIT6 min-system (WeAct mini H743)|openocd
weact_f405|weact_stm32f405_core|JHEMCU F405-class (WeAct F405 core)|openocd
bluepill_f103|stm32_min_dev@blue|WeAct BluePill+/F103C8T6 boards|openocd
promicro_nrf52840|promicro_nrf52840/nrf52840/uf2|nRF52840 Pro Micro / nice!nano clone (UF2 bootloader)|uf2
rpi_pico|rpi_pico|Raspberry Pi Pico (RP2040)|openocd
rpi_pico2|rpi_pico2/rp2350a/m33|Pico 2 / RP2350A boards|openocd
esp32s3|esp32s3_devkitc/esp32s3/procpu|ESP32-S3-WROOM-1 devkit N16R8|esp32
esp32c3|esp32c3_devkitm|ESP32-C3 SuperMini/Zero (devkitm def)|esp32
ch32v003|ch32v003evt|CH32V003 EVT|wlink
ek_ra6e2|ek_ra6e2|Renesas RA6E2 board|jlink
mimxrt1010|mimxrt1010_evk|i.MX RT1011/1021 class (RT1010 EVK)|jlink'

_zrow() { printf '%s\n' "$_ZTARGETS" | awk -F'|' -v t="$1" '$1==t{print; exit}'; }
_zslug() { local s="${1:-samples/basic/blinky}"; s="${s#"$ZEPHYR_BASE"/}"; s="${s#/}"; echo "${s//\//_}"; }
_zapp() {
  local e="${1:-samples/basic/blinky}"
  if [[ "$e" = /* ]]; then echo "$e"; else echo "$ZEPHYR_BASE/$e"; fi
}

sdk_env() { source "$SDK_DIR/env.sh"; }

sdk_doctor() {
  local rc=0 t v
  for t in west cmake ninja dtc python arm-zephyr-eabi-gcc riscv64-zephyr-elf-gcc xtensa-espressif_esp32s3_zephyr-elf-gcc openocd; do
    case "$t" in
      *-gcc) p="$(ls "$ZEPHYR_SDK_INSTALL_DIR"/gnu/*/bin/$t 2>/dev/null | head -1)" ;;
      *) p="$(command -v "$t" || true)" ;;
    esac
    if [[ -n "$p" ]]; then v="$("$p" --version 2>&1 | head -1)"; echo "ok      $t  $v"; else echo "MISSING $t"; rc=1; fi
  done
  [[ -d "$ZEPHYR_BASE" ]] && echo "ok      ZEPHYR_BASE $ZEPHYR_BASE ($(git -C "$ZEPHYR_BASE" describe --tags 2>/dev/null))" || { echo "MISSING ZEPHYR_BASE"; rc=1; }
  [[ -d "$ZEPHYR_SDK_INSTALL_DIR" ]] && echo "ok      SDK $ZEPHYR_SDK_INSTALL_DIR" || { echo "MISSING SDK"; rc=1; }
  return $rc
}

sdk_versions() {
  echo "zephyr   $(git -C "$ZEPHYR_BASE" describe --tags 2>/dev/null)"
  echo "zephyr-sdk $(cat "$ZEPHYR_SDK_INSTALL_DIR/sdk_version")"
  echo "west     $(west --version)"
  echo "python   $(python --version 2>&1)"
  echo "arm-gcc  $("$ZEPHYR_SDK_INSTALL_DIR"/gnu/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc -dumpversion)"
  echo "openocd  $(openocd --version 2>&1 | head -1)"
}

sdk_targets() {
  printf '%-18s %-36s %s\n' TARGET WEST_BOARD OWNED_BOARD
  printf '%s\n' "$_ZTARGETS" | awk -F'|' '{printf "%-18s %-36s %s\n",$1,$2,$3}'
  echo "(any other west board name is also accepted as a target)"
}

sdk_examples() {
  echo "samples/basic/blinky (default)"; echo "samples/hello_world"; echo "samples/basic/button"
  echo "<any path relative to ws/zephyr, or an absolute app dir>"
}

sdk_build() {
  local t="${1:?target}" ex="${2:-}"
  [[ -n "$ex" ]] || { case "$t" in esp32*) ex=samples/hello_world;; *) ex=samples/basic/blinky;; esac; }
  shift $(( $# >= 2 ? 2 : 1 )) || true
  [[ "${1:-}" == -- ]] && shift
  local row board; row="$(_zrow "$t")"; board="${row:+$(cut -d'|' -f2 <<<"$row")}"; board="${board:-$t}"
  local out="$MCU_OUT/$t/$(_zslug "$ex")"
  mkdir -p "$out"
  CMAKE_BUILD_PARALLEL_LEVEL="$MCU_JOBS" west build -p auto -b "$board" -d "$out" "$(_zapp "$ex")" -- "$@" || return $?
  echo "--- artifacts"; ls -l "$out"/zephyr/zephyr.{elf,bin,hex,uf2} 2>/dev/null || true
}

sdk_flash() {
  local t="${1:?target}" ex="${2:-samples/basic/blinky}" probe="${3:-}"
  local row runner; row="$(_zrow "$t")"; runner="$(cut -d'|' -f4 <<<"${row:-||||}")"
  local out="$MCU_OUT/$t/$(_zslug "$ex")"
  [[ -d "$out" ]] || { echo "no build at $out; run: mcu build zephyr $t" >&2; return 1; }
  case "$runner" in
    uf2) echo "Double-tap reset to mount the UF2 drive, then: cp $out/zephyr/zephyr.uf2 /run/media/\$USER/<DRIVE>/"; return 0 ;;
    wlink) echo "Use wlink (WCH-Link): wlink flash $out/zephyr/zephyr.elf  (or: west flash --runner openocd with a WCH openocd)"; ;;
    esp32) west flash -d "$out" --runner esp32 ${probe:+--esp-device "$probe"} ;;
    jlink) west flash -d "$out" --runner jlink ;;
    *) west flash -d "$out" --runner "$runner" ;;
  esac
}

sdk_monitor() { python -m serial.tools.miniterm "${1:-/dev/ttyACM0}" "${2:-115200}"; }

sdk_clean() {
  local t="${1:?target}" ex="${2:-samples/basic/blinky}"
  local out="$MCU_OUT/$t/$(_zslug "$ex")"
  [[ -d "$out" ]] || { echo "nothing at $out"; return 0; }
  mkdir -p ~/.local/share/Trash/files
  mv "$out" ~/.local/share/Trash/files/"zephyr-$t-$(_zslug "$ex")-$(date +%s)" && echo "moved $out to Trash"
}

sdk_prune_families() { printf 'sdk/zephyr-sdk|zephyr-sdk-[0-9]*\n'; }
_z_do() { local new="$1" need d base f
  upd_rec west ws "$(git -C "$ZEPHYR_BASE" rev-parse HEAD)"
  (cd "$SDK_DIR/ws/zephyr" && git fetch -q --depth=1 origin tag "$new" && git checkout -q "$new") || return 1
  (cd "$SDK_DIR/ws" && west update --narrow -o=--depth=1) || return 1
  uv pip install -q -r "$ZEPHYR_BASE/scripts/requirements.txt" || return 1
  need="$(tr -d '[:space:]' < "$ZEPHYR_BASE/SDK_VERSION")"; d="$SDK_DIR/sdk/zephyr-sdk-$need"
  base="https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v$need"
  if [[ ! -d $d ]]; then
    mkdir -p "$SDK_DIR/dl" "$SDK_DIR/sdk" || return 1
    for f in zephyr-sdk-${need}_linux-x86_64_minimal.tar.xz hosttools_linux-x86_64.tar.xz \
      toolchain_gnu_linux-x86_64_{arm-zephyr-eabi,riscv64-zephyr-elf,xtensa-espressif_esp32_zephyr-elf,xtensa-espressif_esp32s3_zephyr-elf}.tar.xz; do
      upd_fetch "$base/$f" "$SDK_DIR/dl/$f" "url:$base/sha256.sum" || return 1; done
    tar xf "$SDK_DIR/dl/zephyr-sdk-${need}_linux-x86_64_minimal.tar.xz" -C "$SDK_DIR/sdk" || return 1
    mkdir -p "$d/gnu"; for f in "$SDK_DIR"/dl/toolchain_gnu_*.tar.xz; do tar xf "$f" -C "$d/gnu"; done
    (cd "$d" && ./setup.sh -h -c) || return 1
    upd_patch "$d"
  fi
  upd_flip "$SDK_DIR/sdk/zephyr-sdk" "zephyr-sdk-$need"; }
sdk_update() {
  UPD_RC=0
  local cur new
  cur="$(git -C "$ZEPHYR_BASE" describe --tags)"
  new="${1:-$(upd_git_newest_tag https://github.com/zephyrproject-rtos/zephyr '^v[0-9]+\.[0-9]+\.[0-9]+$')}"
  upd_step zephyr "$cur" "$new" _z_do "$new"
  upd_report zephyr-sdk "$(basename "$(readlink -f "$ZEPHYR_SDK_INSTALL_DIR")" | sed s/zephyr-sdk-//)" "$(tr -d '[:space:]' < "$ZEPHYR_BASE/SDK_VERSION")"; echo "  (zephyr-sdk follows the version the checked-out zephyr requires)"
  upd_dry_hint
  return "$UPD_RC"
}
