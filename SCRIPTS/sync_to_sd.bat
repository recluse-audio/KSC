@echo off
REM Sync KSC game files from repo to D:\KSC\ (SD card staging)
REM Uses robocopy for efficient mirroring (only copies changed files)

set SOURCE=C:\REPOS\PROJECTS\KSC\KSC_DATA
set DEST=D:\KSC_DATA

echo Syncing KSC/KSC_DATA/ files to SD card...
echo Source: %SOURCE%
echo Dest:   %DEST%
echo.

REM Mirror everything in KSC/KSC_DATA/ to D:/KSC_DATA/
robocopy "%SOURCE%" "%DEST%" /MIR /XF *.ino *.cpp *.h .gitignore /XD .git /R:1 /W:1 /NP

echo.
echo ================================================
echo Sync complete!
echo ================================================
echo.
echo Next step: Copy D:\KSC\ to your SD card root
pause
