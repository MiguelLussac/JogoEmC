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

function Get-DockerComposeCommand {
    if (Get-Command docker -ErrorAction SilentlyContinue) {
        $composeTest = & docker compose version 2>$null
        if ($LASTEXITCODE -eq 0) {
            return @{Exe = 'docker'; Args = @('compose')}    
        }
    }
    if (Get-Command docker-compose -ErrorAction SilentlyContinue) {
        return @{Exe = 'docker-compose'; Args = @()}
    }
    return $null
}

# Se o script estiver em scripts/, sobe um nível para a raiz do projeto
$scriptDir = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent
Set-Location (Resolve-Path (Join-Path $scriptDir ".."))

$composeCommand = Get-DockerComposeCommand
if (-not $composeCommand) {
    Write-Error "Docker Compose não encontrado. Instale 'docker compose' ou 'docker-compose'."
    exit 1
}

if (-not $Headless) {
    Start-VcXsrv
    Write-Output "Atualizando imagem Docker com o codigo atual..."
    & $composeCommand.Exe @($composeCommand.Args + @('build', 'minddrop'))
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Write-Output "Iniciando o jogo (GUI). Pressione Ctrl+C para sair..."
    & $composeCommand.Exe @($composeCommand.Args + @('run', '--rm', '-e', 'DISPLAY=host.docker.internal:0.0', 'minddrop'))
}
else {
    Write-Output "Executando teste headless (xvfb-run)..."
    & $composeCommand.Exe @($composeCommand.Args + @('build', 'minddrop'))
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & $composeCommand.Exe @($composeCommand.Args + @('run', '--rm', 'minddrop', 'bash', '-lc', 'timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true'))
}
