rem  cd "C:\androidprojects\SDL\app\src\main\jni"

rem we are in C:\Dropbox\quakedroid_source_20180401_controllerus_functional2\SDK\SDL2-2.0.8\android-project
rem need to nav to app\src\main\jni

rem sym to here C:\Dropbox\quakedroid_source_20180401_controllerus_functional2\SDK\SDL2-2.0.8

rem Kill the fake folder
rmdir /s /q app\jni\SDL2

cd app\jni
dir
pause
mklink /D SDL2 ..\..\..\SDK\SDL2-2.0.8
dir SDL2
pause

