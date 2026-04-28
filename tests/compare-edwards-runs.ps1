<#
.SYNOPSIS
    Compare two hazard-capture zips and report per-driver deltas.

.DESCRIPTION
    Extracts two capture zips produced by Run-HazardCapture.ps1 (e.g. a
    v4.4.3 baseline vs a post-patch v4.4.4 run on the Windows SAS host), reads both
    manifest.csv files, and prints a per-driver delta table:

        FLIP        baseline exit != 0, new exit == 0     (improvement)
        REGRESS     baseline exit == 0, new exit != 0     (worsens)
        STABLE      same exit code, same first-error class
        ERROR-DRIFT same exit code but different first-error message
        NEW         driver only present in new run
        DROPPED     driver only present in baseline run

    Optionally byte-compares the .lst content after masking SAS page-header
    timestamps; useful for verifying the cosmetic-only-fix claim.

    Designed for the v4.4.x SAS-macro-fix verification workflow but useful
    for any pair of Windows host captures.

.PARAMETER BaselineZip
    Path to the baseline (older) capture zip.

.PARAMETER NewZip
    Path to the new (post-change) capture zip.

.PARAMETER OutputDir
    Directory to write the delta-report.txt into. Default: current dir.

.PARAMETER CompareLstContent
    Also SHA256-compare each driver's .lst after masking SAS page-header
    timestamps. Reports DRIFT-LST when the content differs after masking,
    or LST-MATCH when byte-identical.

.EXAMPLE
    PS> .\compare-edwards-runs.ps1 `
            hazard-capture-20260427-134953.zip `
            hazard-capture-20260428-073323.zip

.EXAMPLE
    # With LST content comparison:
    PS> .\compare-edwards-runs.ps1 baseline.zip new.zip -CompareLstContent

.EXAMPLE
    # From cmd.exe:
    C:\> powershell -ExecutionPolicy Bypass -File compare-edwards-runs.ps1 baseline.zip new.zip
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory=$true, Position=0)]
    [string]$BaselineZip,

    [Parameter(Mandatory=$true, Position=1)]
    [string]$NewZip,

    [string]$OutputDir = ".",

    [switch]$CompareLstContent
)

$ErrorActionPreference = "Stop"

function Resolve-AbsPath([string]$p) { (Resolve-Path -LiteralPath $p).Path }

