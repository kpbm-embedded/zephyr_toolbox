# Rust app on Zephyr (zephyr-lang-rust): main is rust_main() in src/lib.rs, built by cargo as a static library.
# scripts/app adds the module (modules/lang-rust) and the Rust toolchain of 32_17_embedded_rust.
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rust_app)
rust_cargo_application()
