#!/bin/bash
echo "exporting RISCV"

export PATH=/opt/riscv/bin:$PATH

export RISCV=/opt/riscv

export SW_HOME=$(pwd)/software

export HW_HOME=$(pwd)/hardware

echo "exporting QUESTASIM PATH"

export QUESTASIM_HOME=/tools/questasim/2022.1/questa_fe/bin

echo "exporting RISCV 32 bit with zfinx"

export PATH=/opt/riscv/bin:$PATH

echo "cloning submodules"

git submodule update --init --recursive
