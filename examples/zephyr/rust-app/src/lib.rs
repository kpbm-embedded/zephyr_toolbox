// Rust on Zephyr: rust_main() starts a worker thread, sends it numbers over a bounded channel (send blocks while it is full), gets the result
// back on a second channel, checks a CRC and prints PASS/FAIL for twister's console harness.
// Channels backed by Zephyr queues cannot be freed yet (zephyr-lang-rust panics when the last end is
// dropped): the worker serves forever and rust_main forgets its ends.
#![no_std]

extern crate alloc;

use zephyr::printkln;
use zephyr::sync::channel::{self, Receiver, Sender};

use zephyr_panic as _;

/// CRC-16/CCITT-FALSE, the same function the C templates use ("123456789" -> 0x29b1).
fn crc16(data: &[u8]) -> u16 {
    let mut crc: u16 = 0xffff;
    for &b in data {
        crc ^= (b as u16) << 8;
        for _ in 0..8 {
            crc = if crc & 0x8000 != 0 { (crc << 1) ^ 0x1021 } else { crc << 1 };
        }
    }
    crc
}

/// Sums the squares of what arrives on `rx`; a 0 sends the sum back and starts a new round.
#[zephyr::thread(stack_size = 2048)]
fn worker(rx: Receiver<u32>, tx: Sender<u32>) {
    let mut sum = 0u32;
    loop {
        let v = rx.recv().unwrap();
        if v == 0 {
            tx.send(sum).unwrap();
            sum = 0;
        } else {
            sum += v * v;
        }
    }
}

#[no_mangle]
extern "C" fn rust_main() {
    printkln!("rust-app on {}", zephyr::kconfig::CONFIG_BOARD);

    let (tx, rx) = channel::bounded::<u32>(2);
    let (result_tx, result_rx) = channel::bounded::<u32>(1);
    worker(rx, result_tx).start();
    for i in 1..=10u32 {
        tx.send(i).unwrap();
    }
    tx.send(0).unwrap();
    let sum = result_rx.recv().unwrap();
    printkln!("sum of squares 1..10 = {}", sum);

    let crc = crc16(b"123456789");
    printkln!("crc16 = {:#06x}", crc);

    let ok = sum == 385 && crc == 0x29b1;
    printkln!("rust-app: {}", if ok { "PASS" } else { "FAIL" });
    core::mem::forget(tx);
    core::mem::forget(result_rx);
}
