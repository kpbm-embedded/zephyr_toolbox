# zephyr / can-loopback

CAN without a transceiver: Zephyr's loopback CAN controller.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- `zephyr,can-loopback` as `zephyr,canbus`, `CAN_MODE_LOOPBACK`: sent frames reach the node's own filters
- mask filter into a message queue (`CAN_MSGQ_DEFINE`, `can_add_rx_filter_msgq`), extended IDs need their own filter
- async send with a TX callback, bus state and error counters
- a small status-frame protocol (`src/proto.c`) encoded, sent, received and decoded

## Run
```
../../../scripts/app ztest can-loopback                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest can-loopback qemu_riscv32
../../../scripts/app debug can-loopback qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/can-loopback/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
WeAct G431/H562/H743: set `zephyr,canbus` to `&fdcan1`, add a transceiver (e.g. SN65HVD230); use `CAN_MODE_NORMAL` on a real bus.

## Expected output
`6 of 6 executed test cases passed` in the twister summary.
