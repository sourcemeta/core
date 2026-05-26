param(
  [Parameter(Mandatory=$true)]
  [string]$StacktraceSegfaultMain,
  [Parameter(Mandatory=$true)]
  [string]$WorkDir
)

$ErrorActionPreference = "Stop"
$Self = [IO.Path]::GetFileName($StacktraceSegfaultMain)
$Actual = Join-Path $WorkDir "$Self.actual.txt"

& $StacktraceSegfaultMain *> $Actual
$ExitCode = $LASTEXITCODE
# Crashed by a fatal exception
if ($ExitCode -eq 0) {
  throw "Expected non-zero exit code, got $ExitCode"
}

Get-Content $Actual

$Output = Get-Content $Actual -Raw

# Without /Zi + /DEBUG, dbghelp cannot resolve internal application symbols.
# Verify the handler still ran and produced a structurally-correct trace where
# the application frames show as <unknown>.
if ($Output -notmatch '={80}') {
  throw "Missing separator line"
}
if ($Output -notmatch '(?m)^signal:\s+0x[0-9a-fA-F]+ \(SEH\)') {
  throw "Missing signal line"
}
if ($Output -notmatch '<unknown>') {
  throw "Expected at least one <unknown> frame (dbghelp without PDB)"
}
