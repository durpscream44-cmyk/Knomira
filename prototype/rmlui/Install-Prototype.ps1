# Installs the latest successful RmlUi prototype build into this Knomira checkout.
# Run from PowerShell:  .\prototype\rmlui\Install-Prototype.ps1
# Requires GitHub CLI (gh) logged in with access to this repository.
[CmdletBinding()]
param(
    [switch]$NoLaunch
)

$ErrorActionPreference = 'Stop'
$repository = 'durpscream44-cmyk/Knomira'
$branch = 'feature/rmlui-menu-prototype'
$workflow = 'rmlui-prototype.yml'
$artifactName = 'knomira-rmlui-prototype-windows-debug'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$destination = Join-Path $root 'build\rmlui-prototype\desktop-test'

if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
    throw 'GitHub CLI (gh) is required. Install it with: winget install --id GitHub.cli -e'
}

& gh auth status *> $null
if ($LASTEXITCODE -ne 0) {
    throw 'GitHub CLI is not authenticated. Run: gh auth login'
}

$runJson = & gh run list --repo $repository --workflow $workflow --branch $branch --status success --limit 20 --json databaseId,headSha,createdAt
if ($LASTEXITCODE -ne 0) {
    throw 'Could not retrieve successful builds from GitHub Actions.'
}
$runs = @($runJson | ConvertFrom-Json)
if ($runs.Count -eq 0) {
    throw 'No successful Windows RmlUi prototype build was found. Check the Actions workflow.'
}
$run = $runs[0]
Write-Host "Using GitHub Actions run #$($run.databaseId), commit $($run.headSha)" -ForegroundColor Cyan
Write-Host "Target folder: $destination"

if (Test-Path $destination) {
    # Leave any previous desktop-test build untouched on failure.
    $oldDestination = "$destination.previous"
    if (Test-Path $oldDestination) {
        throw "Previous backup already exists: $oldDestination. Remove or rename it manually."
    }
    Rename-Item -Path $destination -NewName (Split-Path $oldDestination -Leaf)
}
New-Item -ItemType Directory -Force -Path $destination | Out-Null

try {
    & gh run download $run.databaseId --repo $repository --name $artifactName --dir $destination
    if ($LASTEXITCODE -ne 0) {
        throw 'Could not download the Windows prototype artifact.'
    }
    $exe = Join-Path $destination 'knomira_rmlui_prototype.exe'
    $document = Join-Path $destination 'main_menu.rml'
    if (-not (Test-Path $exe)) {
        throw "Missing prototype executable in $destination."
    }
    if (-not (Test-Path $document)) {
        throw 'The workflow artifact is missing main_menu.rml. Rebuild with the latest packaging changes.'
    }
} catch {
    Write-Warning 'Installation was not completed. An earlier desktop-test package, if any, remains under desktop-test.previous.'
    throw
}

Write-Host "Prototype installed in $destination" -ForegroundColor Green
if (-not $NoLaunch) {
    Write-Host 'Opening the prototype menu...' -ForegroundColor Cyan
    Start-Process -FilePath $exe -WorkingDirectory $destination
}
