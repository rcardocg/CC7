# Log Processor - Windows Version

This is the Windows version of the log processor project, which analyzes access logs using multi-threading.

## Requirements

- Windows with MinGW or Visual Studio for compilation.
- An `access.log` file in the same directory.

## Compilation

Run `make` to build the executable.

## Usage

Run the generated `log_processor.exe` to process the log file and display statistics.

## Notes

This version uses Windows threads instead of POSIX threads.