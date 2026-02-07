# Qemu Calculator

This project implements a simple calculator that can perform addition on both integers and floating-point numbers. The calculator runs on a custom OS environment and interacts with the user via a UART interface.

## Project Structure

-   `user/main.c`: The main application logic, which prompts the user for numbers, performs calculations, and prints the results.
-   `os/`: Contains the basic operating system functionalities, including UART communication (`os.c`, `os.h`) and the assembly root file (`root.s`).
-   `lib/`: Provides standard library functions for I/O (`stdio.c`, `stdio.h`) and string manipulation (`string.c`, `string.h`).
-   `Files/`: Contains the compiled binary (`calculadora.bin`), ELF file (`calculadora.elf`), object files, and the linker script.

## Building and Running

The project can be built and run using the `build_and_run.sh` script located in the `Files/` directory.

```bash
./Files/build_and_run.sh
```

This script compiles the source files, links them, and runs the resulting binary in a QEMU environment.

## `lib/stdio.c` Documentation

The `lib/stdio.c` file provides custom implementations of standard input/output functions. These functions are designed to work with the underlying UART interface provided by the OS.

### `PRINT(const char *f, ...)`

This function is a simplified version of `printf`. It can format and print strings, integers (`%d`), and floating-point numbers (`%f`).

**Parameters:**

-   `f`: A format string that can contain text and format specifiers.
-   `...`: A variable number of arguments corresponding to the format specifiers in the format string.

**Example:**

```c
PRINT("The sum of %d and %f is %f\n", 10, 3.14, 13.14);
```

### `READ(const char *f, ...)`

This function is a simplified version of `scanf`. It can read and parse integers (`%d`) and floating-point numbers (`%f`) from the UART input.

**Parameters:**

-   `f`: A format string that specifies the type of data to read.
-   `...`: A variable number of pointer arguments where the read data will be stored.

**Example:**

```c
int my_int;
float my_float;
READ("%d", &my_int);
READ("%f", &my_float);
```

### Static Helper Functions

-   **`stof(const char *s)`**: Converts a string to a `float`. This function handles an optional sign, an integer part, and a decimal part.
-   **`ftos(float value)`**: Converts a `float` to a string and prints it to the UART output. It handles the sign, integer part, and a fixed precision of 4 decimal places.
