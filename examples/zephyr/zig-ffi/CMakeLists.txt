# C app + Zig code over the C ABI: zig (32_24_microzig, reached through scripts/app: $ZIG) compiles zig/*.zig
# to an object for the board's CPU, the object is linked into the Zephyr image, ztest calls it.
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zig_ffi)

if(NOT DEFINED ENV{ZIG})
  message(FATAL_ERROR "zig-ffi: ZIG is not set (scripts/app sets it from 32_24_microzig)")
endif()
# Zig target of the board: soft-float Cortex-M3 (qemu_cortex_m3) or RV32IMAC (qemu_riscv32)
if(CONFIG_RISCV)
  set(ZIG_TARGET riscv32-freestanding-none)
  set(ZIG_CPU generic_rv32+m+a+c)
elseif(CONFIG_CPU_CORTEX_M3)
  set(ZIG_TARGET thumb-freestanding-eabi)
  set(ZIG_CPU cortex_m3)
else()
  message(FATAL_ERROR "zig-ffi: no Zig target mapped for ${BOARD} (see CMakeLists.txt)")
endif()
set(ZIG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/zig/fixed.zig)
set(ZIG_OBJ ${CMAKE_BINARY_DIR}/zig/fixed.o)
add_custom_command(
  OUTPUT ${ZIG_OBJ}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/zig
  COMMAND $ENV{ZIG} build-obj -O ReleaseSmall -target ${ZIG_TARGET} -mcpu ${ZIG_CPU}
          -fno-stack-check -fno-unwind-tables -fno-error-tracing -fstrip -femit-bin=${ZIG_OBJ} ${ZIG_SRC}
  DEPENDS ${ZIG_SRC}
  COMMENT "zig build-obj ${ZIG_TARGET}"
  VERBATIM)
add_custom_target(zig_obj DEPENDS ${ZIG_OBJ})
add_dependencies(app zig_obj)
target_link_libraries(app PRIVATE ${ZIG_OBJ})

target_include_directories(app PRIVATE zig)
target_sources(app PRIVATE src/test_zig.c)
