param(
    [ValidateSet('blink', 'blink_all_leds', 'stm32f4discovery')]
    [string]$Target = 'blink'
)

$ErrorActionPreference = 'Stop'

$openocd = Get-Command openocd -ErrorAction SilentlyContinue
if ($null -eq $openocd) {
    $openocdPath = Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages" -Filter openocd.exe -Recurse -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName

    if ([string]::IsNullOrWhiteSpace($openocdPath)) {
        throw 'openocd is not installed or not on PATH.'
    }

    $openocd = [pscustomobject]@{ Source = $openocdPath }
}

$resolvedTarget = if ($Target -eq 'stm32f4discovery') { 'blink' } else { $Target }

$elfPath = Join-Path $PSScriptRoot "..\build\$resolvedTarget.elf"
if (-not (Test-Path $elfPath)) {
    throw "ELF file not found: $elfPath. Build the project first."
}

$configPath = ((Resolve-Path (Join-Path $PSScriptRoot 'openocd\stm32f4discovery.cfg')).Path) -replace '\\', '/'
$elfPath = ((Resolve-Path $elfPath).Path) -replace '\\', '/'
& $openocd.Source -f $configPath -c "program $elfPath verify reset exit"
