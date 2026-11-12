# Install the latest successful Windows RmlUi prototype into this Knomira checkout.
# Usage: .\prototype\rmlui\Install-Prototype.ps1 [-QtHost] [-NoLaunch]
[CmdletBinding()]
param(
    [switch]$QtHost,
    [switch]$NoLaunch
)

$ErrorActionPreference = 'Stop'
$repository = 'durpscream44-cmyk/Knomira'
$branch = if ($QtHost) { 'feature/rmlui-qt-host' } else { 'feature/rmlui-menu-prototype' }
$workflow = if ($QtHost) { 'rmlui-qt-host.yml' } else { 'rmlui-prototype.yml' }
$artifactName = if ($QtHost) { 'knomira-rmlui-qt-host-windows-debug' } else { 'knomira-rmlui-prototype-windows-debug' }
$exeName = if ($QtHost) { 'knomira_rmlui_qt_host.exe' } else { 'knomira_rmlui_prototype.exe' }
$folderName = if ($QtHost) { 'desktop-test-qt' } else { 'desktop-test' }
$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$parent = Join-Path $root 'build\rmlui-prototype'
$destination = Join-Path $parent $folderName

# Handle both a correctly configured PATH and the standard per-machine install.
$ghCommand = Get-Command gh -ErrorAction SilentlyContinue
if ($ghCommand) {
    $ghExe = $ghCommand.Source
} elseif (Test-Path -LiteralPath 'C:\Program Files\GitHub CLI\gh.exe') {
    $ghExe = 'C:\Program Files\GitHub CLI\gh.exe'
} else {
    throw 'GitHub CLI (gh) is required. Install it with: winget install --id GitHub.cli -e'
}

& $ghExe auth status *> $null
if ($LASTEXITCODE -ne 0) {
    throw 'GitHub CLI is not authenticated. Run: gh auth login'
}

# --jq returns exactly one run ID. Do not collect a full JSON array in
# @(...), which Windows PowerShell can treat as a nested array.
$runId = & $ghExe run list --repo $repository --workflow $workflow --branch $branch --status success --limit 1 --json databaseId --jq '.[0].databaseId'
if ($LASTEXITCODE -ne 0) {
    throw 'Failed to query GitHub Actions.'
}
$runId = ([string]$runId).Trim()
if ($runId -notmatch '^[0-9]+$') {
    throw 'No successful Windows prototype build is available yet. Check GitHub Actions.'
}

if ($QtHost) {
    # Prevent an older successful artifact from silently replacing the latest
    # Xbox-controller test. This is the commit that fixed its Windows SDK build.
    $minimumXinputCommit = '37a1b4e20ca03e6b150291ed67f1a6b78385816f'
    $builtSha = & $ghExe run view $runId --repo $repository --json headSha --jq '.headSha'
    if ($LASTEXITCODE -ne 0) {
        throw 'Could not verify the Qt-host build commit.'
    }
    $builtSha = ([string]$builtSha).Trim()
    & git -C $root merge-base --is-ancestor $minimumXinputCommit $builtSha
    if ($LASTEXITCODE -ne 0) {
        throw "The latest successful Qt build ($builtSha) predates the Xbox input fixes. Check the Qt-host workflow for the next successful build."
    }
}

Write-Host "Using GitHub Actions run #$runId" -ForegroundColor Cyan
Write-Host "Target folder: $destination"

# Download into a fresh staging folder, validate, then swap atomically.
# A failure before validation cannot destroy the previously working build.
New-Item -ItemType Directory -Path $parent -Force | Out-Null
$staging = Join-Path $parent ($folderName + '.staging-' + [guid]::NewGuid().ToString('N'))
$backup = $null
New-Item -ItemType Directory -Path $staging | Out-Null

try {
    & $ghExe run download $runId --repo $repository --name $artifactName --dir $staging
    if ($LASTEXITCODE -ne 0) {
        throw "Could not download the Windows prototype artifact from run #$runId."
    }
    if (-not (Test-Path -LiteralPath (Join-Path $staging $exeName))) {
        throw "The artifact is missing $exeName."
    }
    if (-not (Test-Path -LiteralPath (Join-Path $staging 'main_menu.rml'))) {
        throw 'The artifact is missing main_menu.rml.'
    }

    if (Test-Path -LiteralPath $destination) {
        $suffix = (Get-Date -Format 'yyyyMMdd-HHmmss') + '-' + [guid]::NewGuid().ToString('N').Substring(0, 8)
        $backup = Join-Path $parent ($folderName + '.previous-' + $suffix)
        Move-Item -LiteralPath $destination -Destination $backup
        Write-Host "Previous test build preserved at: $backup" -ForegroundColor Yellow
    }

    Move-Item -LiteralPath $staging -Destination $destination
} catch {
    if ($backup -and -not (Test-Path -LiteralPath $destination) -and (Test-Path -LiteralPath $backup)) {
        Move-Item -LiteralPath $backup -Destination $destination
    }
    throw
} finally {
    if (Test-Path -LiteralPath $staging) {
        Remove-Item -LiteralPath $staging -Recurse -Force
    }
}

$exe = Join-Path $destination $exeName
Write-Host "Prototype installed at: $destination" -ForegroundColor Green
if (-not $NoLaunch) {
    Write-Host 'Opening the prototype menu...' -ForegroundColor Cyan
    Start-Process -FilePath $exe -WorkingDirectory $destination
}
