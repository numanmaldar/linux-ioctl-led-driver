# Linux ioctl LED Driver

A Linux character device driver written as a kernel module, simulating a virtual LED controlled via custom `ioctl` commands. Built to learn kernel-space fundamentals: device registration, the kernel/user-space boundary, and `ioctl`-based control — the same pattern used in most real hardware drivers.

## What it does

- Registers a character device at `/dev/myled`
- Exposes three custom `ioctl` commands:
  - `LED_ON` — turns the virtual LED on (sets kernel-space state to 1)
  - `LED_OFF` — turns it off (sets state to 0)
  - `LED_GET` — reads the current state back to user-space via `copy_to_user`
- Logs open/close and state changes to the kernel log (`dmesg`)
- Includes a user-space C test program that exercises all three commands

## Files

| File | Purpose |
|---|---|
| `ioctl_led.c` | Kernel module — device registration, `ioctl` handler, module init/exit |
| `ioctl_led.h` | Shared header defining the `ioctl` command codes |
| `test_led.c` | User-space program that opens `/dev/myled` and calls each `ioctl` command |
| `Makefile` | Builds the kernel module against the running kernel's build tree |

## Build and run

Requires Linux with kernel headers installed (tested on Ubuntu 24.04, kernel 7.0.0).

Install build dependencies:

    sudo apt install build-essential linux-headers-$(uname -r)

Build the kernel module:

    make

Load it:

    sudo insmod ioctl_led.ko
    sudo dmesg | tail

Confirm the device node was created:

    ls -l /dev/myled

Build and run the test program:

    gcc test_led.c -o test_led
    sudo ./test_led

Check kernel log for state changes:

    sudo dmesg | tail

Unload when done:

    sudo rmmod ioctl_led

## Expected output

    Turning LED ON...
    LED state: 1
    Turning LED OFF...
    LED state: 0

## What I learned

Kernel APIs shift across versions in ways user-space code rarely does. While building this on a newer kernel (7.0.0), I hit two real compile errors that don't show up in most older tutorials:

- `_IO()` only takes 2 arguments — reading data back needs `_IOR()` instead
- `class_create()` dropped its `THIS_MODULE` argument in newer kernels, now taking just the class name

Debugging these meant reading the actual kernel header source (`ioctl.h`, `class.h`) to see the current function signatures, rather than trusting an older guide verbatim.

## Author

Numan Maldar — [github.com/numanmaldar](https://github.com/numanmaldar)
