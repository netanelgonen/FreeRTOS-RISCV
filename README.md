# FreeRTOS for RISC-V

This is a port of FreeRTOS to RISC-V

## Contributors
The original port to priv spec 1.7 was contributed by [Technolution](https://interactive.freertos.org/hc/en-us/community/posts/210030246-32-bit-and-64-bit-RISC-V-using-GCC)

Update to priv spec 1.9: [illustris](https://github.com/illustris)

Update to priv spec 1.9.1: [Abhinaya Agrawal](https://bitbucket.org/casl/freertos-riscv-v191/src)

Bug fixes: [Julio Gago](https://github.com/julio-gago-metempsy)

Update to priv spec 1.10: [sherrbc1](https://github.com/sherrbc1)

Update for the SSITH P1 on the Xilinx VCU118: Galois, Inc.

## Demo applications
`main()` in main.c contains demo tasks testing queue functions within tasks, 
counting semaphores, and recursive mutexes.

`main()` in test_uart.c contains loopback tests to be run with the UART driver
unit test.

The UART drivers have been updated for the Xilinx AXI UART 16550.
The drivers do not handle interrupts.

## Build
To build FreeRTOS,
```bash
cd Demo/p1-besspin
make
```

To build a version of FreeRTOS to use with the UART driver unit test,
```bash
make MAIN_FILE=test_uart.c
```

Your `$RISCV` variable should be set to the location of `riscv-tools`.

## Run
This can be run in gdb.

Using a serial port communication program such as minicom, you should see:
```bash
[0] All threads still alive! 
[1] All threads still alive! 
[2] All threads still alive!
[3] All threads still alive!
...
```
This indicates that the demo tasks are running.

## Tested environments

This has been tested on the Chisel P1 on the VCU118.
