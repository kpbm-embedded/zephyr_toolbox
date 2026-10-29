# source-able in bash and zsh
if [ -n "${BASH_SOURCE[0]:-}" ]; then _zs="${BASH_SOURCE[0]}"; elif [ -n "${ZSH_VERSION:-}" ]; then _zs="${(%):-%x}"; else _zs="$0"; fi
ZEPHYR_ROOT="$(cd -P "$(dirname "$_zs")" && pwd -P)"; unset _zs   # -P: via mcu's tool/zephyr symlink it must still resolve to this ID
export ZEPHYR_ROOT
# host layer (cmake, ninja, uv, CPython): this ID's own copy, ../host (self-contained)
. "$ZEPHYR_ROOT/../host/env.sh"
export UV_PYTHON_INSTALL_DIR="$ZEPHYR_ROOT/python"   # zephyr venv was created from this interpreter
. "$ZEPHYR_ROOT/.venv/bin/activate"
export ZEPHYR_BASE="$ZEPHYR_ROOT/ws/zephyr"
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR="$ZEPHYR_ROOT/sdk/zephyr-sdk"   # stable symlink; mcu update flips it to the SDK dir Zephyr needs
export PATH="$ZEPHYR_SDK_INSTALL_DIR/hosttools/sysroots/x86_64-pokysdk-linux/usr/bin:$PATH"
