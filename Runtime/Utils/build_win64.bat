@echo off

if "%VCVARS%" == "" (
	echo   ERROR: VCVARS not defined!
	exit 1
)

call "%VCVARS%\vcvars64.bat"

for %%x in (%*) do (
	echo. &  echo == Configuring [Project%%x] & echo.
	cmake ..\..\Project%%x
	echo. & echo == Building [Project%%x] & echo.
	cmake --build ..\..\Project%%x
)