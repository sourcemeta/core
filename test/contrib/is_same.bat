@echo off
setlocal

set "PROGRAM=%1"
set "EXPECTED=%2"
set "ARGUMENT=%3"

set "TEMP_OUTPUT=test-output-%RANDOM%-%RANDOM%.txt"
echo Running %PROGRAM% %ARGUMENT% 1>&2
"%PROGRAM%" "%ARGUMENT%" >"%TEMP_OUTPUT%"
fc "%TEMP_OUTPUT%" "%EXPECTED%" > nul
if errorlevel 1 (
  echo Got 1>&2
  type "%TEMP_OUTPUT%"
  del "%TEMP_OUTPUT%"
  echo Expected 1>&2
  type "%EXPECTED%"
  exit /b 1
) else (
  del "%TEMP_OUTPUT%"
)
