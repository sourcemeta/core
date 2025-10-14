param(
    [Parameter(Mandatory=$true)]
    [string]$ProcessSpawnMain
)

$ErrorActionPreference = "Stop"

# Get the temp directory (equivalent to /tmp on Unix)
$TempDir = $env:TEMP

# Run the process_spawn_main executable with cmd.exe to print the current directory
$Output = & $ProcessSpawnMain $TempDir "cmd.exe" "/c" "cd" 2>&1 | Out-String
$ExitCode = $LASTEXITCODE

if ($ExitCode -ne 0) {
    Write-Error "FAIL: Expected exit code 0, got $ExitCode"
    exit 1
}

# Get the expected path (resolve to full path)
$Expected = (Resolve-Path $TempDir).Path

# Normalize the output (trim whitespace)
$Output = $Output.Trim()

if ($Output -ne $Expected) {
    Write-Error "FAIL: Output mismatch"
    Write-Error "Expected: $Expected"
    Write-Error "Got: $Output"
    exit 1
}

# Success
exit 0
