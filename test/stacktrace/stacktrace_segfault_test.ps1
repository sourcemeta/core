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
# CMake's $<TARGET_FILE:> can emit forward slashes; dbghelp returns
# backslashes via GetModuleFileNameA. Normalize so the expected matches.
$StacktraceSegfaultMain = $StacktraceSegfaultMain -replace '/', '\'
$StacktraceLibrary = $StacktraceLibrary -replace '/', '\'
$LibraryPath = if ($StacktraceLibrary -match '\.(lib|a)$') {
  $StacktraceSegfaultMain
} else {
  $StacktraceLibrary
}
$Actual = Join-Path $WorkDir "$Self.actual.txt"
$Normalized = Join-Path $WorkDir "$Self.normalized.txt"
$Expected = Join-Path $WorkDir "$Self.expected.txt"

# Delegate the redirection to cmd.exe: PowerShell wraps native-command stderr
# as ErrorRecord objects whose stringified form (with file/line/category
# metadata) ends up in the captured file. cmd.exe does plain fd-level
# redirection without that wrapping.
$ErrorActionPreference = "Continue"
& cmd.exe /c "`"$StacktraceSegfaultMain`" > `"$Actual`" 2>&1"
$ExitCode = $LASTEXITCODE
$ErrorActionPreference = "Stop"
# Crashed by a fatal exception
if ($ExitCode -eq 0) {
  throw "Expected non-zero exit code, got $ExitCode"
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
signal:  0xADDR (SEH)
pid:     <PID>

# 0xADDR crash_handler +0xOFFSET
  in $LibraryPath
# 0xADDR sourcemeta_core_stacktrace_test::crash_deepest +0xOFFSET
  in $StacktraceSegfaultMain
# 0xADDR sourcemeta_core_stacktrace_test::crash_middle +0xOFFSET
  in $StacktraceSegfaultMain
# 0xADDR main +0xOFFSET
  in $StacktraceSegfaultMain
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
