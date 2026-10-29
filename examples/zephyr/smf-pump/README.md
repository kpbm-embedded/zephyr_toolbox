# zephyr / smf-pump

State Machine Framework: a hierarchical pump controller.

Status: passes on qemu_cortex_m3 and qemu_riscv32 under twister (`scripts/app ztest`); not run on hardware (no board attached). Zephyr v4.4.2, out-of-tree app (`find_package(Zephyr)`).

## What it shows
- parent state RUNNING with children PRIME and PUMP (`CONFIG_SMF_ANCESTOR_SUPPORT`)
- initial transition: entering RUNNING enters PRIME (`CONFIG_SMF_INITIAL_TRANSITION`)
- children return `SMF_EVENT_PROPAGATE` so the parent handles stop/fault; sibling transitions keep the parent
- the test records every entry/exit (`I> I< R> P>`) and checks the order; `smf_set_terminate` ends the machine

## Run
```
../../../scripts/app ztest smf-pump                  # twister on qemu_cortex_m3 (build, boot, parse the ztest output)
../../../scripts/app ztest smf-pump qemu_riscv32
../../../scripts/app debug smf-pump qemu_cortex_m3   # the image under gdb
```
Report: `$ZEPHYR_OUT/twister/smf-pump/<board>/twister.xml`; a failing case fails the run (exit 1).

## On a board
Pure logic; runs the same on any board.

## Expected output
`7 of 7 executed test cases passed` in the twister summary.
