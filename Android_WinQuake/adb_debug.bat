set "LOGTAG=CoreMain"

rem call adb shell am start -a android.intention.action.MAIN -n com.quakedroid.app/org.libsdl.app.SDLActivity

adb logcat -c
rem "We need to filter this via project tag or something"  Log tag is CoreMain
start "New Window" cmd  /k adb logcat -s "%LOGTAG%"


pause