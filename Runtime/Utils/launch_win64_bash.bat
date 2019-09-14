@echo off

REM This batch script loads vcvars64.bat into the environment, then launches a build-ready bash shell.
REM
REM Requirements:
REM  - `VCVARS` (location of `vcvarsall.bat`) and `bash.exe` (WSL or Git) must be defined in `PATH`

if "%VCVARS%" == "" (
	echo   ERROR: VCVARS not defined! Set it to the path to `vcvarsall.bat`
	exit 1
)

call "%VCVARS%\vcvars64.bat"
bash.exe
