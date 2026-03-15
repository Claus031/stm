$ErrorActionPreference = 'Stop'

cmake --build build --target lint

$clangTidy = Get-Command clang-tidy -ErrorAction SilentlyContinue
if ($null -ne $clangTidy) {
    clang-tidy -p build src/main.c src/main_all_leds.c
} else {
    Write-Host 'clang-tidy not found; skipping optional clang-tidy run.'
}
