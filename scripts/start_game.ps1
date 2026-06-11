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
$composeFile = Join-Path $projectRoot "docker\docker-compose.yml"
Set-Location $projectRoot

function Invoke-DockerCompose {
    param([Parameter(ValueFromRemainingArguments = $true)][string[]]$Args)
    docker compose -f $composeFile @Args
}

function Ensure-MciApi {
    if ("WinMci" -as [type]) { return }

    Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class WinMci {
    [DllImport("winmm.dll", CharSet=CharSet.Unicode)]
    public static extern int mciSendString(string command, StringBuilder buffer, int bufferSize, IntPtr hwnd);
}
"@
}

function Invoke-MciCommand {
    param(
        [string]$Command,
        [switch]$SilentError
    )

    $buffer = New-Object System.Text.StringBuilder 256
    $result = [WinMci]::mciSendString($Command, $buffer, 256, [IntPtr]::Zero)
    if ($result -ne 0 -and -not $SilentError) {
        [WinMci]::mciSendString("status error", $buffer, 256, [IntPtr]::Zero) | Out-Null
        Write-Output "Aviso MCI ($result) em '$Command': $($buffer.ToString())"
        return $false
    }

    return ($result -eq 0)
}

function Start-TrilhaHost {
    $trilha = Join-Path $projectRoot "resources\musica\trilha.mp3"
    if (-not (Test-Path $trilha)) {
        Write-Output "Aviso: nenhum arquivo de trilha encontrado em resources/musica."
        return $false
    }

    try {
        Ensure-MciApi
        $trilhaPath = (Resolve-Path $trilha).Path
        [void](Invoke-MciCommand -Command "close trilha" -SilentError)

        if (-not (Invoke-MciCommand -Command ('open "' + $trilhaPath + '" type mpegvideo alias trilha'))) {
            Write-Output "Aviso: nao foi possivel abrir a trilha no host."
            return $false
        }

        [void](Invoke-MciCommand -Command "setaudio trilha volume to 150")
        if (-not (Invoke-MciCommand -Command "play trilha repeat")) {
            [void](Invoke-MciCommand -Command "close trilha" -SilentError)
            Write-Output "Aviso: nao foi possivel iniciar reproducao da trilha no host."
            return $false
        }

        Write-Output "Trilha sonora iniciada no Windows: $trilhaPath"
        return $true
    }
    catch {
        Write-Output "Aviso: nao foi possivel iniciar trilha no host ($($_.Exception.Message))."
        return $false
    }
}

function Stop-TrilhaHost($trilhaAtiva) {
    if (-not $trilhaAtiva) { return }
    try {
        Ensure-MciApi
        [void](Invoke-MciCommand -Command "stop trilha" -SilentError)
        [void](Invoke-MciCommand -Command "close trilha" -SilentError)
        Write-Output "Trilha sonora encerrada."
    }
    catch { }
}

function Play-TiroHost {
    $sfx = Join-Path $projectRoot "resources\musica\freesound_community-shoot-5-102360.mp3"
    if (-not (Test-Path $sfx)) { return }

    try {
        Ensure-MciApi
        $sfxPath = (Resolve-Path $sfx).Path
        [void](Invoke-MciCommand -Command "close sfx_tiro" -SilentError)
        if (-not (Invoke-MciCommand -Command ('open "' + $sfxPath + '" type mpegvideo alias sfx_tiro'))) { return }
        [void](Invoke-MciCommand -Command "setaudio sfx_tiro volume to 400")
        [void](Invoke-MciCommand -Command "play sfx_tiro" -SilentError)
    }
    catch { }
}

if (-not $Headless) {
    Start-VcXsrv
    Write-Output "Atualizando imagem Docker com o codigo atual..."
    Invoke-DockerCompose build minddrop
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Write-Output "Iniciando o jogo (GUI). Pressione Ctrl+C para sair..."
    $trilhaHost = $false
    try {
        Invoke-DockerCompose run --rm -e DISPLAY=host.docker.internal:0.0 minddrop 2>&1 | ForEach-Object {
            $line = $_.ToString()
            Write-Output $line
            if ($line -match '\[MINDDROP\] MUSIC:START') {
                if (-not $trilhaHost) {
                    $trilhaHost = Start-TrilhaHost
                }
            }
            elseif ($line -match '\[MINDDROP\] MUSIC:STOP') {
                Stop-TrilhaHost $trilhaHost
                $trilhaHost = $false
            }
            elseif ($line -match '\[MINDDROP\] SFX:SHOOT') {
                Play-TiroHost
            }
        }
    }
    finally {
        Stop-TrilhaHost $trilhaHost
    }
}
else {
    Write-Output "Executando teste headless (xvfb-run)..."
    Invoke-DockerCompose build minddrop
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Invoke-DockerCompose run --rm minddrop bash -lc "timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true"
}
