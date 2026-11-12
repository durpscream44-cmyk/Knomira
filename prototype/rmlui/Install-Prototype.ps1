# Install the latest successful RmlUi prototype build into this Knomira checkout.
# Usage: .\prototype\rmlui\Install-Prototype.ps1 [-NoLaunch] [-QtHost]
# Requires GitHub CLI (gh), authenticated to GitHub.
[CmdletBinding()]
param(
    [switch]$NoLaunch,
    [switch]$QtHost
)

$ErrorActionPreference = 'Stop'
$repository = 'durpscream44-cmyk/Knomira'
$branch = if ($QtHost) { 'feature/rmlui-qt-host' } else { 'feature/rmlui-menu-prototype' }
$workflow = if ($QtHost) { 'rmlui-qt-host.yml' } else { 'rmlui-prototype.yml' }
$artifactName = if ($QtHost) { 'knomira-rmlui-qt-host-windows-debug' } else { 'knomira-rmlui-prototype-windows-debug' }
$exeName = if ($QtHost) { 'knomira_rmlui_qt_host.exe' } else { 'knomira_rmlui_prototype.exe' }
$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$parent = Join-Path $root 'build\rmlui-prototype'
$destination = Join-Path $parent $(if ($QtHost) { 'desktop-test-qt' } else { 'desktop-test' })

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
if ($runId -notmatch '^[0-9]+
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
    if (-not (Test-Path -LiteralPath (Join-Path $staging $exeName))) {
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

$exe = Join-Path $destination $exeName
Write-Host "Prototype installed in $destination" -ForegroundColor Green
if (-not $NoLaunch) {
    Write-Host 'Opening the prototype menu...' -ForegroundColor Cyan
    Start-Process -FilePath $exe -WorkingDirectory $destination
}
) {
    throw 'No successful Windows RmlUi prototype build was found. Check the Actions workflow.'
}

# The Qt host is actively evolving. Never silently install an older artifact
# while testing new controller/navigation code on the checked-out branch.
if ($QtHost) {
    $expectedSha = (& git rev-parse HEAD).Trim()
    if ($LASTEXITCODE -ne 0 -or $expectedSha -notmatch '^[0-9a-f]{40}
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
    if (-not (Test-Path -LiteralPath (Join-Path $staging $exeName))) {
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

$exe = Join-Path $destination $exeName
Write-Host "Prototype installed in $destination" -ForegroundColor Green
if (-not $NoLaunch) {
    Write-Host 'Opening the prototype menu...' -ForegroundColor Cyan
    Start-Process -FilePath $exe -WorkingDirectory $destination
}
) {
        throw 'Could not resolve the current Knomira Git commit. Run this from a git checkout.'
    }
    $builtSha = (& gh run view $runId --repo $repository --json headSha --jq '.headSha').Trim()
    if ($LASTEXITCODE -ne 0 -or $builtSha -ne $expectedSha) {
        throw "Latest successful Qt-host build is for commit $builtSha, but this checkout is $expectedSha. Check the latest Qt-host workflow result before testing new changes."
    }
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
    if (-not (Test-Path -LiteralPath (Join-Path $staging $exeName))) {
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

$exe = Join-Path $destination $exeName
Write-Host "Prototype installed in $destination" -ForegroundColor Green
if (-not $NoLaunch) {
    Write-Host 'Opening the prototype menu...' -ForegroundColor Cyan
    Start-Process -FilePath $exe -WorkingDirectory $destination
}
