##################################################################################
# This script builds QOwnNotes and creates a ZIP including all necessary libraries
##################################################################################

# Stop on error
$ErrorActionPreference = "Stop"

#dir -s ..\..\Qt
Write-Host $Env:QT_VERSION
Write-Output "#define RELEASE ""GitHub Actions""" > release.h
qmake6 CONFIG+=release QOwnNotes.pro -r
lrelease QOwnNotes.pro
set CL=/MP
nmake
# Create release directory
New-Item -Path '..\release' -ItemType 'Directory'
# copy the binary to our release path
Copy-Item release\QOwnNotes.exe ..\release
# copy Win64 OpenSSL v1.1.1g DLLs to the release path
Copy-Item ..\build-systems\github\windows\libcrypto-1_1-x64.dll ..\release
Copy-Item ..\build-systems\github\windows\libssl-1_1-x64.dll ..\release
# copy unzip application for updater
Copy-Item ..\appveyor\unzip.exe ..\release
# copy updater script
Copy-Item ..\appveyor\update.bat ..\release
# copy portable mode launcher to the release path
Copy-Item ..\appveyor\QOwnNotesPortable.bat ..\release
# copy translation files
Copy-Item languages\*.qm ..\release
Set-Location ..\release
# Fetching dependencies of QT app
# https://doc.qt.io/qt-6/windows-deployment.html
windeployqt -core -gui -widgets -sql -svg -network -xml -printsupport -qml -quick -quickwidgets -websockets -concurrent QOwnNotes.exe

# Create zip archive
Get-ChildItem
Compress-Archive -Path * -DestinationPath ..\QOwnNotes.zip
# Get sha256 checksum
$Checksum = [string] (Get-FileHash -Path '..\QOwnNotes.zip' -Algorithm 'SHA256').'Hash'.ToLower()
# Create sha256 file with checksum only
Out-File -FilePath '..\QOwnNotes.zip.sha256' -Encoding 'utf8' -InputObject $Checksum
# Create sha256 file with name of file
Out-File -FilePath '..\QOwnNotes.zip.sha256sum' -Encoding 'utf8' -InputObject (
    [string]::Format(
        '{0} *QOwnNotes.zip',
        $Checksum
    )
)
