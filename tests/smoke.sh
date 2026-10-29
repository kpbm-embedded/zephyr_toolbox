#!/usr/bin/env bash
# Smoke test of scripts/app and scripts/setup: help, where, templates, usage errors (exit 2), setup --check,
# and, when the toolchain is installed, doctor and `new` for the mcu/soc templates (CLion files rendered).
# Usage: tests/smoke.sh        (output under $ZEPHYR_OUT/selftest; nothing in the repo changes)
set -uo pipefail
here=$(cd "$(dirname "$0")/.." && pwd)
app="$here/scripts/app"
pass=0 fail=0
t() { local name=$1; shift; if "$@" >/dev/null 2>&1; then pass=$((pass + 1)); else fail=$((fail + 1)); echo "FAIL $name"; fi; }
exits() { local want=$1; shift; "$@" >/dev/null 2>&1; [ $? -eq "$want" ]; }
t help "$app" help
t setup-help "$here/scripts/setup" --help
t unknown-exits-2 exits 2 "$app" no-such-command
t setup-bad-step-exits-2 exits 2 "$app" setup --only nothing
t setup-bad-option-exits-2 exits 2 "$app" setup --bogus
if "$here/scripts/setup" --check >/dev/null 2>&1; then
  out=$("$app" where | awk '$1=="output"{print $2}')
  tmp=$(mkdir -p "$out/selftest" && mktemp -d "$out/selftest/smoke.XXXXXX")
  t setup-check "$here/scripts/setup" --check
  t doctor "$app" doctor
  t doctor-extra-arg-exits-2 exits 2 "$app" doctor x
  t where "$app" where
  t templates "$app" templates
  t boards "$app" boards
  t build-no-app-exits-2 exits 2 "$app" build
  t example-no-name-exits-2 exits 2 "$app" example
  t ztest-no-target-exits-2 exits 2 "$app" ztest
  t new-no-name-exits-2 exits 2 "$app" new
  t new-bad-template-exits-2 exits 2 "$app" new x --template no-such-kind --dir "$tmp"
  t new-soc-without-board-exits-2 exits 2 "$app" new x --template soc --dir "$tmp"
  t new-mcu "$app" new m1 --dir "$tmp"
  t new-mcu-board bash -c 'grep -q -- "-DBOARD=blackpill_f411ce" "$1/m1/.idea/cmake.xml"' _ "$tmp"
  t new-mcu-rendered bash -c '! grep -rq "@ID_ROOT@\|@ZEPHYR_OUT@" "$1/m1/.idea/cmake.xml" "$1/m1/.run/Debug.run.xml"' _ "$tmp"
  t new-mcu-renamed bash -c 'grep -q "^project(m1)" "$1/m1/CMakeLists.txt"' _ "$tmp"
  t new-soc "$app" new s1 --board luckfox_lyra --dir "$tmp"
  t new-soc-board bash -c 'grep -q -- "-DBOARD=luckfox_lyra" "$1/s1/.idea/cmake.xml"' _ "$tmp"
  t new-exists-fails exits 1 "$app" new s1 --dir "$tmp"
  t new-zbus-placeholders bash -c '"$1" new z1 --template zbus --dir "$2" && ! grep -rq __name__ "$2/z1"' _ "$app" "$tmp"
  t new-ztest "$app" new zt1 --ztest --dir "$tmp"
  echo "smoke: temp files in $tmp"
else
  echo "smoke: toolchain not installed (scripts/app setup): only the checks without it ran"
fi
echo "smoke: $pass ok, $fail failed"
[ "$fail" -eq 0 ]
