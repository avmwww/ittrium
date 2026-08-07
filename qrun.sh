#!/bin/bash

export PATH=/opt/xilinx/Vitis/2023.2/gnu/aarch64/lin/aarch64-none/bin:$PATH
cd example/qemu-a53 && make run

