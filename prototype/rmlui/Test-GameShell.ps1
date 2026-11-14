# Local Codex / Computer Use smoke-test for the Knomira RmlUi shell.
# Examples:
# powershell -NoProfile -ExecutionPolicy Bypass -File .\prototype\rmlui\Test-GameShell.ps1
# powershell -NoProfile -ExecutionPolicy Bypass -File .\prototype\rmlui\Test-GameShell.ps1 -Install -Screenshot
[CmdletBinding()]
param(
    [switch]$Install,
    [switch]$Screenshot,
    [ValidateRange(1,30)][int]$StartupSeconds = 4
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$branch = [string](& git -C $root branch --show-current)
if ($LASTEXITCODE -ne 0) { throw 'Unable to identify local Knomira Git checkout.' }
$branch = $branch.Trim()
if ($branch -ne 'feature/controls-menu-redesign') {
    throw "Expected feature/controls-menu-redesign, found '$branch'. Switch explicitly; this script does not modify branches."
}

$folder = Join-Path $root 'build\rmlui-prototype\desktop-test-controls-v2'
$exe = Join-Path $folder 'knomira_game_shell.exe'

if ($Install) {
    Write-Host 'Installing verified Windows game shell artifact...' -ForegroundColor Cyan
    & (Join-Path $PSScriptRoot 'Install-Prototype.ps1') -GameShell -NoLaunch
    if ($LASTEXITCODE -ne 0) { throw "The shell installer failed with exit code $LASTEXITCODE." }
}

if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "Missing game shell executable: $exe. Use -Install if you approve fetching the verified build."
}
if (-not (Test-Path -LiteralPath (Join-Path $folder 'game_shell.rml') -PathType Leaf)) {
    throw 'Missing game_shell.rml beside the executable.'
}

$game = Start-Process -FilePath $exe -WorkingDirectory $folder -PassThru
Start-Sleep -Seconds $StartupSeconds
$game.Refresh()
if ($game.HasExited) { throw "Knomira exited on startup (code $($game.ExitCode)). Launch it directly for Qt diagnostics." }
Write-Host "PASS: Game shell process is running (PID $($game.Id))." -ForegroundColor Green

if ($Screenshot) {
    # Captures only the game window rectangle and saves locally; never uploads.
    # Obscuring windows may appear in the screenshot. Review before sharing.
    Add-Type -AssemblyName System.Drawing
    if (-not ('KnomiraWindowApi' -as [type])) {
        Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class KnomiraWindowApi {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left, Top, Right, Bottom; }
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
}
'@
    }

    $handle = [IntPtr]::Zero
    for ($i = 0; $i -lt 12; $i++) {
        $game.Refresh()
        if ($game.HasExited) { throw 'Game closed before screenshot.' }
        $handle = $game.MainWindowHandle
        if ($handle -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 500
    }
    if ($handle -eq [IntPtr]::Zero) { throw 'No main window was available for screenshot.' }
    [void][KnomiraWindowApi]::SetForegroundWindow($handle)
    Start-Sleep -Milliseconds 300
    $rect = New-Object KnomiraWindowApi+RECT
    if (-not [KnomiraWindowApi]::GetWindowRect($handle, [ref]$rect)) {
        throw 'Unable to get the game window rectangle.'
    }
    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    if ($width -lt 100 -or $height -lt 100 -or $width -gt 12000 -or $height -gt 12000) {
        throw ('Invalid game window dimensions: ' + $width + 'x' + $height)
    }
    $dir = Join-Path $root 'build\rmlui-prototype\desktop-tests'
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
    $filename = Join-Path $dir ('knomira-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '.png')
    $bmp = New-Object System.Drawing.Bitmap -ArgumentList $width, $height
    $graphics = [System.Drawing.Graphics]::FromImage($bmp)
    try {
        $graphics.CopyFromScreen($rect.Left, $rect.Top, 0, 0, $bmp.Size)
        $bmp.Save($filename, [System.Drawing.Imaging.ImageFormat]::Png)
    } finally {
        $graphics.Dispose()
        $bmp.Dispose()
    }
    Write-Host "Screenshot saved locally: $filename" -ForegroundColor Green
}

Write-Host 'Next: use permitted Computer Use to click through Controls and verify mouse rebinding. A running process alone does not prove UI correctness.'
