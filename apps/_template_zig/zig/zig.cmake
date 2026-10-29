# zig_object(<target> <source.zig>): compile with $ENV{ZIG} for the board's CPU and link into <target>
function(zig_object tgt src)
  if(NOT DEFINED ENV{ZIG})
    message(FATAL_ERROR "ZIG is not set (scripts/app sets it from 32_24_microzig)")
  endif()
  if(CONFIG_RISCV)
    set(zt riscv32-freestanding-none)
    set(zc generic_rv32+m+a+c)
  elseif(CONFIG_CPU_CORTEX_M3)
    set(zt thumb-freestanding-eabi)
    set(zc cortex_m3)
  else()
    message(FATAL_ERROR "no Zig target mapped for ${BOARD}: add it in zig/zig.cmake")
  endif()
  get_filename_component(name ${src} NAME_WE)
  set(obj ${CMAKE_BINARY_DIR}/zig/${name}.o)
  add_custom_command(
    OUTPUT ${obj}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/zig
    COMMAND $ENV{ZIG} build-obj -O ReleaseSmall -target ${zt} -mcpu ${zc}
            -fno-stack-check -fno-unwind-tables -fno-error-tracing -fstrip -femit-bin=${obj} ${src}
    DEPENDS ${src}
    VERBATIM)
  add_custom_target(zig_${name} DEPENDS ${obj})
  add_dependencies(${tgt} zig_${name})
  target_link_libraries(${tgt} PRIVATE ${obj})
endfunction()
