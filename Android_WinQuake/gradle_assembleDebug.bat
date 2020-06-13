rem Build

call gradlew assembleDebug

if %ERRORLEVEL% EQU 0 echo OK

rundll32 user32.dll,MessageBeep

pause
pause

