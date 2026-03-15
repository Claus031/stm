# STM32F4Discovery Blink (CMake)

Minimal C firmware project for STM32F4Discovery that blinks the orange LED (PD13).

## Requirements

- CMake
- Ninja
- GNU Arm Embedded Toolchain (`arm-none-eabi-*`)
- cppcheck

## Configure and Build

```powershell
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Artifacts:

- `build/blink.elf`
- `build/blink.hex`
- `build/blink.bin`

## Lint

```powershell
cmake --build build --target lint
```

If `clang-tidy` is installed:

```powershell
clang-tidy -p build src/main.c
```
