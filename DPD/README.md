# Digital Pre-Distortion (DPD) Firmware

A firmware project for Digital Pre-Distortion implementation using fixed-point arithmetic.

## Project Structure

```
.
├── src/
│   ├── actuator.c
│   ├── perf_cnt.c
│   └── main.c
├── inc/
│   ├── actuator.h
│   ├── fixed.h
│   ├── perf_cnt.h
│   ├── main.h
│   ├── input_r.h
│   ├── input_i.h
│   ├── expected_r.h
│   └── expected_i.h
├── build/
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

- Fixed-point arithmetic implementation
- Performance counter support
- Actuator control interface

## License

[Add your license information here] 