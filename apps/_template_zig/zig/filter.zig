//! Exponential moving average in Q8 fixed point (alpha = shift), with min/max tracking.

pub const Ema = extern struct {
    value: i32,
    min: i32,
    max: i32,
    shift: u8,
    primed: bool,
};

export fn ema_init(e: *Ema, shift: u8) void {
    e.* = .{ .value = 0, .min = 0, .max = 0, .shift = shift, .primed = false };
}

/// value += (sample - value) / 2^shift; the first sample sets value directly
export fn ema_add(e: *Ema, sample: i32) i32 {
    if (!e.primed) {
        e.* = .{ .value = sample, .min = sample, .max = sample, .shift = e.shift, .primed = true };
        return sample;
    }
    e.value += (sample - e.value) >> @intCast(e.shift);
    e.min = @min(e.min, sample);
    e.max = @max(e.max, sample);
    return e.value;
}
