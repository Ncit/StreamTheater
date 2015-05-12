@rem Run.bat for StreamTheater
@if "%1" NEQ "debug" (
    @if "%1" NEQ "release" (
	    @if "%1" NEQ "clean" (
            @echo The first parameter to run.bat must be one of debug, release or clean.
            @goto:EOF
		)
	)
)
@call ..\build.cmd com.vrmatter.streamtheater bin/StreamTheater-debug.apk com.vrmatter.streamtheater.MainActivity %1 %2
