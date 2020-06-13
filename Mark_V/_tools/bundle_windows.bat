@echo off
echo Constructing bundle ...
set "checkFile=bundle_windows.bat"
set "potentialErrorMessage=Sentry file found %checkFile% means we being run from the tools folder, which is not correct"
if exist %checkFile% goto :errorFailed

%ToolsDir%\bundler
%ToolsDir%\bin2h -c _binary_bundle_pak < bundle.pak > bundle_bin2.h

echo Bundle was built successfully.
pause

goto :EOF

:errorFailed
@echo ******* ERROR *******
@echo ** ERROR: %potentialErrorMessage%

pause
