param(
    [string]$AdbPath = "$env:LOCALAPPDATA\Android\Sdk\platform-tools\adb.exe",
    [string]$Action = "status"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $AdbPath)) {
    Write-Host "[ERROR] ADB binary not found at $AdbPath" -ForegroundColor Red
    exit 1
}

switch ($Action) {
    "connect" {
        Write-Host "Connecting to WSA instance..." -ForegroundColor Cyan
        & $AdbPath connect 127.0.0.1:58526
    }
    "devices" {
        & $AdbPath devices
    }
    "restart" {
        Write-Host "Restarting ADB server..." -ForegroundColor Cyan
        & $AdbPath kill-server
        & $AdbPath start-server
    }
    default {
        Write-Host "Usage: .\adb_manage.ps1 -Action [connect|devices|restart]" -ForegroundColor Yellow
    }
}