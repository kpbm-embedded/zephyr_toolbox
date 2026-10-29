# __name__

Zephyr app whose behaviour is a flat state machine on SMF (from `apps/_template_smf`: `scripts/app new <name> --template smf`).
Events go into a message queue; one thread runs the machine, so state code never races.

| Path | What |
|---|---|
| `lib/fsm/` | states BOOT -> IDLE <-> ACTIVE, any -> ERROR -> IDLE on reset; `fsm_post()` / `fsm_step()` |
| `src/main.c` | the event thread and a short demo sequence |
| `tests/ztest/` | transition table tests, driven with `fsm_step()` (no thread) |

```sh
scripts/app ztest __name__
scripts/app build __name__ Debug qemu_cortex_m3
```

Parent states, initial transitions and propagation: `examples/zephyr/smf-pump`.