if (-not (Test-Path -LiteralPath $BaselineZip)) {
    Write-Error "Baseline zip not found: $BaselineZip"; exit 2
}
if (-not (Test-Path -LiteralPath $NewZip)) {
    Write-Error "New zip not found: $NewZip"; exit 2
}
if (-not (Test-Path -LiteralPath $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

$BaselineZip = Resolve-AbsPath $BaselineZip
$NewZip      = Resolve-AbsPath $NewZip
$OutputDir   = Resolve-AbsPath $OutputDir
$stamp       = Get-Date -Format "yyyyMMdd-HHmmss"

# ---------------------------------------------------------------------------
# Extract both zips into temp dirs
# ---------------------------------------------------------------------------
$tmpBase = Join-Path $env:TEMP "compare-edwards-$stamp"
$baseDir = Join-Path $tmpBase "baseline"
$newDir  = Join-Path $tmpBase "new"
New-Item -ItemType Directory -Path $baseDir | Out-Null
New-Item -ItemType Directory -Path $newDir  | Out-Null

Write-Host "Extracting:"
Write-Host "  baseline: $BaselineZip"
Write-Host "  new:      $NewZip"
Expand-Archive -LiteralPath $BaselineZip -DestinationPath $baseDir -Force
Expand-Archive -LiteralPath $NewZip      -DestinationPath $newDir  -Force

function Find-Manifest([string]$root) {
    $direct = Join-Path $root "manifest.csv"
    if (Test-Path -LiteralPath $direct) { return $direct }
    # Fall back to recursive search (some zips wrap the contents in a parent
    # dir named after the capture stem).
    $found = Get-ChildItem -LiteralPath $root -Recurse -File -Filter manifest.csv `
             -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($found) { return $found.FullName }
    Write-Error "manifest.csv not found in $root"; exit 2
}

$baseManifest = Find-Manifest $baseDir
$newManifest  = Find-Manifest $newDir

$baseRows = Import-Csv -LiteralPath $baseManifest
$newRows  = Import-Csv -LiteralPath $newManifest

# Index by driver name
$baseByName = @{}
foreach ($r in $baseRows) { $baseByName[$r.Driver] = $r }
$newByName = @{}
foreach ($r in $newRows) { $newByName[$r.Driver] = $r }

$allDrivers = ($baseByName.Keys + $newByName.Keys) | Sort-Object -Unique

# ---------------------------------------------------------------------------
# Optional .lst content comparison (timestamp-masked SHA256)
# ---------------------------------------------------------------------------
function Get-MaskedLstHash([string]$lstRoot, [string]$driver) {
    # SAS page-header pattern: "13:49 Monday, April 27, 2026"
    $lstPath = Join-Path $lstRoot "lst" "$driver.lst"
    if (-not (Test-Path -LiteralPath $lstPath)) { return $null }
    $regex = '\b\d{1,2}:\d{2} (Mon|Tues|Wednes|Thurs|Fri|Satur|Sun)day, \w+ \d{1,2}, \d{4}'
    $masked = (Get-Content -LiteralPath $lstPath -Raw) -replace $regex, '<TS>'
    $stream = [System.IO.MemoryStream]::new([System.Text.Encoding]::UTF8.GetBytes($masked))
    $hash = (Get-FileHash -InputStream $stream -Algorithm SHA256).Hash
    $stream.Dispose()
    $hash
}

# ---------------------------------------------------------------------------
# Build the delta table
# ---------------------------------------------------------------------------
$report = @()
function Emit([string]$line) { Write-Host $line; $script:report += $line }

Emit "Windows host capture comparison"
Emit "=========================="
Emit "Baseline: $BaselineZip"
Emit "New:      $NewZip"
Emit "Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss zzz')"
Emit ""

$counts = @{
    FLIP        = 0
    REGRESS     = 0
    STABLE      = 0
    'ERROR-DRIFT' = 0
    'LST-DRIFT'   = 0
    NEW         = 0
    DROPPED     = 0
}

$rows = @()
foreach ($drv in $allDrivers) {
    $b = $baseByName[$drv]
    $n = $newByName[$drv]

    if (-not $b) { $verdict = "NEW";     $bx="-"; $nx=$n.ExitCode }
    elseif (-not $n) { $verdict = "DROPPED"; $bx=$b.ExitCode; $nx="-" }
    else {
        $bx = $b.ExitCode; $nx = $n.ExitCode
        if ($bx -ne 0 -and $nx -eq 0)      { $verdict = "FLIP" }
        elseif ($bx -eq 0 -and $nx -ne 0)  { $verdict = "REGRESS" }
        elseif ($bx -eq $nx) {
            if ($b.FirstErrors -ne $n.FirstErrors) { $verdict = "ERROR-DRIFT" }
            else { $verdict = "STABLE" }
        } else {
            # Both non-zero but different exit codes.
            $verdict = "ERROR-DRIFT"
        }
    }

    $lstHashCmp = ""
    if ($CompareLstContent -and $b -and $n -and $bx -eq 0 -and $nx -eq 0) {
        $bh = Get-MaskedLstHash $baseDir $drv
        $nh = Get-MaskedLstHash $newDir  $drv
        if ($bh -and $nh) {
            if ($bh -eq $nh) { $lstHashCmp = "LST-MATCH" }
            else { $lstHashCmp = "LST-DRIFT"; $counts['LST-DRIFT']++ }
        }
    }

    $counts[$verdict]++

    $rows += [pscustomobject]@{
        Driver  = $drv
        Base    = $bx
        New     = $nx
        Verdict = $verdict
        Lst     = $lstHashCmp
    }
}

# Print summary first
Emit "Summary"
Emit "-------"
foreach ($k in @('FLIP','REGRESS','STABLE','ERROR-DRIFT','LST-DRIFT','NEW','DROPPED')) {
    if ($counts[$k] -gt 0) {
        Emit ("  {0,-12} {1,3}" -f $k, $counts[$k])
    }
}
Emit ""

# Then the per-driver table
$header = "{0,-30} {1,5} {2,5}  {3,-12} {4}" -f "Driver","Base","New","Verdict","LST"
Emit $header
Emit ("-" * $header.Length)

foreach ($r in ($rows | Sort-Object @{e='Verdict'; desc=$false}, Driver)) {
    Emit ("{0,-30} {1,5} {2,5}  {3,-12} {4}" -f $r.Driver, $r.Base, $r.New, $r.Verdict, $r.Lst)
}

# ---------------------------------------------------------------------------
# Show first-error deltas for REGRESS / ERROR-DRIFT (the actionable cases)
# ---------------------------------------------------------------------------
$actionable = $rows | Where-Object { $_.Verdict -in 'REGRESS','ERROR-DRIFT' }
if ($actionable) {
    Emit ""
    Emit "Actionable details"
    Emit "------------------"
    foreach ($r in $actionable) {
        $b = $baseByName[$r.Driver]
        $n = $newByName[$r.Driver]
        Emit ""
        Emit "[$($r.Driver)]  $($r.Verdict)  base=$($r.Base) -> new=$($r.New)"
        if ($b.FirstErrors) { Emit "  baseline: $($b.FirstErrors)" }
        if ($n.FirstErrors) { Emit "  new:      $($n.FirstErrors)" }
    }
}

# ---------------------------------------------------------------------------
# Write report file
# ---------------------------------------------------------------------------
$reportPath = Join-Path $OutputDir "edwards-compare-$stamp.txt"
Set-Content -LiteralPath $reportPath -Value $report -Encoding UTF8

Write-Host ""
Write-Host "Report written to: $reportPath"

# Cleanup temp extracts
Remove-Item -Recurse -Force -LiteralPath $tmpBase

# Exit code reflects whether any regressions occurred
if ($counts['REGRESS'] -gt 0 -or $counts['LST-DRIFT'] -gt 0) {
    exit 1
}
exit 0
