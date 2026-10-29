# __name__

Zephyr app that reads a temperature sensor through the sensor API (from `apps/_template_sensor`:
`scripts/app new <name> --template sensor`). The sensor is found by the devicetree alias `ambient-temp0`,
so changing the chip is an overlay change only.

| Path | What |
|---|---|
| `lib/temp/` | `temp_read_mdeg()` (fetch + get, milli-degrees) and a moving-average filter |
| `src/main.c` | reads `ambient-temp0` once a second, prints raw and filtered values |
| `boards/qemu_*.overlay` | qemu: an emulated I2C bus with an F75303 (Zephyr driver + emulator) as `ambient-temp0` |
| `tests/ztest/` | sets temperatures through the emulator backend, checks reads and the filter |

On a board add `boards/<board>.overlay` with the real sensor under its I2C bus and the alias, e.g. BlackPill:
`&i2c1 { status = "okay"; temp: tmp117@48 { compatible = "ti,tmp11x"; reg = <0x48>; }; };` and
`/ { aliases { ambient-temp0 = &temp; }; };`.

```sh
scripts/app ztest __name__
scripts/app build __name__ Debug qemu_cortex_m3
```
