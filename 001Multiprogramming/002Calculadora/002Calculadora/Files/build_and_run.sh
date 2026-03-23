#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Remove previous compiled objects and binaries
echo "Cleaning up previous build files..."
rm -f root.o main.o stdio.o string.o os.o calculadora.elf calculadora.bin

echo "Assembling startup.s..."
arm-none-eabi-as -o root.o ../os/root.s 

echo "Compiling main.c..."
arm-none-eabi-gcc -c -o main.o ../user/main.c

echo "Compiling stdio.c..."
arm-none-eabi-gcc -c -o stdio.o ../lib/stdio.c

echo "Compiling string.c..."
arm-none-eabi-gcc -c -o string.o ../lib/string.c

echo "Compiling os.c..."
arm-none-eabi-gcc -c -o os.o ../os/os.c

echo "Linking object files..."
#arm-none-eabi-ld -T linker.ld -o calculadora.elf ./program/root.o ./program/main.o ./program/os.o ./lib/stdio.o
arm-none-eabi-gcc -T linker.ld -nostdlib -Wl,--gc-sections \
  -o calculadora.elf \
  root.o main.o os.o stdio.o \
  -lgcc

echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary calculadora.elf calculadora.bin

echo "Running QEMU..."
qemu-system-arm -M versatilepb -nographic -kernel calculadora.elf
