# zephyr / workq-timers

Work queues and kernel timers.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- system work queue vs. an own work queue (`k_work_queue_start`, thread name)
- a button debouncer on delayable work: every edge `k_work_reschedule`s the 30 ms window; cancel with `k_work_cancel_delayable_sync`
- periodic `k_timer` with expiry and stop functions; one-shot timer with `k_timer_status_sync`
- timing checks allow one tick of slack (1000 ticks/s in `prj.conf`)

## Run
```
../../../scripts/app ztest workq-timers                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest workq-timers qemu_riscv32
../../../scripts/app debug workq-timers qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/workq-timers/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
On a board call `debounce_edge()` from a GPIO interrupt callback.

## Expected output
`6 of 6 executed test cases passed` in the twister summary.
