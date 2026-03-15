# STM32F4Discovery Blink (CMake)

Minimal C firmware project for STM32F4Discovery with two test programs:

- `blink`: blinks orange LED on PD13.
- `blink_all_leds`: runs a pattern across PD12, PD13, PD14, and PD15.

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
- `build/blink_all_leds.elf`
- `build/blink_all_leds.hex`
- `build/blink_all_leds.bin`

## Lint

```powershell
cmake --build build --target lint
```

If `clang-tidy` is installed:

```powershell
clang-tidy -p build src/main.c
```

## Flash and Debug (OpenOCD + ST-Link)

Install OpenOCD, then use either CMake targets or scripts.

Flash with CMake targets:

```powershell
cmake --build build --target flash-blink
cmake --build build --target flash-all-leds
```

Start OpenOCD debug server with CMake target:

```powershell
cmake --build build --target debug-openocd
```

Alternative script usage:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/flash.ps1 -Target blink
powershell -ExecutionPolicy Bypass -File scripts/flash.ps1 -Target blink_all_leds
powershell -ExecutionPolicy Bypass -File scripts/flash.ps1 -Target stm32f4discovery
powershell -ExecutionPolicy Bypass -File scripts/debug-openocd.ps1
```

The OpenOCD board configuration is in `scripts/openocd/stm32f4discovery.cfg`.

## CI

GitHub Actions workflow `.github/workflows/ci.yml` runs on push and pull requests. It performs:

- recursive submodule checkout
- cross-toolchain dependency installation
- CMake configure and build
- cppcheck lint target
- artifact existence checks for both firmware targets
