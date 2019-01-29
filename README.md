# FreeRTOS for RISC-V

This is a port of FreeRTOS to RISC-V

This has been tested on the Chisel P1 on the VCU118.

## Contributors
The original port to priv spec 1.7 was contributed by [Technolution](https://interactive.freertos.org/hc/en-us/community/posts/210030246-32-bit-and-64-bit-RISC-V-using-GCC)

Update to priv spec 1.9: [illustris](https://github.com/illustris)

Update to priv spec 1.9.1: [Abhinaya Agrawal](https://bitbucket.org/casl/freertos-riscv-v191/src)

Bug fixes: [Julio Gago](https://github.com/julio-gago-metempsy)

Update to priv spec 1.10: [sherrbc1](https://github.com/sherrbc1)


## Install
You need a modified spike simulator with UART patch. There is no easy way to install it, we suggest using a modified version of riscv-tools:

```bash
git clone --recursive -b uart_patch https://github.com/GasloiInc/riscv-tools.git
cd riscv-tools
```

Set up your `$RISCV` variable to some sensible location, for example:
```bash
export RISCV=~/Documents/riscv
```

Now build the spike simulator:
```bash
./build-spike-only.sh
```

If you need the other tools as well, run instead:
```bash
./build-rv32ima.sh
```

## Build
Go to `Demo/riscv-spike` and hit `make clean; make`. 

## Run

```bash
./run_spike.sh # will run rtos in the spike simulator
```

Note that spike uses tty for a simulated UART peripheral. In order to see any simulation output, you have to start `socat`:
```bash
./run_socat.sh # will display UART output
```

You should see:
```bash
$ ./run_socat.sh 
[0] Timer callback! 
[1] Timer callback! 
[2] Timer callback! 
[3] Timer callback! 
...
```
## UART

If you want to build a version with 16550 UART tests,
go to `Demo/p1-besspin` and hit `make clean; make MAIN_FILE=test_uart.c`
This version is made to run on hardware.

## Debug

If you want to debug spike, use `-H` flag that stops execution until a debugger is connected. Note that is stops the execution only until you connect `openocd`, so by the time you connect `gdb` it will be already running.

```bash
./run_spike.sh -H # add the -H flag
```

Start openocd:
```bash
./run_openocd.sh
```

And then start gdb:
```bash
riscv32-unknown-elf-gdb riscv-spike.elf
```

And connect to the target with:
```bash
(gdb) target remote localhost:3333
```

Happy debugging!

## Tested environments

Tested on a Rocket RISC-V processor with local interrupt controller (Clint) using preemption.

Tested in Spike and Verilator with several builds including single-task, multi-task and typical demo test including queues, semaphores, mutexes and about a dozen concurrent tasks.
