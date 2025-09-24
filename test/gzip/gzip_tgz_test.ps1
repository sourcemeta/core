param(
    [Parameter(Mandatory=$true)]
    [string]$Maker,
    [Parameter(Mandatory=$true)]
    [string]$Output,
    [Parameter(Mandatory=$true, ValueFromRemainingArguments=$true)]
    [string[]]$Args
)

$ErrorActionPreference = "Stop"
if (($Args.Count % 2) -ne 0) {
    Write-Error "Usage: $($MyInvocation.MyCommand) <maker> <output.tgz> <file1> <content1> [file2] [content2] ..."
    exit 1
}

& $Maker $Output @Args

$TempDir = Join-Path ([System.IO.Path]::GetTempPath()) ([System.Guid]::NewGuid().ToString())
New-Item -ItemType Directory -Path $TempDir -Force | Out-Null

try {
    tar -xzf $Output -C $TempDir
    for ($i = 0; $i -lt $Args.Count; $i += 2) {
        $ExpectedFile = $Args[$i]
        $ExpectedContent = $Args[$i + 1]
        $ExpectedContent | Out-File -FilePath "$TempDir\$ExpectedFile.expected" -NoNewline -Encoding utf8
        $result = Compare-Object (Get-Content "$TempDir\$ExpectedFile") (Get-Content "$TempDir\$ExpectedFile.expected")
        if ($result) {
            exit 1
        }
    }
} finally {
    Remove-Item -Path $TempDir -Recurse -Force
}
