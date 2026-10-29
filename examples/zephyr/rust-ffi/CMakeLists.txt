# C app + a no_std Rust static library over the C ABI (no zephyr-lang-rust, no bindgen): cargo builds
# rust/ for the board's Rust target, the library is linked into the Zephyr image, ztest calls it.
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rust_ffi)

# Rust target of the board: soft-float Cortex-M3 (qemu_cortex_m3) or RV32IMAC (qemu_riscv32)
if(CONFIG_RISCV)
  set(RUST_TARGET riscv32imac-unknown-none-elf)
elseif(CONFIG_CPU_CORTEX_M3)
  set(RUST_TARGET thumbv7m-none-eabi)
else()
  message(FATAL_ERROR "rust-ffi: no Rust target mapped for ${BOARD} (see CMakeLists.txt)")
endif()
find_program(CARGO cargo REQUIRED)   # scripts/app puts 32_17_embedded_rust's cargo on PATH
set(RUST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rust)
set(RUST_LIB ${CMAKE_BINARY_DIR}/rust/${RUST_TARGET}/release/libzffi.a)
add_custom_command(
  OUTPUT ${RUST_LIB}
  COMMAND ${CARGO} build --release --locked --target ${RUST_TARGET}
          --manifest-path ${RUST_DIR}/Cargo.toml --target-dir ${CMAKE_BINARY_DIR}/rust
  DEPENDS ${RUST_DIR}/Cargo.toml ${RUST_DIR}/Cargo.lock ${RUST_DIR}/src/lib.rs
  COMMENT "cargo build ${RUST_TARGET}"
  VERBATIM)
add_custom_target(zffi DEPENDS ${RUST_LIB})
add_dependencies(app zffi)
target_link_libraries(app PRIVATE ${RUST_LIB})

target_include_directories(app PRIVATE rust/include)
target_sources(app PRIVATE src/test_ffi.c)
