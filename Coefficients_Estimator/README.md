# Matrix Inversion Firmware

A firmware project for 3x3 matrix inversion using fixed-point arithmetic in Q4.12 format.

## Project Structure

```
.
├── src/
│   ├── mat_inv.c
│   └── main.c
├── inc/
│   ├── mat_inv.h
│   ├── main.h
│   ├── input_data.h
│   └── output_data.h
├── test/
│   ├── inc/
│   │   └── test.h
│   └── test.c
├── build/
│   └── test/
├── obj/
├── Makefile
└── README.md
```

## Prerequisites

- GCC compiler
- Make

## Build Commands

### Main Project
- Build the project:
  ```bash
  make
  ```
- Build with debug information:
  ```bash
  make debug
  ```
- Build with maximum optimization:
  ```bash
  make release
  ```

### Testing
- Build the test program:
  ```bash
  make test
  ```
- Build and run the tests:
  ```bash
  make run_tests
  ```

### Other Commands
- Clean build files:
  ```bash
  make clean
  ```
- Show size information:
  ```bash
  make size
  ```
- Generate dependencies:
  ```bash
  make deps
  ```
- Show help:
  ```bash
  make help
  ```

## Features

- Fixed-point arithmetic in Q4.12 format
- Optimized for embedded systems
- Minimal memory usage
- Inlined helper functions
- Comprehensive test suite
- Test data validation
- Detailed test reporting

## Test Suite

The test suite includes:
- 100 test matrices with known inverses
- Validation of matrix inversion results
- Tolerance checking for fixed-point arithmetic
- Detailed test reporting with pass/fail statistics

## License

[Add your license information here] 