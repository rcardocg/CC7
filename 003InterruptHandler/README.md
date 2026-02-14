# 003InterruptHandler - BeagleBone Black Bare-Metal Timer Interrupt Example

This project demonstrates a bare-metal application for the BeagleBone Black (ARM Cortex-A8) that utilizes a timer interrupt. The code is written in a combination of ARM Assembly and C, providing a foundational example for handling hardware interrupts in an embedded environment.

## Features

*   **Bare-Metal Execution:** Runs directly on the ARM Cortex-A8 processor without an operating system.
*   **UART Communication:** Basic serial communication functions for debugging and output.
*   **DMTIMER2 Interrupts:** Configures and handles interrupts from the on-chip DMTIMER2.
*   **QEMU Emulation:** Includes a build script to compile and run the application within QEMU, emulating a BeagleBone Black.

## Project Structure

*   `Code/`: Contains all source code and build scripts.
    *   `OS/`:
        *   `os.c`: C source file containing UART communication functions and timer control logic.
        *   `os.h`: Header file for `os.c` and assembly functions.
        *   `root.s`: ARM Assembly source file with the exception vector table, `_start` routine, and IRQ handler.
    *   `linker.ld`: GNU Linker script for memory mapping.
    *   `build_and_run.sh`: Script to compile, link, and execute the project in QEMU.
    *   `bin/`: Directory for compiled object files and the final executable.

