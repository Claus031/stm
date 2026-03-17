param(
    [int]$BaudRate = 115200,
    [string]$Port,
    [int]$PollMs = 20
)

$ErrorActionPreference = 'Stop'

function Get-DetectedPort {
    param([string]$ExplicitPort)

    if (-not [string]::IsNullOrWhiteSpace($ExplicitPort)) {
        return $ExplicitPort
    }

    [string[]]$ports = [System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object | Select-Object -Unique

    if ($ports.Length -eq 0) {
        throw 'No COM ports detected. Connect a USB cable to the micro-USB port on the STM32F4Discovery and try again.'
    }

    if ($ports.Length -eq 1) {
        return $ports[0]
    }

    $portList = $ports -join ', '
    throw "Multiple COM ports found ($portList). Re-run with -Port COMx to specify which one."
}

$selectedPort = Get-DetectedPort -ExplicitPort $Port
Write-Host "Opening $selectedPort at $BaudRate baud..."

$maxRetries = 5
$retryCount = 0
$sp = $null

while ($retryCount -lt $maxRetries) {
    try {
        $sp = New-Object System.IO.Ports.SerialPort(
            $selectedPort,
            $BaudRate,
            [System.IO.Ports.Parity]::None,
            8,
            [System.IO.Ports.StopBits]::One
        )
        $sp.Handshake = [System.IO.Ports.Handshake]::None
        $sp.ReadTimeout = 100
        $sp.Open()
        break
    }
    catch {
        $retryCount++
        if ($retryCount -lt $maxRetries) {
            Write-Host "Port locked (attempt $retryCount/$maxRetries). Waiting 3 seconds..." -ForegroundColor Yellow
            if ($sp) { $sp.Dispose() }
            [System.GC]::Collect()
            [System.GC]::WaitForPendingFinalizers()
            Start-Sleep -Seconds 3
        }
        else {
            Write-Host "Failed to open $selectedPort after $maxRetries attempts." -ForegroundColor Red
            Write-Host "Error: $_" -ForegroundColor Red
            Write-Host "Try closing all PowerShell windows and restarting." -ForegroundColor Yellow
            exit 1
        }
    }
}

Write-Host 'Serial monitor started. Press Ctrl+C to stop.'

try {
    while ($true) {
        if ($sp.BytesToRead -gt 0) {
            Write-Host -NoNewline $sp.ReadExisting()
        }
        Start-Sleep -Milliseconds $PollMs
    }
}
finally {
    if ($sp.IsOpen) {
        $sp.Close()
    }
    Write-Host "`nClosed $selectedPort."
}
