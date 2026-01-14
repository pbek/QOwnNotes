##################################################################################
# This script builds QOwnNotes and creates a ZIP including all necessary libraries
##################################################################################

#dir -s ..\..\Qt
Write-Output "#define RELEASE ""GitHub Actions""" > release.h
qmake QOwnNotes.pro -r
lrelease QOwnNotes.pro
make
# Create release directory and languages subdirectory
New-Item -Path '..\release\languages' -ItemType 'Directory' -Force
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
# copy translation files into languages subdirectory
Copy-Item languages\*.qm ..\release\languages
Set-Location ..\release
# fetching dependencies of QT app
# http://doc.qt.io/qt-5/windows-deployment.html
# Bug in Qt 5.14+: https://stackoverflow.com/questions/61045959/windeployqt-error-unable-to-find-the-platform-plugin
# Don't use "--release"! (maybe because of debug log?)
windeployqt --debug QOwnNotes.exe
# these dlls where missed by windeployqt
Copy-Item ..\..\Qt\5.15.2\mingw81_64\bin\libwinpthread-1.dll .
Copy-Item ..\..\Qt\5.15.2\mingw81_64\bin\libgcc_s_seh-1.dll .
# this dll didn't work when released by windeployqt
# important: this dll needs to be updated when a new version of Qt is used!
# search for it in the mingw* folder of your local installation of Qt
# Update: we are trying a direct copy again
Copy-Item ..\..\Qt\5.15.2\mingw81_64\bin\libstdc++-6.dll .
# Create zip archive
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
