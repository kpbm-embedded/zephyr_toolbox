# __name__

Zephyr app that samples periodically on its own work queue and hands the samples to a consumer through a
ring buffer (from `apps/_template_workq`: `scripts/app new <name> --template workq`).

| Path | What |
|---|---|
| `lib/sampler/` | `sampler_start(period, read_fn)`: a delayable work item that reschedules itself on a private work queue; samples go into a `ring_buf`; overflow is counted, not blocking |
| `src/main.c` | a consumer thread drains the buffer every 200 ms |
| `tests/ztest/` | sample rate, stop, overflow |

```sh
scripts/app ztest __name__
scripts/app build __name__ Debug qemu_cortex_m3
```

Timers, cancel and debounce patterns: `examples/zephyr/workq-timers`.
