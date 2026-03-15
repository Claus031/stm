$ErrorActionPreference = 'Stop'

$openocd = Get-Command openocd -ErrorAction SilentlyContinue
if ($null -eq $openocd) {
    throw 'openocd is not installed or not on PATH.'
}

$configPath = Join-Path $PSScriptRoot 'openocd\stm32f4discovery.cfg'
& $openocd.Source -f $configPath -c "init" -c "reset halt"
