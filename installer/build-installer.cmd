@echo off
setlocal
set "ISCC=%~1"
if not defined ISCC if exist "%ProgramFiles%\Inno Setup 7\ISCC.exe" set "ISCC=%ProgramFiles%\Inno Setup 7\ISCC.exe"
if not defined ISCC if exist "%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" set "ISCC=%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"
if not defined ISCC (
    echo Inno Setup compiler not found. Install Inno Setup or supply the full ISCC.exe path.
    exit /b 1
)
if not exist "%~dp0..\outputs\Earth-and-Sky\app\Earth-and-Sky.exe" (
    echo Packaged application missing. Build and deploy Earth and Sky first.
    exit /b 1
)
"%ISCC%" "%~dp0Earth-and-Sky.iss"
exit /b %errorlevel%
