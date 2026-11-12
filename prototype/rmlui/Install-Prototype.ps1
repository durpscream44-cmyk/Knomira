# Install the latest successful RmlUi prototype build into this Knomira checkout.
# Usage: .\prototype\rmlui\Install-Prototype.ps1 [-NoLaunch]
# Requires GitHub CLI (gh), authenticated to GitHub.
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
$parent = Join-Path $root 'build\rmlui-prototype'
$destination = Join-Path $parent 'desktop-test'

if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
    throw 'GitHub CLI (gh) is required. Install it with: winget install --id GitHub.cli -e'
}

& gh auth status *> $null
if ($LASTEXITCODE -ne 0) {
    throw 'GitHub CLI is not authenticated. Run: gh auth login'
}

# Do not pipe a JSON array into @(...): Windows PowerShell may treat it as one
# nested array, causing $runs[0].databaseId to contain every run ID.
# Select exactly one ID in gh itself so the value is always scalar.
$runId = & gh run list --repo $repository --workflow $workflow --branch $branch --status success --limit 1 --json databaseId --jq '.[0].databaseId'
if ($LASTEXITCODE -ne 0) {
    throw 'Could not retrieve successful builds from GitHub Actions.'
}
$runId = "$runId".Trim()
if ($runId -notmatch '^[0-9]+$') {
    throw 'No successful Windows RmlUi prototype build was found. Check the Actions workflow.'
}

Write-Host "Using GitHub Actions run #$runId" -ForegroundColor Cyan
Write-Host "Target folder: $destination"

# Never replace a previous working build until the new download has been
# verified. Unique staging folders also avoid conflicts with previous failures.
New-Item -ItemType Directory -Force -Path $parent | Out-Null
$staging = Join-Path $parent ("desktop-test.staging-" + [guid]::NewGuid().ToString('N'))
$backup = $null
New-Item -ItemType Directory -Path $staging | Out-Null

try {
    & gh run download $runId --repo $repository --name $artifactName --dir $staging
    if ($LASTEXITCODE -ne 0) {
        throw "Could not download the Windows prototype artifact from run #$runId."
    }
    if (-not (Test-Path -LiteralPath (Join-Path $staging 'knomira_rmlui_prototype.exe'))) {
        throw 'The workflow artifact is missing the prototype executable.'
    }
    if (-not (Test-Path -LiteralPath (Join-Path $staging 'main_menu.rml'))) {
        throw 'The workflow artifact is missing main_menu.rml. Rebuild with the latest packaging changes.'
    }

    if (Test-Path -LiteralPath $destination) {
        $backup = Join-Path $parent ("desktop-test.previous-" + (Get-Date -Format 'yyyyMMdd-HHmmss') + "-" + [guid]::NewGuid().ToString('N').Substring(0, 8))
        Move-Item -LiteralPath $destination -Destination $backup
        Write-Host "Previous test folder saved to: $backup" -ForegroundColor Yellow
    }

    Move-Item -LiteralPath $staging -Destination $destination
} catch {
    # Restore the previous build if the replacement itself failed.
    if ($backup -and -not (Test-Path -LiteralPath $destination) -and (Test-Path -LiteralPath $backup)) {
        Move-Item -LiteralPath $backup -Destination $destination
    }
    throw
} finally {
    if (Test-Path -LiteralPath $staging) {
        Remove-Item -LiteralPath $staging -Recurse -Force
    }
}

$exe = Join-Path $destination 'knomira_rmlui_prototype.exe'
Write-Host "Prototype installed in $destination" -ForegroundColor Green
if (-not $NoLaunch) {
    Write-Host 'Opening the prototype menu...' -ForegroundColor Cyan
    Start-Process -FilePath $exe -WorkingDirectory $destination
}
