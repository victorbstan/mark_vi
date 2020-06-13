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


rem adb install app\build\outputs\apk\app-debug.apk
rem adb shell pm clear com.rufus.app  // Didn't work
adb uninstall com.quakedroid.app
pause
