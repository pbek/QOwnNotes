##################################################################################
# This script builds QOwnNotes and creates a ZIP including all necessary libraries
##################################################################################

#dir -s ..\..\Qt
echo "#define RELEASE ""GitHub Actions""" > release.h
qmake QOwnNotes.pro -r
lrelease QOwnNotes.pro
make
md ..\release
# copy the binary to our release path
copy release\QOwnNotes.exe ..\release
# copy Win64 OpenSSL v1.1.1g DLLs to the release path
copy ..\build-systems\github\windows\libcrypto-1_1-x64.dll ..\release
copy ..\build-systems\github\windows\libssl-1_1-x64.dll ..\release
# copy unzip application for updater
copy ..\appveyor\unzip.exe ..\release
# copy updater script
copy ..\appveyor\update.bat ..\release
# copy portable mode launcher to the release path
copy ..\appveyor\QOwnNotesPortable.bat ..\release
# copy translation files
copy languages\*.qm ..\release
cd ..\release
# fetching dependencies of QT app
# http://doc.qt.io/qt-5/windows-deployment.html
# Bug in Qt 5.14+: https://stackoverflow.com/questions/61045959/windeployqt-error-unable-to-find-the-platform-plugin
# Don't use "--release"! (maybe because of debug log?)
windeployqt --debug QOwnNotes.exe
# these dlls where missed by windeployqt
copy ..\..\Qt\5.15.2\mingw81_64\bin\libwinpthread-1.dll .
copy ..\..\Qt\5.15.2\mingw81_64\bin\libgcc_s_seh-1.dll .
# this dll didn't work when released by windeployqt
# important: this dll needs to be updated when a new version of Qt is used!
# search for it in the mingw* folder of your local installation of Qt
# Update: we are trying a direct copy again
copy ..\..\Qt\5.15.2\mingw81_64\bin\libstdc++-6.dll .
# create zip archive
Compress-Archive -Path * -DestinationPath ..\QOwnNotes.zip
$(CertUtil -hashfile ..\QOwnNotes.zip SHA256)[1] -replace " ","" | Out-File -FilePath ..\QOwnNotes.zip.sha256
