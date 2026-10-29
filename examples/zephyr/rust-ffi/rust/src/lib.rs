//! no_std Rust functions with a C ABI (declared in ../include/zffi.h), linked into a Zephyr C app.
#![no_std]

use core::panic::PanicInfo;
use core::slice;

/// A panic in Rust code ends up here: call into Zephyr's fatal error path through the C ABI.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    extern "C" {
        fn k_panic_from_rust() -> !;
    }
    unsafe { k_panic_from_rust() }
}

/// CRC-16/CCITT-FALSE ("123456789" -> 0x29b1).
#[no_mangle]
pub extern "C" fn zffi_crc16(data: *const u8, len: usize) -> u16 {
    if data.is_null() {
        return 0xffff;
    }
    let bytes = unsafe { slice::from_raw_parts(data, len) };
    let mut crc: u16 = 0xffff;
    for &b in bytes {
        crc ^= (b as u16) << 8;
        for _ in 0..8 {
            crc = if crc & 0x8000 != 0 { (crc << 1) ^ 0x1021 } else { crc << 1 };
        }
    }
    crc
}

/// Adds without wrapping: false (and *out untouched) on overflow.
#[no_mangle]
pub extern "C" fn zffi_checked_add(a: u32, b: u32, out: *mut u32) -> bool {
    match a.checked_add(b) {
        Some(v) if !out.is_null() => {
            unsafe { *out = v };
            true
        }
        _ => false,
    }
}

/// PI controller state, shared with C (same layout as struct zffi_pi in zffi.h).
#[repr(C)]
pub struct Pi {
    pub kp: f32,
    pub ki: f32,
    pub integral: f32,
    pub out_min: f32,
    pub out_max: f32,
}

/// One step: returns the clamped output; the integral stops growing while the output saturates.
#[no_mangle]
pub extern "C" fn zffi_pi_step(pi: *mut Pi, error: f32, dt: f32) -> f32 {
    let pi = match unsafe { pi.as_mut() } {
        Some(p) => p,
        None => return 0.0,
    };
    let candidate = pi.integral + error * dt;
    let out = pi.kp * error + pi.ki * candidate;
    if out > pi.out_max {
        pi.out_max
    } else if out < pi.out_min {
        pi.out_min
    } else {
        pi.integral = candidate;
        out
    }
}
