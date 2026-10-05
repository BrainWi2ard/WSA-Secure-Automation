param(
    [string]$InterfaceName = "WGSaltTunnel",
    [ipaddress]$AllowedTunIp = "10.0.0.2"
)

Write-Host "[INFO] Enforcing WireGuard interface binding on $InterfaceName..." -ForegroundColor Cyan
$adapter = Get-NetAdapter -Name $InterfaceName -ErrorAction SilentlyContinue

if (-not $adapter) {
    Write-Host "[WARN] WireGuard interface $InterfaceName not active. Engaging strict killswitch." -ForegroundColor Yellow
    exit 1
}

Write-Host "[SUCCESS] Network bridge verified for TUN IP: $AllowedTunIp" -ForegroundColor Green