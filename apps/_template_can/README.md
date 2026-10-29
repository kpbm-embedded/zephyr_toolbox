# __name__

Zephyr CAN node (from `apps/_template_can`: `scripts/app new <name> --template can`): a heartbeat frame every
second and a command frame it obeys, on the controller chosen as `zephyr,canbus`.

| Path | What |
|---|---|
| `lib/canio/` | node protocol: heartbeat `0x700+node` (state, uptime s), command `0x600+node` (`[op, arg]`), RX filter callback -> message queue -> `canio_poll()` |
| `src/main.c` | heartbeat timer, command loop |
| `boards/qemu_*.overlay` | qemu: Zephyr's loopback CAN controller as `zephyr,canbus` (frames come back to the node itself) |
| `tests/ztest/` | in loopback mode: heartbeat content, commands, filtering |

On a board with CAN (WeAct G431/H562/H743: FDCAN1 + a transceiver such as SN65HVD230) set `zephyr,canbus`
to it in `boards/<board>.overlay` if the board DTS does not, and enable its pins.

```sh
scripts/app ztest __name__
scripts/app build __name__ Debug qemu_cortex_m3
```
