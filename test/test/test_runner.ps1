param(
  [Parameter(Mandatory=$true)]
  [string]$Driver,
  [Parameter(Mandatory=$true)]
  [int]$ExpectedExit,
  [Parameter(Mandatory=$true)]
  [string]$ExpectedOutput,
  [Parameter(ValueFromRemainingArguments=$true)]
  [string[]]$Rest
)

$Output = & $Driver @Rest 2>&1 | Out-String
$ExitCode = $LASTEXITCODE

if ($ExitCode -ne $ExpectedExit) {
  Write-Error "FAIL: Expected exit code $ExpectedExit, got $ExitCode"
  Write-Error $Output
  exit 1
}

$Expected = Get-Content -Raw $ExpectedOutput
$ActualNormalized = ($Output -replace "`r`n", "`n").TrimEnd("`n")
$ExpectedNormalized = ($Expected -replace "`r`n", "`n").TrimEnd("`n")

if ($ActualNormalized -ne $ExpectedNormalized) {
  Write-Error "FAIL: Output did not match $ExpectedOutput"
  Write-Error "Expected:`n$ExpectedNormalized"
  Write-Error "Got:`n$ActualNormalized"
  exit 1
}

exit 0
