@echo off
rem QOwnNotes updater script
set source=%1
set destination=%2
rem don't use the variable `unzip` because it's used by unzip.exe!
set unzipExecutable=%3
set unzipCommand=%unzipExecutable% -o %source% -d %destination%
set qownnotesCommand=%destination%\QOwnNotes.exe --after-update %0

echo %0 %*
echo ======================================
echo Updating QOwnNotes in folder:
echo %destination%
echo ======================================
echo Source archive:
echo %source%
echo ======================================
echo Unzip executable:
echo %unzipExecutable%
echo ======================================
echo Executing unzip command:
echo %unzipCommand%
echo ======================================
echo Uncompressing files...
call %%unzipCommand%% 
echo ======================================
echo Deleting temporary files
del %source%
del %unzipExecutable%
echo ======================================
echo Starting QOwnNotes:
echo %qownnotesCommand%
call %%qownnotesCommand%%
echo ======================================
pause