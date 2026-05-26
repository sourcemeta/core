param(
  [Parameter(Mandatory=$true)]
  [string]$StacktraceOnDemandMain,
  [Parameter(Mandatory=$true)]
  [string]$WorkDir
)

$ErrorActionPreference = "Stop"
$Self = [IO.Path]::GetFileName($StacktraceOnDemandMain)
$Actual = Join-Path $WorkDir "$Self.actual.txt"

$ErrorActionPreference = "Continue"
& $StacktraceOnDemandMain > $Actual 2>&1
$ExitCode = $LASTEXITCODE
$ErrorActionPreference = "Stop"
# Exited cleanly after printing the on-demand trace
if ($ExitCode -ne 0) {
  throw "Expected exit code 0, got $ExitCode"
}

Get-Content $Actual

$Output = Get-Content $Actual -Raw

# Without /Zi + /DEBUG, dbghelp cannot resolve internal application symbols.
# Verify the on-demand call still produced a structurally-correct trace where
# the application frames show as <unknown>.
if ($Output -notmatch '={80}') {
  throw "Missing separator line"
}
if ($Output -notmatch '<unknown>') {
  throw "Expected at least one <unknown> frame (dbghelp without PDB)"
}
