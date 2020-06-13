rem Gradle clean is not only slow, but is also daft.  Doesn't clean this project, but apparently the last thing it built.

rem Build

call gradlew clean

rmdir /s /q .gradle
rmdir /s /q build
rmdir /s /q app\.externalNativeBuild

pause

