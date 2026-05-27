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
$projectRoot = Resolve-Path (Join-Path $scriptDir "..")
Set-Location $projectRoot

function Start-TrilhaHost {
    $candidatos = @(
        (Join-Path $projectRoot "resources\musica\trilha.mp3"),
        (Join-Path $projectRoot "musica\THE FLIPSIDE - boggio (youtube).mp3")
    )
    $trilha = $candidatos | Where-Object { Test-Path $_ } | Select-Object -First 1
    if (-not $trilha) { return $null }

    try {
        $player = New-Object -ComObject WMPlayer.OCX
        $player.settings.setMode("loop", $true)
        $player.settings.volume = 45
        $player.URL = (Resolve-Path $trilha).Path
        $player.controls.play()
        return $player
    }
    catch {
        Write-Output "Aviso: nao foi possivel iniciar trilha no host."
        return $null
    }
}

function Stop-TrilhaHost($player) {
    if ($null -eq $player) { return }
    try {
        $player.controls.stop()
        $player.close()
    }
    catch { }
}

if (-not $Headless) {
    Start-VcXsrv
    Write-Output "Atualizando imagem Docker com o codigo atual..."
    docker compose build minddrop
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Write-Output "Iniciando o jogo (GUI). Pressione Ctrl+C para sair..."
    $trilhaHost = Start-TrilhaHost
    try {
        docker compose run --rm -e DISPLAY=host.docker.internal:0.0 minddrop
    }
    finally {
        Stop-TrilhaHost $trilhaHost
    }
}
else {
    Write-Output "Executando teste headless (xvfb-run)..."
    docker compose build minddrop
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    docker compose run --rm minddrop bash -lc "timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true"
}
