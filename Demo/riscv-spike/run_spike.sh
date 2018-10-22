#!/bin/bash
spike --ttyS0=ttyS0 --isa=RV32IMA -m256 --rbb-port=9824  $* riscv-spike.elf
