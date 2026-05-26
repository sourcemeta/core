param(
  [Parameter(Mandatory=$true)]
  [string]$StacktraceSegfaultMain,
  [Parameter(Mandatory=$true)]
  [string]$WorkDir,
  [Parameter(Mandatory=$true)]
  [string]$StacktraceLibrary
)

$ErrorActionPreference = "Stop"
$Self = [IO.Path]::GetFileName($StacktraceSegfaultMain)
$LibraryPath = if ($StacktraceLibrary -match '\.(lib|a)$') {
  $StacktraceSegfaultMain
} else {
  $StacktraceLibrary
}
$Actual = Join-Path $WorkDir "$Self.actual.txt"
$Normalized = Join-Path $WorkDir "$Self.normalized.txt"
$Expected = Join-Path $WorkDir "$Self.expected.txt"

& $StacktraceSegfaultMain *> $Actual
$ExitCode = $LASTEXITCODE
# Crashed by a fatal exception
if ($ExitCode -eq 0) {
  throw "Expected non-zero exit code, got $ExitCode"
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
signal:  0xc0000005 (SEH)
pid:     <PID>

# 0xADDR sourcemeta_core_stacktrace_test::crash_deepest +0xOFFSET
  in $StacktraceSegfaultMain
# 0xADDR crash_handler +0xOFFSET
  in $LibraryPath
# 0xADDR sourcemeta_core_stacktrace_test::crash_middle +0xOFFSET
  in $StacktraceSegfaultMain
# 0xADDR main +0xOFFSET
  in $StacktraceSegfaultMain
================================================================================
"@ | Set-Content -NoNewline $Expected

if ((Get-Content $Normalized -Raw) -ne (Get-Content $Expected -Raw)) {
  Compare-Object (Get-Content $Expected) (Get-Content $Normalized)
  throw "Output did not match expected"
}
