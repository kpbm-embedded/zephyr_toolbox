//! Zig functions with a C ABI (declared in fixed.h), compiled to an object and linked into a Zephyr app.
//! Q16.16 fixed-point math (no FPU needed), a Fletcher-16 checksum and a bounded moving average.

/// Q16.16 multiply with saturation (i64 intermediate, no overflow UB)
export fn zf_q16_mul(a: i32, b: i32) i32 {
    const p: i64 = (@as(i64, a) * @as(i64, b)) >> 16;
    return @intCast(@max(@min(p, @as(i64, std_max)), @as(i64, std_min)));
}

/// Q16.16 from an integer and a thousandths fraction (3, 250 -> 3.25)
export fn zf_q16_from_milli(whole: i32, milli: i32) i32 {
    return (whole << 16) + @divTrunc(milli * 65536, 1000);
}

/// Fletcher-16 over len bytes
export fn zf_fletcher16(data: [*]const u8, len: usize) u16 {
    var s1: u16 = 0;
    var s2: u16 = 0;
    for (data[0..len]) |b| {
        s1 = (s1 + b) % 255;
        s2 = (s2 + s1) % 255;
    }
    return (s2 << 8) | s1;
}

/// Moving average over the last 4 values; the C side owns the struct (layout from extern struct)
pub const Avg = extern struct {
    buf: [4]i32,
    n: u32,
    pos: u32,
};

export fn zf_avg_add(avg: *Avg, v: i32) i32 {
    avg.buf[avg.pos] = v;
    avg.pos = (avg.pos + 1) % 4;
    if (avg.n < 4) avg.n += 1;
    var sum: i64 = 0;
    for (avg.buf[0..avg.n]) |x| sum += x;
    return @intCast(@divTrunc(sum, @as(i64, avg.n)));
}

const std_max: i32 = 0x7fffffff;
const std_min: i32 = -0x80000000;
