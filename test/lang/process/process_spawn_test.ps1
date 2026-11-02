param(
  [Parameter(Mandatory=$true)]
  [string]$ProcessSpawnMain
)

$ErrorActionPreference = "Stop"
$TempDir = $env:TEMP
$Output = & $ProcessSpawnMain $TempDir "cmd.exe" "/c" "cd" 2>&1 | Out-String
$ExitCode = $LASTEXITCODE

if ($ExitCode -ne 0) {
  Write-Error "FAIL: Expected exit code 0, got $ExitCode"
  exit 1
}

$Expected = (Resolve-Path $TempDir).Path
$Output = $Output.Trim()

if ($Output -ne $Expected) {
  Write-Error "FAIL: Output mismatch"
  Write-Error "Expected: $Expected"
  Write-Error "Got: $Output"
  exit 1
}

exit 0
