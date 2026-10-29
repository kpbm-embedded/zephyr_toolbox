#![no_std]

extern crate alloc;

use zephyr::printkln;
use zephyr::sync::channel::{self, Receiver, Sender};
use zephyr::time::{sleep, Duration};

use zephyr_panic as _;

/// Doubles every value it receives and sends it back. Runs forever: channels backed by Zephyr queues
/// cannot be freed yet (dropping the last end panics).
#[zephyr::thread(stack_size = 2048)]
fn doubler(rx: Receiver<u32>, tx: Sender<u32>) {
    loop {
        let v = rx.recv().unwrap();
        tx.send(v * 2).unwrap();
    }
}

#[no_mangle]
extern "C" fn rust_main() {
    printkln!("__name__ on {}", zephyr::kconfig::CONFIG_BOARD);
    let (to_worker, worker_rx) = channel::bounded::<u32>(1);
    let (worker_tx, from_worker) = channel::bounded::<u32>(1);
    doubler(worker_rx, worker_tx).start();

    let mut total = 0;
    for i in 1..=3u32 {
        to_worker.send(i).unwrap();
        total += from_worker.recv().unwrap();
        sleep(Duration::millis_at_least(10));
    }
    printkln!("total = {}", total);
    printkln!("__name__: {}", if total == 12 { "PASS" } else { "FAIL" });
    core::mem::forget(to_worker);
    core::mem::forget(from_worker);
}
