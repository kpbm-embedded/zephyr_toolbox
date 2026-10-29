# __name__

Zephyr module template (from `apps/_template_module`: `scripts/app new <name> --template module`): an out-of-tree
driver with its own devicetree binding, Kconfig, public API header, a ztest suite and a sample. Use it for a
driver or library that several apps share (or that goes upstream later).

| Path | What |
|---|---|
| `zephyr/module.yml` | makes the folder a Zephyr module: CMake + Kconfig entry points, `dts_root` for `dts/bindings` |
| `CMakeLists.txt`, `Kconfig` | module root: adds `include/` and `drivers/` |
| `drivers/tally/` | the driver: `acme,tally` devices from devicetree (`DEVICE_DT_INST_DEFINE`), threshold callback |
| `include/tally/tally.h` | public API (`tally_add`, `tally_get`, `tally_reset`, `tally_set_callback`) |
| `dts/bindings/acme,tally.yaml` | binding: `threshold` (required), `initial` |
| `tests/ztest/` | two devicetree instances, API and callback tests (twister, qemu) |
| `samples/basic/` | an app using the module |

An app uses the module by adding it to `ZEPHYR_EXTRA_MODULES` (as `tests/ztest` and `samples/basic` do in their
CMakeLists.txt, or `-DZEPHYR_EXTRA_MODULES=<path>`), or through a west manifest entry.

```sh
scripts/app ztest <path>/__name__                                   # tests/ztest
scripts/app build <path>/__name__/samples/basic Debug qemu_cortex_m3
```

`ports/` of this ID is a larger module of the same shape (boards, SoCs, drivers).
