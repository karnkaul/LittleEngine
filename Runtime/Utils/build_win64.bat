@echo off

if "%VCVARS%" == "" (
	echo   ERROR: VCVARS not defined!
	exit 1
)

call "%VCVARS%\vcvars64.bat"

for %%x in (%*) do (
	echo. &  echo == Configuring [%%x] & echo.
	cmake %%x
	echo. & echo == Building [%%x] & echo.
	cmake --build %%x
)
