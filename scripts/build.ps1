$ErrorActionPreference = 'Stop'

cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build
