<#
.SYNOPSIS
    Capture HAZARD Windows test outputs into a zip, optionally laid out to
    match the UNIX corpus reference layout consumed by tests/validate_corpus.sh.

.DESCRIPTION
    Two modes:

    (1) Default (flat zip) — bundles *.log and *.lst (and optionally *.haz)
        from a source directory into a single dated zip with a MANIFEST.txt
        listing SHA256 per file. Suitable for ad-hoc archiving / sharing.

    (2) -CorpusLayout — populates
          <RepoRoot>\tests\corpus\hazard\reference\v<Version>-<Platform>\
        with one <name>.lst, <name>.haz (if present), and <name>.meta per case,
        matching the schema produced by scripts/capture-legacy.sh on UNIX. This
        is what tests/validate_corpus.sh reads when you run:
          REFERENCE=v<Version>-<Platform> ./tests/validate_corpus.sh
        A zip of the populated reference dir is also written, unless -NoZip.

    The .meta file synthesized on Windows is an honest approximation:
    per-invocation real_exit is not recoverable from a post-hoc directory
    scan (UNIX captures this via a PATH-shadow wrapper). real_exit is written
    as 0 with a comment in the file header.

.PARAMETER SourceDir
    Directory containing the .log / .lst / .haz files produced by the Windows
    run (typically tests/ after tests.bat). Default: current directory.

.PARAMETER OutputDir
    Directory to write the zip into. Default: current directory.

.PARAMETER Version
    Version string. Default: 4.4.3.

.PARAMETER Platform
    Platform tag used in the corpus directory name. Default: windows-x86_64.

.PARAMETER RepoRoot
    Path to the HAZARD git clone. Required when -CorpusLayout is used.
    Default: two levels above this script (..\..\ from tests\).

.PARAMETER HazardBin
    Path to the hazard.exe used for the run, recorded into .meta as real_bin.
    Default: <RepoRoot>\src\hazard\hazard.exe.

.PARAMETER CorpusLayout
    Populate the corpus reference directory (mode 2). Implies capture of *.haz.

.PARAMETER IncludeHaz
    In default (flat zip) mode, also include *.haz files. Ignored in -CorpusLayout
    mode (which always captures .haz).

.PARAMETER NoZip
    Skip creating the zip file (only meaningful with -CorpusLayout).

.EXAMPLE
    # Flat zip, current dir, v4.4.3:
    PS> .\capture-windows-outputs.ps1

.EXAMPLE
    # Corpus layout for validate_corpus.sh:
    PS> .\capture-windows-outputs.ps1 -CorpusLayout -RepoRoot C:\hazard -SourceDir C:\hazard\tests

.EXAMPLE
    # From cmd.exe:
    C:\> powershell -ExecutionPolicy Bypass -File tests\capture-windows-outputs.ps1 -CorpusLayout
#>

[CmdletBinding()]
param(
    [string]$SourceDir  = ".",
    [string]$OutputDir  = ".",
    [string]$Version    = "4.4.3",
    [string]$Platform   = "windows-x86_64",
    [string]$RepoRoot,
    [string]$HazardBin,
    [switch]$CorpusLayout,
    [switch]$IncludeHaz,
    [switch]$NoZip
)

$ErrorActionPreference = "Stop"

