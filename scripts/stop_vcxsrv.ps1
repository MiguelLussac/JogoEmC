$vcxpath = 'C:\Program Files\VcXsrv\vcxsrv.exe'
$proc = Get-Process vcxsrv -ErrorAction SilentlyContinue
if ($proc) {
    Write-Output "Parando VcXsrv (PID: $($proc.Id))..."
    $proc | Stop-Process -Force
    Start-Sleep -Milliseconds 200
    Write-Output "VcXsrv parado."
}
else {
    Write-Output "VcXsrv não está rodando."
}
