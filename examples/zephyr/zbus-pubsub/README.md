# zephyr / zbus-pubsub

zbus publish/subscribe between modules, checked by a ztest suite.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- `ZBUS_CHAN_DEFINE` with a validator (`temp_valid`): out-of-range messages are rejected with `-ENOMSG` and never reach observers
- three observer kinds on one channel: listener (callback in the publisher's context), subscriber (thread, notification only, reads the channel), message subscriber (thread, own copy of every message via a static net_buf pool)
- observers added and removed at run time (`zbus_chan_add_obs`, static node pool)
- channel and observer names (`CONFIG_ZBUS_CHANNEL_NAME`)

## Run
```
../../../scripts/app ztest zbus-pubsub                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest zbus-pubsub qemu_riscv32
../../../scripts/app debug zbus-pubsub qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/zbus-pubsub/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
Nothing board-specific: the same image runs on any board; the result is printed on the console UART.

## Expected output
`6 of 6 executed test cases passed` in the twister summary.