# --- Resolve paths -----------------------------------------------------------
$SourceDir = (Resolve-Path -LiteralPath $SourceDir).Path
if (-not (Test-Path -LiteralPath $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}
$OutputDir = (Resolve-Path -LiteralPath $OutputDir).Path

if ($CorpusLayout) {
    if (-not $RepoRoot) {
        $RepoRoot = Split-Path -Parent (Split-Path -Parent $PSCommandPath)
    }
    $RepoRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
    if (-not $HazardBin) {
        $HazardBin = Join-Path $RepoRoot "src\hazard\hazard.exe"
    }
}

# --- Helpers ----------------------------------------------------------------
function New-Uid {
    $epoch = [int64](New-TimeSpan -Start (Get-Date "1970-01-01Z") -End (Get-Date).ToUniversalTime()).TotalSeconds
    $frac  = Get-Random -Minimum 100000 -Maximum 999999
    "{0}.{1}.{2}" -f $epoch, $frac, $PID
}

function Get-HostLine {
    $arch = if ([System.Environment]::Is64BitOperatingSystem) { 'x86_64' } else { 'x86' }
    "Windows {0} {1} {2}" -f $env:COMPUTERNAME, [System.Environment]::OSVersion.VersionString, $arch
}

function New-MetaFile {
    param(
        [string]$Path,
        [string]$BinKind,
        [string]$RealBin,
        [string]$Argv,
        [string]$Pwd
    )
    $lines = @(
        '# capture-windows-outputs.ps1 metadata',
        '# NOTE: real_exit is synthesized as 0 — Windows capture is post-hoc from',
        '# the working directory, not a PATH-shadow wrapper (cf. scripts/capture-legacy.sh).',
        '# Per-invocation exit codes are not recoverable from a directory scan.',
        "uid=$(New-Uid)",
        "bin_kind=$BinKind",
        "real_bin=$RealBin",
        'real_exit=0',
        "argv=$Argv",
        "timestamp=$((Get-Date).ToUniversalTime().ToString('yyyy-MM-ddTHH:mm:ssZ'))",
        "host=$(Get-HostLine)",
        "tmpdir=$env:TEMP",
        "pwd=$Pwd"
    )
    Set-Content -LiteralPath $Path -Value $lines -Encoding UTF8
}

# --- Collect files -----------------------------------------------------------
$patterns = @("*.log", "*.lst")
if ($IncludeHaz -or $CorpusLayout) { $patterns += "*.haz" }

$files = foreach ($pat in $patterns) {
    Get-ChildItem -LiteralPath $SourceDir -Filter $pat -File -ErrorAction SilentlyContinue
}
$files = $files | Sort-Object FullName -Unique

if (-not $files -or $files.Count -eq 0) {
    Write-Error "No matching files found in $SourceDir (patterns: $($patterns -join ', '))"
    exit 1
}

Write-Host "Found $($files.Count) file(s) in $SourceDir"

# ============================================================================
#  MODE 2 — Corpus layout
# ============================================================================
if ($CorpusLayout) {
    $refDirName = "v$Version-$Platform"
    $refDir = Join-Path $RepoRoot "tests\corpus\hazard\reference\$refDirName"
    if (Test-Path -LiteralPath $refDir) {
        Write-Warning "Reference dir already exists, overwriting: $refDir"
        Remove-Item -Recurse -Force -LiteralPath $refDir
    }
    New-Item -ItemType Directory -Path $refDir | Out-Null

    # In corpus mode we only care about .lst (the primary output) and companion .haz.
    # .log is excluded — it's SAS log noise, not what validate_corpus.sh consumes.
    $lstFiles = $files | Where-Object { $_.Extension -eq '.lst' }
    if (-not $lstFiles -or $lstFiles.Count -eq 0) {
        Write-Error "No .lst files found in $SourceDir — corpus mode needs .lst outputs."
        exit 1
    }

    $casesWritten = 0
    foreach ($lst in $lstFiles) {
        $case = [System.IO.Path]::GetFileNameWithoutExtension($lst.Name)
        Copy-Item -LiteralPath $lst.FullName -Destination (Join-Path $refDir "$case.lst")

        $hazPath = Join-Path $SourceDir "$case.haz"
        if (Test-Path -LiteralPath $hazPath) {
            Copy-Item -LiteralPath $hazPath -Destination (Join-Path $refDir "$case.haz")
        }

        $metaPath = Join-Path $refDir "$case.meta"
        New-MetaFile -Path $metaPath `
                     -BinKind "hazard" `
                     -RealBin $HazardBin `
                     -Argv "" `
                     -Pwd $SourceDir

        $casesWritten++
    }

    # Top-level README so a reader lands on something meaningful
    $readmePath = Join-Path $refDir "README.md"
    $readmeLines = @(
        "# HAZARD v$Version Windows reference ($Platform)",
        '',
        "Captured: $((Get-Date).ToUniversalTime().ToString('yyyy-MM-ddTHH:mm:ssZ'))",
        "Host:     $env:COMPUTERNAME",
        "User:     $env:USERNAME",
        "Binary:   $HazardBin",
        "Cases:    $casesWritten",
        '',
        'Generated by `tests/capture-windows-outputs.ps1 -CorpusLayout`.',
        '',
        'Consumed by `tests/validate_corpus.sh` when invoked with:',
        '',
        '```sh',
        "REFERENCE=v$Version-$Platform ./tests/validate_corpus.sh",
        '```',
        '',
        'See `tests/corpus/README.md` for the full corpus layout and validation harness docs.'
    )
    Set-Content -LiteralPath $readmePath -Value $readmeLines -Encoding UTF8

    Write-Host "Populated: $refDir"
    Write-Host "Cases:     $casesWritten"

    if (-not $NoZip) {
        $stamp = Get-Date -Format "yyyyMMdd-HHmmss"
        $zipPath = Join-Path $OutputDir "hazard-corpus-$refDirName-$stamp.zip"
        if (Test-Path -LiteralPath $zipPath) { Remove-Item -Force -LiteralPath $zipPath }
        Compress-Archive -Path (Join-Path $refDir '*') -DestinationPath $zipPath -CompressionLevel Optimal
        $zipSize = (Get-Item -LiteralPath $zipPath).Length
        Write-Host ""
        Write-Host "Wrote zip: $zipPath"
        Write-Host ("Size:      {0:N0} bytes ({1:N2} MB)" -f $zipSize, ($zipSize / 1MB))
    }

    Write-Host ""
    Write-Host "Next steps:"
    Write-Host "  1. Verify contents:  Get-ChildItem '$refDir'"
    Write-Host "  2. Commit the reference dir into the repo."
    Write-Host "  3. On any host:  REFERENCE=$refDirName bash tests/validate_corpus.sh"
    exit 0
}

# ============================================================================
#  MODE 1 — Flat zip (default)
# ============================================================================
$stamp   = Get-Date -Format "yyyyMMdd-HHmmss"
$stem    = "hazard-windows-outputs-v$Version-$stamp"
$staging = Join-Path $env:TEMP $stem
if (Test-Path -LiteralPath $staging) { Remove-Item -Recurse -Force -LiteralPath $staging }
New-Item -ItemType Directory -Path $staging | Out-Null

foreach ($f in $files) {
    Copy-Item -LiteralPath $f.FullName -Destination $staging
}

$manifestPath = Join-Path $staging "MANIFEST.txt"
$manifest = @()
$manifest += "HAZARD Windows test output capture"
$manifest += "=================================="
$manifest += "Version:    v$Version"
$manifest += "Captured:   $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss zzz')"
$manifest += "Host:       $env:COMPUTERNAME"
$manifest += "User:       $env:USERNAME"
$manifest += "SourceDir:  $SourceDir"
$manifest += "Patterns:   $($patterns -join ', ')"
$manifest += "FileCount:  $($files.Count)"
$manifest += ""
$manifest += "Files (SHA256  size_bytes  name):"
$manifest += "---------------------------------"
foreach ($f in ($files | Sort-Object Name)) {
    $hash = (Get-FileHash -LiteralPath $f.FullName -Algorithm SHA256).Hash
    $manifest += "{0}  {1,12}  {2}" -f $hash, $f.Length, $f.Name
}
Set-Content -LiteralPath $manifestPath -Value $manifest -Encoding UTF8

$zipPath = Join-Path $OutputDir "$stem.zip"
if (Test-Path -LiteralPath $zipPath) { Remove-Item -Force -LiteralPath $zipPath }
Compress-Archive -Path (Join-Path $staging '*') -DestinationPath $zipPath -CompressionLevel Optimal

Remove-Item -Recurse -Force -LiteralPath $staging

$zipSize = (Get-Item -LiteralPath $zipPath).Length
Write-Host ""
Write-Host "Wrote: $zipPath"
Write-Host ("Size:  {0:N0} bytes ({1:N2} MB)" -f $zipSize, ($zipSize / 1MB))
Write-Host "Files: $($files.Count) captured (+ MANIFEST.txt)"
