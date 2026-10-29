#!/usr/bin/env bash
# Instantiates every template with `scripts/app new` (into $ZEPHYR_OUT/selftest/templates.*, not apps/), then:
# __name__ templates: their tests on qemu (ztest; testplan: every twister scenario) and the app build for
# qemu_cortex_m3 (module: samples/basic); ztest: tests + build; mcu: build for blackpill_f411ce; soc: build for
# nanopi_neo. Usage: tests/templates.sh [kind ...]
set -uo pipefail
here=$(cd "$(dirname "$0")/.." && pwd)
app="$here/scripts/app"
out=$("$app" where | awk '$1=="output"{print $2}')
mkdir -p "$out/selftest"; tmp=$(mktemp -d "$out/selftest/templates.XXXXXX")
export ZEPHYR_OUT="$tmp/out"   # builds of the copies go with them (build dirs are named after the app folder)
pass=0 fail=0 skip=0
step() { # <kind> <what> <cmd...>   (exit 3 of scripts/app = Rust/Zig toolchain ID missing = skip)
  local kind=$1 what=$2 rc; shift 2
  "$@" >"$tmp/$kind.$what.log" 2>&1; rc=$?
  if [ $rc -eq 0 ]; then pass=$((pass + 1)); printf 'ok   %-9s %s\n' "$kind" "$what"
  elif [ $rc -eq 3 ]; then skip=$((skip + 1)); printf 'skip %-9s %s (%s)\n' "$kind" "$what" "$(grep -m1 SKIP "$tmp/$kind.$what.log")"
  else fail=$((fail + 1)); printf 'FAIL %-9s %s (log: %s)\n' "$kind" "$what" "$tmp/$kind.$what.log"; return 1; fi
}
kinds=${*:-$("$app" templates | awk '{print $1}')}
for k in $kinds; do
  d="$tmp/t_$k"
  case "$k" in
    mcu) step "$k" new "$app" new "t_$k" --dir "$tmp" && step "$k" build "$app" build "$d" Release blackpill_f411ce ;;
    soc) step "$k" new "$app" new "t_$k" --board nanopi_neo --dir "$tmp" && step "$k" build "$app" build "$d" Release ;;
    *)
      step "$k" new "$app" new "t_$k" --template "$k" --dir "$tmp" || continue
      step "$k" placeholders bash -c '! grep -rq __name__ "$1"' _ "$d"
      case "$k" in
        testplan) step "$k" twister "$app" twister -T "$d" -p qemu_cortex_m3 -p qemu_riscv32 ;;
        *) step "$k" ztest "$app" ztest "$d" ;;
      esac
      case "$k" in
        module) step "$k" build "$app" build "$d/samples/basic" Debug qemu_cortex_m3 ;;
        *) step "$k" build "$app" build "$d" Debug qemu_cortex_m3 ;;
      esac
      ;;
  esac
done
echo "templates: $pass ok, $fail failed, $skip skipped; copies and logs in $tmp"
[ "$fail" -eq 0 ]
