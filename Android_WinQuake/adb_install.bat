rem	<target name="run">
rem		<exec executable="adb">
rem			<arg value="shell"/>
rem			<arg value="am"/>
rem			<arg value="start"/>
rem			<arg value="-a"/>
rem			<arg value="android.intent.action.MAIN"/>
rem			<arg value="-n"/>
rem			<arg value="com.quakedroid.app/.MainActivity"/>
rem		</exec>
rem	</target>

rem adb connect 192.168.1.10:5555
adb install -r app\build\outputs\apk\QuakeDroid-WinQuake-debug.apk
rem rem adb -s 192.168.1.10:5555 install -r app\build\outputs\apk\QuakeDroid-WinQuake-debug.apk
rem adb install -r app\build\outputs\apk\QuakeDroid-WinQuake-debug.apk
pause
