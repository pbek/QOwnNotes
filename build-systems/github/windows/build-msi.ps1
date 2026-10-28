##################################################################################
# This script creates a Windows installer (MSI) from the release folder
# that was created by build-zip-qt6.ps1
##################################################################################

# Stop on error
$ErrorActionPreference = "Stop"

# WiX v5 is used, because later versions require an Open Source Maintenance Fee EULA
$WixVersion = "5.0.2"

$ReleaseDir = (Resolve-Path ..\release).Path
$WxsFile = (Resolve-Path ..\build-systems\github\windows\QOwnNotes.wxs).Path
$IconPath = (Resolve-Path QOwnNotes.ico).Path
$MsiFile = Join-Path (Resolve-Path ..).Path "QOwnNotes.msi"

# Read the version from version.h, MSI versions need to be in the format MAJOR.MINOR.PATCH
$Version = (Select-String -Path version.h -Pattern '"([0-9]+\.[0-9]+\.[0-9]+)"').Matches[0].Groups[1].Value
Write-Host "Creating MSI installer for QOwnNotes $Version"

dotnet tool install --global wix --version $WixVersion
if ($LASTEXITCODE -ne 0) { throw "Installing WiX Toolset failed" }
$Env:PATH = "$Env:USERPROFILE\.dotnet\tools;$Env:PATH"

wix build $WxsFile -arch x64 -d "Version=$Version" -d "IconPath=$IconPath" -bindpath "AppDir=$ReleaseDir" -o $MsiFile
if ($LASTEXITCODE -ne 0) { throw "Building MSI installer failed" }

# Remove debug information that we don't want to publish
Remove-Item -Path ([System.IO.Path]::ChangeExtension($MsiFile, ".wixpdb")) -ErrorAction SilentlyContinue

# Get sha256 checksum
$Checksum = [string] (Get-FileHash -Path $MsiFile -Algorithm 'SHA256').'Hash'.ToLower()
# Create sha256 file with name of file
Out-File -FilePath "$MsiFile.sha256sum" -Encoding 'utf8' -InputObject (
    [string]::Format(
        '{0} *QOwnNotes.msi',
        $Checksum
    )
)

Get-ChildItem $MsiFile, "$MsiFile.sha256sum"
