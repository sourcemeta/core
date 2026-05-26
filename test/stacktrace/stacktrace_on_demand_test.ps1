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
$StacktraceOnDemandMain = $StacktraceOnDemandMain -replace '/', '\'
$StacktraceLibrary = $StacktraceLibrary -replace '/', '\'
$LibraryPath = if ($StacktraceLibrary -match '\.(lib|a)$') {
  $StacktraceOnDemandMain
} else {
  $StacktraceLibrary
}
$Actual = Join-Path $WorkDir "$Self.actual.txt"
$Normalized = Join-Path $WorkDir "$Self.normalized.txt"
$Expected = Join-Path $WorkDir "$Self.expected.txt"

$ErrorActionPreference = "Continue"
& cmd.exe /c "`"$StacktraceOnDemandMain`" > `"$Actual`" 2>&1"
$ExitCode = $LASTEXITCODE
$ErrorActionPreference = "Stop"
# Exited cleanly after printing the on-demand trace
if ($ExitCode -ne 0) {
  throw "Expected exit code 0, got $ExitCode"
}

Get-Content $Actual

$NormalizedContent = (Get-Content $Actual -Raw) `
  -replace '0x[0-9a-fA-F]+', '0xADDR' `
  -replace '\+0xADDR', '+0xOFFSET' `
  -replace '(?m)^pid:\s+\d+', 'pid:     <PID>' `
  -replace '(?m)^#\d+ ', '# ' `
  -replace '(?m)^# [^\r\n]*(_sigtramp|__restore_rt| _?start \+|__scrt_|BaseThreadInitThunk|RtlUserThreadStart|UnhandledExceptionFilter|KiUserExceptionDispatcher)[^\r\n]*\r?\n  in [^\r\n]*\r?\n', '' `
  -replace '(?m)^# [^\r\n]*\r?\n  in [^\r\n]*(libsystem_|libdyld|/dyld|\\Windows\\)[^\r\n]*\r?\n', '' `
  -replace "`r`n", "`n"

$ExpectedContent = (@"
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
"@) -replace "`r`n", "`n"

$NormalizedContent = $NormalizedContent.TrimEnd()
$ExpectedContent = $ExpectedContent.TrimEnd()
[System.IO.File]::WriteAllText($Normalized, $NormalizedContent)
[System.IO.File]::WriteAllText($Expected, $ExpectedContent)

if ($NormalizedContent -ne $ExpectedContent) {
  Write-Host "=== EXPECTED ==="
  Write-Host $ExpectedContent
  Write-Host "=== ACTUAL (normalized) ==="
  Write-Host $NormalizedContent
  throw "Output did not match expected"
}
