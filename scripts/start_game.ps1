Param(
    [switch]$Headless
)

$vcxpath = 'C:\Program Files\VcXsrv\vcxsrv.exe'

function Start-VcXsrv {
    if (Test-Path $vcxpath) {
        $proc = Get-Process vcxsrv -ErrorAction SilentlyContinue
        if (-not $proc) {
            Start-Process -FilePath $vcxpath -ArgumentList ':0 -multiwindow -ac -nowgl -silent-dup-error' -WindowStyle Hidden -PassThru | Out-Null
            Start-Sleep -Milliseconds 500
        }
        else {
            Write-Output "VcXsrv já rodando (PID: $($proc.Id))."
        }
    }
    else {
        Write-Error "VcXsrv não encontrado em: $vcxpath`nInstale via winget: winget install --id marha.VcXsrv -e"
        exit 1
    }
}

# Se o script estiver em scripts/, sobe um nível para a raiz do projeto
$scriptDir = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent
Set-Location (Resolve-Path (Join-Path $scriptDir ".."))

if (-not $Headless) {
    Start-VcXsrv
    Write-Output "Iniciando o jogo (GUI). Pressione Ctrl+C para sair..."
    docker compose run --rm -e DISPLAY=host.docker.internal:0.0 minddrop
}
else {
    Write-Output "Executando teste headless (xvfb-run)..."
    docker compose run --rm minddrop bash -lc "timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true"
}
