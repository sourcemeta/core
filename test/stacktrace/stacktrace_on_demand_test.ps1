param(
  [Parameter(Mandatory=$true)]
  [string]$StacktraceOnDemandMain,
  [Parameter(Mandatory=$true)]
  [string]$WorkDir,
  [Parameter(Mandatory=$true)]
  [string]$StacktraceLibrary
)

$ErrorActionPreference = "Stop"
$Self = [IO.Path]::GetFileName($StacktraceOnDemandMain)
$LibraryPath = if ($StacktraceLibrary -match '\.(lib|a)$') {
  $StacktraceOnDemandMain
} else {
  $StacktraceLibrary
}
$Actual = Join-Path $WorkDir "$Self.actual.txt"
$Normalized = Join-Path $WorkDir "$Self.normalized.txt"
$Expected = Join-Path $WorkDir "$Self.expected.txt"

& $StacktraceOnDemandMain *> $Actual
$ExitCode = $LASTEXITCODE
# Exited cleanly after printing the on-demand trace
if ($ExitCode -ne 0) {
  throw "Expected exit code 0, got $ExitCode"
}

Get-Content $Actual

(Get-Content $Actual -Raw) `
  -replace '0x[0-9a-fA-F]+', '0xADDR' `
  -replace '\+0xADDR', '+0xOFFSET' `
  -replace '(?m)^pid:\s+\d+', 'pid:     <PID>' `
  -replace '(?m)^#\d+ ', '# ' `
  -replace '(?m)^# [^\r\n]*(_sigtramp|__restore_rt| _?start \+)[^\r\n]*\r?\n  in [^\r\n]*\r?\n', '' `
  -replace '(?m)^# [^\r\n]*\r?\n  in [^\r\n]*(libsystem_|libdyld|/dyld)[^\r\n]*\r?\n', '' `
  | Set-Content -NoNewline $Normalized

@"
================================================================================
pid:     <PID>

# 0xADDR sourcemeta::core::stacktrace +0xOFFSET
  in $LibraryPath
# 0xADDR sourcemeta_core_stacktrace_test::print_deepest +0xOFFSET
  in $StacktraceOnDemandMain
# 0xADDR sourcemeta_core_stacktrace_test::print_middle +0xOFFSET
  in $StacktraceOnDemandMain
# 0xADDR main +0xOFFSET
  in $StacktraceOnDemandMain
================================================================================
"@ | Set-Content -NoNewline $Expected

if ((Get-Content $Normalized -Raw) -ne (Get-Content $Expected -Raw)) {
  Compare-Object (Get-Content $Expected) (Get-Content $Normalized)
  throw "Output did not match expected"
}
