#!/usr/bin/env bash
# Runs every example with a testcase.yaml (ztest / pytest suites), or a sample.yaml that allows qemu_cortex_m3
# (console harness, e.g. rust-app), under twister on qemu for each board in BOARDS (default: qemu_cortex_m3
# qemu_riscv32). Boards an example does not allow count as n/a; exit 3 of scripts/app (Rust/Zig toolchain ID
# missing) counts as skip. Usage: tests/examples.sh [example ...]
set -uo pipefail
here=$(cd "$(dirname "$0")/.." && pwd)
app="$here/scripts/app"
boards=${BOARDS:-qemu_cortex_m3 qemu_riscv32}
out=$("$app" where | awk '$1=="output"{print $2}')
log="$out/selftest/examples"; mkdir -p "$log"
pass=0 fail=0 skip=0 cases=0
if [ $# -gt 0 ]; then list="$*"; else
  list=$(cd "$here/examples/zephyr" && for d in */; do
    if [ -f "$d/testcase.yaml" ] || grep -qs "qemu_cortex_m3" "$d/sample.yaml"; then echo "${d%/}"; fi
  done)
fi
for ex in $list; do
  for b in $boards; do
    "$app" ztest "$ex" "$b" >"$log/$ex.$b.log" 2>&1
    rc=$?
    n=$(grep -o '[0-9]* of [0-9]* executed test cases passed' "$log/$ex.$b.log" | awk '{print $1}')
    if [ $rc -eq 3 ]; then
      skip=$((skip + 1)); printf 'skip %-20s %-16s %s\n' "$ex" "$b" "$(grep -m1 SKIP "$log/$ex.$b.log")"
    elif [ $rc -eq 0 ] && grep -q " 0 of 0 executed test configurations" "$log/$ex.$b.log"; then
      printf 'n/a  %-20s %-16s (not in platform_allow)\n' "$ex" "$b"
    elif [ $rc -eq 0 ]; then
      pass=$((pass + 1)); cases=$((cases + ${n:-0}))
      printf 'ok   %-20s %-16s %s cases\n' "$ex" "$b" "${n:-?}"
    else
      fail=$((fail + 1)); printf 'FAIL %-20s %-16s log: %s\n' "$ex" "$b" "$log/$ex.$b.log"
    fi
  done
done
echo "examples: $pass ok, $fail failed, $skip skipped ($cases test cases passed); logs in $log"
[ "$fail" -eq 0 ]
