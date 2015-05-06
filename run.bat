@rem Run.bat for CinemaSDK
@if "%1" NEQ "debug" (
    @if "%1" NEQ "release" (
	    @if "%1" NEQ "clean" (
            @echo The first parameter to run.bat must be one of debug, release or clean.
            @goto:EOF
		)
	)
)
@call ..\build.cmd com.oculus.cinemasdk bin/CinemaSDK-debug.apk com.oculus.cinemasdk.MainActivity %1 %2
