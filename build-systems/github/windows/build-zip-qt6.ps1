##################################################################################
# This script builds QOwnNotes and creates a ZIP including all necessary libraries
##################################################################################

# Stop on error
$ErrorActionPreference = "Stop"

Write-Host $Env:QT_VERSION

Write-Host "Print content of $Env:QT_ROOT_DIR"
dir -s $Env:QT_ROOT_DIR

Write-Output "#define RELEASE ""GitHub Actions""" > release.h
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DQON_QT6_BUILD=ON -DBUILD_WITH_BUNDLED_LIBGIT2=ON
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed" }
cmake --build build-release --parallel
if ($LASTEXITCODE -ne 0) { throw "CMake build failed" }
# Create release directory and languages subdirectory
New-Item -Path '..\release\languages' -ItemType 'Directory' -Force
# copy the binary to our release path
Copy-Item build-release\QOwnNotes.exe ..\release
# copy Win64 OpenSSL v1.1.1g DLLs to the release path
Copy-Item ..\build-systems\github\windows\libcrypto-1_1-x64.dll ..\release
Copy-Item ..\build-systems\github\windows\libssl-1_1-x64.dll ..\release
# copy unzip application for updater
Copy-Item ..\appveyor\unzip.exe ..\release
# copy updater script
Copy-Item ..\appveyor\update.bat ..\release
# copy portable mode launcher to the release path
Copy-Item ..\appveyor\QOwnNotesPortable.bat ..\release
# copy translation files into languages subdirectory
Copy-Item build-release\*.qm ..\release\languages
Set-Location ..\release
# Fetching dependencies of QT app
# https://doc.qt.io/qt-6/windows-deployment.html
# Note: Library "qmltooling" and "declarative" are not existing anymore
windeployqt --no-compiler-runtime -core -gui -widgets -sql -svg -network -xml -printsupport -qml -quick -quickwidgets -websockets -concurrent QOwnNotes.exe

# Remove icuuc.dll as it causes issues if the other ICU DLLs are not installed
# https://github.com/pbek/QOwnNotes/issues/3445#issuecomment-3777501835
Remove-Item icuuc.dll

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
