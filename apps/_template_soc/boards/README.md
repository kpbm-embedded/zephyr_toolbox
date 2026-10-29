# Board overlays

Zephyr picks `boards/<board>.overlay` (devicetree) and `boards/<board>.conf` (Kconfig) automatically
for the board you build for. Examples here:

- `nanopi_neo.overlay`: enables a second GPIO bank (PG) for your own pins.
- `luckfox_lyra.conf`: console at 1.5 Mbaud needs nothing extra; shows where per-board Kconfig goes.

Board names: `../../../scripts/app boards`.
