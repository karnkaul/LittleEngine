@echo off

REM This batch script is an alternative to `install_builds.sh`.
REM
REM Requirements:
REM  - `VCVARS` (location of `vcvarsall.bat`) must be defined in `PATH`

if "%VCVARS%" == "" (
	echo   ERROR: VCVARS not defined! Set it to the path to `vcvarsall.bat`
	exit 1
)

call "%VCVARS%\vcvars64.bat"

for %%x in (%*) do (
	echo. &  echo == Configuring [%%x] & echo.
	cmake %%x
	echo. & echo == Building [%%x] & echo.
	cmake --build %%x
)
