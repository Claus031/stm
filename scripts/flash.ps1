param(
    [ValidateSet('blink', 'blink_all_leds')]
    [string]$Target = 'blink'
)

$ErrorActionPreference = 'Stop'

$openocd = Get-Command openocd -ErrorAction SilentlyContinue
if ($null -eq $openocd) {
    throw 'openocd is not installed or not on PATH.'
}

$elfPath = Join-Path $PSScriptRoot "..\build\$Target.elf"
if (-not (Test-Path $elfPath)) {
    throw "ELF file not found: $elfPath. Build the project first."
}

$configPath = Join-Path $PSScriptRoot 'openocd\stm32f4discovery.cfg'
& $openocd.Source -f $configPath -c "program $elfPath verify reset exit"
